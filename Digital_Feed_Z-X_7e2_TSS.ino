// 25.06.2025
// Доработан Александром Кочкиным Для Виктора !!!
// - псевдолинейки с обнулением
// - автоповтор кнопок с ускорением
// - перескок джойстика через нейтраль
// - контроль джойстика при старте
// - минимизация обработки в прерывании
// - хардверная отладка/тайминг на 42 пине
// - моргание лимитами
//
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

// ОТЛАДКА !!!
// которые "без понимания" - не включать !!!
//
//#define DEBUG_TIME_ON_LOOP
#define DEBUG_TIME_ON_INT0
//#define DEBUG_TIME_ON_TIMER5_COMPA

//////////////////////////////////
// ***** ПАРАМЕТРЫ ЖЕЛЕЗА ***** //

#define ENC_LINE_PER_REV     2500//2500     // Кол-во линий энкодера на 1 оборот шпинделя

#define MOTOR_Z_STEP_PER_REV 200      // Кол-во шагов на оборот винта Z, продольная (мотор)
#define SCREW_Z              200      // Шаг продольного винта Z в сотках, 2.0мм
#define McSTEP_Z             4        // Микрошаг, ось Z, продольная
#define REBOUND_Z            400      // отскоки задавать очень желательно кратно Кделения McSTEP_?

#define MOTOR_X_STEP_PER_REV 200      // Кол-во шагов на оборот винта X, поперечная (мотор)
#define SCREW_X              200      // Шаг поперечного винта X в сотках, 2.0мм
#define McSTEP_X             4        // Микрошаг, ось X, поперечная
#define REBOUND_X            80       // Отскок резца в микрошагах, для авторезьбы, должен быть больше люфта поперечки сейчас (0.132мм)
//
#define THRD_ACCEL           25       // К.деления с которого будем ускоряться на Резьбах, Accel+Ks должен быть < 255
#define FEED_ACCEL           15        // Жесткость разгона на подачах, бОльше значение - короче разгон.
//
#define MIN_FEED             2        // Желаемая минимальная подача  в сотках/оборот, 0.02мм/об
#define MAX_FEED             150      // Желаемая максимальная подача в сотках/оборот, 1.5мм/об
#define MIN_aFEED            5        // Желаемая минимальная подача  в mm/минуту, 5 мм/мин
#define MAX_aFEED            400      // Желаемая максимальная подача в mm/минуту, 400 мм/мин

// Ускоренные перемещения
#define MAX_RAPID_MOTION     50                       // Меньше - бОльшая конечная скорость           //16000000/32/((25+1)*2)/800*60=721rpm
#define MIN_RAPID_MOTION    (MAX_RAPID_MOTION + 150)  // Больше - мЕньшая начальная скорость, max 255 //16000000/32/((150+25+1)*2)/800*60=107rpm
#define REPEAt              (McSTEP_Z * 1)            // Кол-во повторов для постоянной скорости в пределах полного шага    
// Длительность разгона = 150/2*REPEAT(4)/Microstep(4) = 75 полных шагов цикл ускорения
// Ручной энкодер (100 линий)
#define HC_SCALE_1           1        // 1-e положение, масштаб = 1сотка/тик = 1мм/оборот
#define HC_SCALE_10          10       // 2-e положение, масштаб = 10соток/тик = 10мм/оборот
#define HC_START_SPEED_1     250      // старт РГИ, 250000/(250+1)/800*60/2 = 37rpm
#define HC_MAX_SPEED_1       150      // максимум скорости РГИ, 250000/(150+1)/800*60/2 = 62rpm
#define HC_START_SPEED_10    150      // старт РГИ, 250000/(150+1)/800*60/2 = 62rpm
#define HC_MAX_SPEED_10      23       // максимум скорости РГИ, 250000/(23+1)/800*60/2 = 391rpm
#define HC_X_DIR             1        // 1-поперечка по часовой, 0-против

//////////////////////////////////////////////////////////////////////////////////////////////////

  int rebound_x = REBOUND_X;
  int rebound_x_step = MIN_FEED/(float)((float)SCREW_X/(float)(MOTOR_X_STEP_PER_REV*McSTEP_X));

  int rebound_z = REBOUND_Z;
  int rebound_z_step = MIN_FEED/(float)((float)SCREW_Z/(float)(MOTOR_Z_STEP_PER_REV*McSTEP_Z));

  long DRO_Shift_X = 0;
  long DRO_Shift_Z = 0;

  bool flag_end_operation = false;
  bool pass_end_flag = false;
  
//////////////////////////////////////////////////////////////////////////////////////////////////
#define a  (uint32_t)(ENC_LINE_PER_REV / ((float)MOTOR_Z_STEP_PER_REV * McSTEP_Z * MIN_FEED / SCREW_Z) /2 +0.5)
static_assert(a <= 255, "Неверно задано значение MIN_FEED");
#define b  (uint32_t)(ENC_LINE_PER_REV / ((float)MOTOR_Z_STEP_PER_REV * McSTEP_Z * MAX_FEED / SCREW_Z) /2 +0.5)
static_assert(b > 1, "Неверно задано значение MAX_FEED");
#define c  250000 / ((uint32_t)MIN_aFEED * MOTOR_Z_STEP_PER_REV * McSTEP_Z / ((uint32_t)60 * SCREW_Z / 100) * 2) -1
static_assert(c <= 65535, "Неверно задано значение MIN_aFEED");
#define d  250000 / ((uint32_t)MAX_aFEED * MOTOR_Z_STEP_PER_REV * McSTEP_Z / ((uint32_t)60 * SCREW_Z / 100) * 2) -1
static_assert(d > 1, "Неверно задано значение MAX_aFEED");

#define e  (uint32_t)(ENC_LINE_PER_REV / ((float)MOTOR_X_STEP_PER_REV * McSTEP_X * MIN_FEED / SCREW_X) /2 +0.5)
static_assert(e <= 255, "Неверно задано значение MIN_FEED");
#define f  (uint32_t)(ENC_LINE_PER_REV / ((float)MOTOR_X_STEP_PER_REV * McSTEP_X * MAX_FEED / SCREW_X) /2 +0.5)
static_assert(f > 1, "Неверно задано значение MAX_FEED");
#define g  250000 / ((uint32_t)MIN_aFEED * MOTOR_X_STEP_PER_REV * McSTEP_X / ((uint32_t)60 * SCREW_X / 100) * 2) -1
static_assert(g <= 65535, "Неверно задано значение MIN_aFEED");
#define h  250000 / ((uint32_t)MAX_aFEED * MOTOR_X_STEP_PER_REV * McSTEP_X / ((uint32_t)60 * SCREW_X / 100) * 2) -1
static_assert(h > 1, "Неверно задано значение MAX_aFEED");
//////////////////////////////////////////////////////////

// ***** MY CONSTANT *****
#define CW            0
#define CCW           1
#define ON            1
#define OFF           0

// ***** LCD *****
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
char LCD_Row_1[17];
char LCD_Row_2[17];

#define Beeper_Init()          DDRH = B01100010;\
                               PORTH = B10011101    // LCD-H5,H6 Buzzer-PH1_Pin16
#define Beeper_On()            PORTH &= ~(1<<1)     // Pin16 0
#define Beeper_Off()           PORTH |= (1<<1)      // Pin16 1


// ***** Stepper Motor *****
//#define Motor_Init()           DDRL = B11111111
#define Motor_Init()           DDRL = B11111011;\
                               PORTL= B00000100   // PL2 (D47) - вход Т5-ext.clk, 43,44(Dir)45,46(Ena)49,48(Step)

#define Flags_Init()           DDRG |= B11111111;\
                               DDRF |= (1 << 5)   // G0–G5 как выходы, F5 (A5) как выход

/**
 * @brief Установить бит в порту (SBI)
 * @param port Порт, например PORTL
 * @param bit Номер бита в порту (0–7)
 */
  #define SET_BIT(port, bit)    (_SFR_IO8(_SFR_IO_ADDR(port)) |=  (1 << (bit)))  // Установить бит// SBI, 2 такта, 0.125 мкс @16 МГц

/**
 * @brief Сбросить бит в порту (CBI)
 * @param port Порт, например PORTL
 * @param bit Номер бита в порту (0–7)
 */
  #define CLEAR_BIT(port, bit)  (_SFR_IO8(_SFR_IO_ADDR(port)) &= ~(1 << (bit)))  // Сбросить бит// CBI, 2 такта, 0.125 мкс @16 МГц

/**
 * @brief Инвертировать бит в порту (эквивалент PINx ^= (1 << bit))
 */
  #define TOGGLE_BIT(port, bit)  (_SFR_IO8(_SFR_IO_ADDR(port)) ^= (1 << (bit)))  ///< Инвертировать бит// IN OUT - 4 такта
  #define INVERT_BIT(port, bit)  (_SFR_IO8(_SFR_IO_ADDR(port)) ^=  (1 << (bit))) ///< Инвертировать бит// IN OUT - 4 такта

  /*
  #define Motor_Z_SetPulse()     PORTL &= ~(1<<0)     // Pin49 0
  #define Motor_Z_RemovePulse()  PORTL |= (1<<0)      // Pin49 1
  #define Motor_Z_InvertPulse()  PORTL ^= (1<<0)      // Pin49
  #define Read_Z_State           (PINL & (1<<0))

  #define Motor_X_SetPulse()     PORTL &= ~(1<<1)     // Pin48 0
  #define Motor_X_RemovePulse()  PORTL |= (1<<1)      // Pin48 1
  #define Motor_X_InvertPulse()  PORTL ^= (1<<1)      // Pin48
  #define Read_X_State           (PINL & (1<<1))
  */
  
  #define Motor_Z_SetPulse()     CLEAR_BIT(PORTL, 0)//PORTL &= ~(1<<0)    // Pin49 0
  #define Motor_Z_RemovePulse()  SET_BIT(PORTL, 0)//PORTL |=  (1<<0)    // Pin49 1
  #define Motor_Z_InvertPulse()  PORTL ^=  (1<<0)    // Pin49
  #define Read_Z_State           (PINL & (1<<0))

  #define Motor_X_SetPulse()     CLEAR_BIT(PORTL, 1)//PORTL &=~(1<<1)      // Pin48 0
  #define Motor_X_RemovePulse()  SET_BIT(PORTL, 1)//PORTL |= (1<<1)      // Pin48 1
  #define Motor_X_InvertPulse()  PORTL ^= (1<<1)      // Pin48
  #define Read_X_State           (PINL &  (1<<1))

  #define Motor_Z_CW()           PORTL &= ~(1<<6)    // Pin43 0
  #define Motor_Z_CCW()          PORTL |= (1<<6)     // Pin43 1

  #define Motor_X_CW()           PORTL &= ~(1<<5)    // Pin44 0
  #define Motor_X_CCW()          PORTL |= (1<<5)     // Pin44 1

  //#define Motor_Z_Enable()   do {PORTL |= (1<<4); _delay_us(10);} while(0)   // Pin45 1
  #define Motor_Z_Enable()       PORTL |= (1<<4)      // Pin45 1
  #define Motor_Z_Disable()      PORTL &= ~(1<<4)     // Pin45 0
  #define Read_Z_Ena_State       (PINL & (1<<4))

  //#define Motor_X_Enable()   do {PORTL |= (1<<3); _delay_us(10);} while(0)   // Pin46 1
  #define Motor_X_Enable()       PORTL |= (1<<3)     // Pin46 1
  #define Motor_X_Disable()      PORTL &= ~(1<<3)    // Pin46 0
  #define Read_X_Ena_State       (PINL & (1<<3))
  
  /*
  // --- Motor Z Pulse (Pin49 = PORTL0) ---
  #define Motor_Z_SetPulse()      __asm__ __volatile__("cbi 0x10, 0")  // PORTL &= ~(1<<0)
  #define Motor_Z_RemovePulse()   __asm__ __volatile__("sbi 0x10, 0")  // PORTL |=  (1<<0)

  #define Motor_Z_InvertPulse()   (PORTL ^= (1 << 0))                  // Toggle
  #define Read_Z_State            (PINL & (1 << 0))

  // --- Motor X Pulse (Pin48 = PORTL1) ---
  #define Motor_X_SetPulse()      __asm__ __volatile__("cbi 0x10, 1")
  #define Motor_X_RemovePulse()   __asm__ __volatile__("sbi 0x10, 1")

  #define Motor_X_InvertPulse()   (PORTL ^= (1 << 1))
  #define Read_X_State            (PINL & (1 << 1))

  // --- Motor Z Direction (Pin43 = PORTL6) ---
  #define Motor_Z_CW()            __asm__ __volatile__("cbi 0x10, 6")
  #define Motor_Z_CCW()           __asm__ __volatile__("sbi 0x10, 6")

  // --- Motor X Direction (Pin44 = PORTL5) ---
  #define Motor_X_CW()            __asm__ __volatile__("cbi 0x10, 5")
  #define Motor_X_CCW()           __asm__ __volatile__("sbi 0x10, 5")

  // --- Motor Z Enable (Pin45 = PORTL4) ---
  #define Motor_Z_Enable()        __asm__ __volatile__("sbi 0x10, 4")
  #define Motor_Z_Disable()       __asm__ __volatile__("cbi 0x10, 4")

  #define Read_Z_Ena_State        (PINL & (1 << 4))

  // --- Motor X Enable (Pin46 = PORTL3) ---
  #define Motor_X_Enable()        __asm__ __volatile__("sbi 0x10, 3")
  #define Motor_X_Disable()       __asm__ __volatile__("cbi 0x10, 3")

  #define Read_X_Ena_State        (PINL & (1 << 3))
*/
// ***** Tacho *****
//#define TachoSetPulse()        PORTL |= (1<<7)      // Pin42 1
//#define TachoRemovePulse()     PORTL &= ~(1<<7)     // Pin42 0
// The same but correct for DEBUGGING & TIME CHECK
#define DIAG_HIGH()            SET_BIT(PORTL, 7)      //PORTL |= (1<<7)      // Pin42 1
#define DIAG_LOW()             CLEAR_BIT(PORTL, 7)    //PORTL &= ~(1<<7)     // Pin42 0
//#define DIAG_HIGH()  __asm__ __volatile__("sbi 0x10, 7")
//#define DIAG_LOW()  __asm__ __volatile__("cbi 0x10, 7")

// ***** Encoder *****
int enc_tick_x1 = (ENC_LINE_PER_REV * 1);    // Рабочее кол-во импульсов x1
int enc_tick_x2 = (ENC_LINE_PER_REV * 2);    // Рабочее кол-во импульсов x2
int enc_tick_x4 = (ENC_LINE_PER_REV * 4);    // Рабочее кол-во импульсов x4

#define ENC_TICK              enc_tick_x2 //(ENC_LINE_PER_REV * 2)    // Рабочее кол-во импульсов

#define Encoder_Init()        DDRD  = B00000000;\
                              PORTD = B11111111        // подтяжка PIN_21, 20, 19, 18

#define Enc_Read              (PIND & (1<<1))
#define Enc_Ch_A              (PIND & (1<<0))
#define Enc_Ch_B              (PIND & (1<<1))



// ***** Hand_Coder *****            // Z/X: Input-E4,E5, подтяжка-E4,E5, X1/X10: Input-J0,J1, подтяжка-J0,J1.
#define Hand_Init()           DDRE  = B00000000;\
                              PORTE = B11111111;\
                              DDRJ  = B00000000;\
                              PORTJ = B11111111

#define Hand_Ch_A             (PIND & (1<<2))
#define Hand_Ch_B             (PIND & (1<<3))

#define Hand_Axis_Read        (PINE & B00110000)       // E4,E5
byte Hand_Axis_Old = 0;

#define Hand_Scale_Read       (PINJ & B00000011)      // J0,J1
byte Hand_Scale_Old = 0;


//***** Limit Buttons & LEDs *****
#define Limit_Init()          DDRA  = B10101010;\
                              PORTA = B01010101    // IN-A0,A2,A4,A6, OUT-A1,A3,A5,A7, подтяжка

#define Limit_Buttons_Read    (PINA & B01010101)    // PA0 Pin22, PA2 Pin24, PA4 Pin26, PA6 Pin28.
byte Limit_Button_Old = 0;

#define Limit_Rear_LED_On()    PORTA &= ~(1<<1)     // PA1, Pin23 0
#define Limit_Rear_LED_Off()   PORTA |= (1<<1)      // PA1, Pin23 1
#define Limit_Front_LED_On()   PORTA &= ~(1<<3)     // PA3, Pin25 0
#define Limit_Front_LED_Off()  PORTA |= (1<<3)      // PA3, Pin25 1
#define Limit_Right_LED_On()   PORTA &= ~(1<<5)     // PA5, Pin27 0
#define Limit_Right_LED_Off()  PORTA |= (1<<5)      // PA5, Pin27 1
#define Limit_Left_LED_On()    PORTA &= ~(1<<7)     // PA7, Pin29 0
#define Limit_Left_LED_Off()   PORTA |= (1<<7)      // PA7, Pin29 1

#define Limit_Pos_Max          1073741824
#define Limit_Pos_Min         -1073741824

//////////////////
#define Menu_Buttons_Init()    DDRF  = B00000000;\
                               PORTF = B11111111;

#define Buttons_Read           (PINF & B00001111)    // Pin_A0 PF0, Pin_A1 PF1, Pin_A2 PF2, Pin_A3 PF3, Pin_A4 PF4.
byte Button_Old = 0;

#define Button_Sel_Read        (PINF & B00010000)    // Pin_A4 PF4
byte Button_Sel_Old = 0;
bool key_sel_flag = false;

//////////////////
#define Joy_Init()             DDRK  = B00000000;\
                               PORTK = B11111111;    // подтяжка PIN_A8, A9, A10, A11, A12 // Submode Sw: A13, A14, A15

#define Joy_Read              (PINK & B00001111)     // PK0 PK1 PK2 PK3
#define Button_Rapid          (PINK & B00010000)     // PK4
byte Joy_Old = 0;

////////////////////
#define Submode_Read          (PINK & B11100000)     // PK5 PK6 PK7
byte Submode_New = 0;
byte Submode_Old = 0;


// ***** Mode *****
#define Mode_Switch_Init()     DDRC = B00000000;\
                               PORTC = B11111111;        // подтяжка PORT_A, ОБЯЗАТЕЛЬНА! внешняя подтяжка к +5 через 1К резисторы
#define Mode_Read             (PINC & B11111111)
byte Mode_Old = 0;


enum Mode
{
  Mode_Thread = 1,
  Mode_Feed,
  Mode_aFeed,
  Mode_Cone_L,
  Mode_Cone_R,
  Mode_Reserve,
  Mode_Sphere,
  Mode_Divider
};

enum Sub_Mode_Thread
{
  Sub_Mode_Thread_Int = 1,
  Sub_Mode_Thread_Man,
  Sub_Mode_Thread_Ext,
};

enum Sub_Mode_Feed
{
  Sub_Mode_Feed_Int = 1,
  Sub_Mode_Feed_Man,
  Sub_Mode_Feed_Ext,
};

enum Sub_Mode_aFeed
{
  Sub_Mode_aFeed_Int = 1,
  Sub_Mode_aFeed_Man,
  Sub_Mode_aFeed_Ext,
};

enum Sub_Mode_Cone
{
  Sub_Mode_Cone_Int = 1,
  Sub_Mode_Cone_Man,
  Sub_Mode_Cone_Ext,
};

enum Sub_Mode_Sphere
{
  Sub_Mode_Sphere_Int = 1,
  Sub_Mode_Sphere_Man,
  Sub_Mode_Sphere_Ext,
};


//***** Ускоренное перемещение *****
#define Timer2_Init()         TCCR2A = (1<<WGM21);\
                              TCCR2B = (1<<CS20)|(1<<CS21); // 16MHz/32 = 500kHz

//***** РГИ перемещение *****
#define Timer3_Init()         TCCR3A = 0;\
                              TCCR3B = (1<<WGM32)|(1<<CS30)|(1<<CS31); // 16MHz/32 = 250kHz

// ***** aFeed *****
#define Timer4_Init()         TCCR4A = 0;\
                              TCCR4B = (1<<WGM42)|(1<<CS40)|(1<<CS41); // 16MHz/32 = 250kHz

// ***** Feed *****
#define Timer5_Init()         TCCR5A = 0;\
                              TCCR5B = (1<<WGM52) | (1<<CS52) | (1<<CS51) | (1<<CS50);\
                              TCCR5A &= ~(1 << COM5C1)


// ***** Cone *****
struct cone_info_type
{
  byte Cs_Div;
  int  Cm_Div;
  char Cone_Print[6];
};
const cone_info_type Cone_Info[] =
{
  { 2, 0, "45\xDF" },
  { 3, 4641, "30\xDF" },
  { 7, 4641, "15\xDF" },
  { 11, 3426, "10\xDF" },
  { 14, 2307, " 8\xDF" },
  { 76, 8480, " KM0" },
  { 80, 1880, " KM1" },
  { 80, 800, " KM2" },
  { 79, 6880, " KM3" },
  { 77, 160, " KM4" },
  { 76, 80, " KM5" },
  { 76, 7200, " KM6" },
  { 16, 0, " 1:4" },
  { 20, 0, " 1:5" },
  { 28, 0, " 1:7" },
  { 40, 0, "1:10" },
  { 64, 0, "1:16" },
  { 80, 0, "1:20" },
  { 96, 0, "1:24" },
  { 120, 0, "1:30" },
  { 200, 0, "1:50" },
  { 36, 5714, "7:64" },
};
#define TOTAL_CONE (sizeof(Cone_Info) / sizeof(Cone_Info[0]))


// ***** Threads *****
struct thread_info_type
{
  byte Ks_Div_Z;
  int  Km_Div_Z;
  byte Ks_Div_X;
  int  Km_Div_X;
  char Thread_Print[7];
  float Step;
  byte Pass;
  char Limit_Print[8];
};
const thread_info_type Thread_Info[] =
{
  { 72, 0, 72, 0, "0.25mm", 0.25, 4, " 750" },
  { 60, 0, 60, 0, "0.30mm", 0.3, 4, " 750" },
  { 51, 4286, 51, 4286, "0.35mm", 0.35, 4, " 750" },
  { 45, 0, 45, 0, "0.40mm", 0.4, 4, " 750" },
  { 36, 0, 36, 0, "0.50mm", 0.5, 4, " 750" },
  { 30, 0, 30, 0, "0.60mm", 0.6, 4, " 750" },
  { 25, 7143, 25, 7143, "0.70mm", 0.7, 4, " 750" },
  { 24, 0, 24, 0, "0.75mm", 0.75, 5, " 750" },
  { 22, 5000, 22, 5000, "0.80mm", 0.8, 5, " 700" },
  { 18, 0, 18, 0, "1.00mm", 1, 6, " 560" },
  { 14, 4000, 14, 4000, "1.25mm", 1.25, 7, " 460" },
  { 12, 0, 12, 0, "1.50mm", 1.5, 7, " 380" },
  { 10, 2857, 10, 2857, "1.75mm", 1.75, 8, " 320" },
  { 9, 0, 9, 0, "2.00mm", 2, 9, " 280" },
  { 7, 2000, 7, 2000, "2.50mm", 2.5, 11, " 220" },
  { 6, 0, 6, 0, "3.00mm", 3, 15, " 190" },
  { 4, 5000, 4, 5000, "4.00mm", 4, 22, " 140" },
  { 51, 236, 51, 236, "72 tpi", 0.353, 4, " 750" },
  { 43, 9370, 43, 9370, "62 tpi", 0.41, 4, " 750" },
  { 42, 5197, 42, 5197, "60 tpi", 0.423, 4, " 750" },
  { 39, 6850, 39, 6850, "56 tpi", 0.454, 4, " 750" },
  { 34, 157, 34, 157, "48 tpi", 0.529, 4, " 750" },
  { 31, 1811, 31, 1811, "44 tpi", 0.577, 4, " 750" },
  { 28, 3465, 28, 3465, "40 tpi", 0.635, 4, " 750" },
  { 25, 5118, 25, 5118, "36 tpi", 0.706, 4, " 750" },
  { 22, 6772, 22, 6772, "32 tpi", 0.794, 5, " 750" },
  { 21, 2598, 21, 2598, "30 tpi", 0.847, 5, " 710" },
  { 19, 8425, 19, 8425, "28 tpi", 0.907, 5, " 650" },
  { 18, 4252, 18, 4252, "26 tpi", 0.977, 5, " 600" },
  { 17, 7165, 17, 7165, "25 tpi", 1.016, 6, " 570" },
  { 17, 79, 17, 79, "24 tpi", 1.058, 6, " 500" },
  { 15, 5906, 15, 5906, "22 tpi", 1.155, 6, " 450" },
  { 14, 1732, 14, 1732, "20 tpi", 1.27, 7, " 440" },
  { 13, 4646, 13, 4646, "19 tpi", 1.337, 7, " 420" },
  { 12, 7559, 12, 7559, "18 tpi", 1.411, 7, " 380" },
  { 11, 3386, 11, 3386, "16 tpi", 1.588, 8, " 350" },
  { 9, 9213, 9, 9213, "14 tpi", 1.814, 9, " 320" },
  { 9, 2126, 9, 2126, "13 tpi", 1.954, 10, " 270" },
  { 8, 5039, 8, 5039, "12 tpi", 2.117, 11, " 220" },
  { 7, 866, 7, 866, "10tpi", 2.54, 14, " 190" },
  { 6, 3780, 6, 3780, " 9 tpi", 2.822, 16, " 170" },
  { 5, 6693, 5, 6693, " 8 tpi", 3.175, 19, " 150" },
  { 4, 2520, 4, 2520, " 6 tpi", 4.233, 24, " 140" },
};
#define TOTAL_THREADS (sizeof(Thread_Info) / sizeof(Thread_Info[0]))
#define THRD_PASS_FINISH   3 // THRD_PS_FN ???

//#define Thrd_Accel_Err Thread_Info[0].Ks_Div_Z                 // неверно задано ускорение
//static_assert(Thrd_Accel_Err + THRD_ACCEL <= 255, "Неверно задано значение THRD_ACCEL");


// ***** Interrupts *****
#define INT0_Init()               EICRA |= (1<<ISC00)
#define INT1_Init()               EICRA |= (1<<ISC10)
#define INT2_Init()               EICRA |= (1<<ISC20)

#define Enable_INT0()             EIMSK |= (1<<INT0)
#define Disable_INT0()            EIMSK &= ~(1<<INT0)
#define Enable_INT1()             EIMSK |= (1<<INT1)
#define Disable_INT1()            EIMSK &= ~(1<<INT1)

#define Ena_INT_Hcoder()      do {EIFR = (1<<INTF2); EIMSK |= (1<<INT2);} while(0)
#define Disa_INT_Hcoder()         EIMSK &= ~(1<<INT2)

#define Enable_INT_OCR2A()    do {TCNT2 = 0; TIFR2 = (1<<OCF2A); TIMSK2 = (1<<OCIE2A);} while(0)
#define Disable_INT_OCR2A()       TIMSK2 &= ~(1<<OCIE2A)

#define Enable_INT_OCR2B()    do {TCNT2 = 0; TIFR2 = (1<<OCF2B); TIMSK2 = (1<<OCIE2B);} while(0)
#define Disable_INT_OCR2B()       TIMSK2 &= ~(1<<OCIE2B)

#define Enable_INT_OCR3A()    do {TCNT3 = 0; TIFR3 = (1<<OCF3A); TIMSK3 = (1<<OCIE3A);} while(0)
#define Disable_INT_OCR3A()       TIMSK3 &= ~(1<<OCIE3A)

#define Enable_INT_OCR3B()    do {TCNT3 = 0; TIFR3 = (1<<OCF3B); TIMSK3 = (1<<OCIE3B);} while(0)
#define Disable_INT_OCR3B()       TIMSK3 &= ~(1<<OCIE3B)

#define Enable_INT_OCR4A()    do {TCNT4 = 0; TIFR4 = (1<<OCF4A); TIMSK4 = (1<<OCIE4A);} while(0)
#define Disable_INT_OCR4A()       TIMSK4 &= ~(1<<OCIE4A)

#define Enable_INT_OCR4B()    do {TCNT4 = 0; TIFR4 = (1<<OCF4B); TIMSK4 = (1<<OCIE4B);} while(0)
#define Disable_INT_OCR4B()       TIMSK4 &= ~(1<<OCIE4B)

#define Enable_INT_OCR5A()    do {TCNT5 = 0; TIFR5 = (1<<OCF5A); TIMSK5 = (1<<OCIE5A);} while(0)
#define Disable_INT_OCR5A()       TIMSK5 &= ~(1<<OCIE5A)

#define Enable_INT_OCR5B()    do {TCNT5 = 0; TIFR5 = (1<<OCF5B); TIMSK5 = (1<<OCIE5B);} while(0)
#define Disable_INT_OCR5B()       TIMSK5 &= ~(1<<OCIE5B)

////////////////////////////////////////////////////////////
#define Ena_INT_Thrd()        do {Disable_INT_OCR2A();\
    Disable_INT_OCR2B();\
    Disable_INT_OCR4A();\
    Disable_INT_OCR4B();\
    Disable_INT_OCR5A();\
    Disable_INT_OCR5B();\
    Enable_INT0();} while(0)

#define Ena_INT_Z_Feed()      do {Disable_INT0();\
    Disable_INT_OCR2A();\
    Disable_INT_OCR2B();\
    Disable_INT_OCR4A();\
    Disable_INT_OCR4B();\
    Disable_INT_OCR5B();\
    Enable_INT_OCR5A();} while(0)

#define Ena_INT_Z_aFeed()     do {Disable_INT0();\
    Disable_INT_OCR2A();\
    Disable_INT_OCR2B();\
    Disable_INT_OCR4B();\
    Disable_INT_OCR5A();\
    Disable_INT_OCR5B();\
    Enable_INT_OCR4A();} while(0)

#define Ena_INT_X_Feed()      do {Disable_INT0();\
    Disable_INT_OCR2A();\
    Disable_INT_OCR2B();\
    Disable_INT_OCR4A();\
    Disable_INT_OCR4B();\
    Disable_INT_OCR5A();\
    Enable_INT_OCR5B();} while(0)

#define Ena_INT_X_aFeed()     do {Disable_INT0();\
    Disable_INT_OCR2A();\
    Disable_INT_OCR2B();\
    Disable_INT_OCR4A();\
    Disable_INT_OCR5A();\
    Disable_INT_OCR5B();\
    Enable_INT_OCR4B();} while(0)

#define Ena_INT_Z_Rapid()     do {Disable_INT0();\
    Disable_INT_OCR2B();\
    Disable_INT_OCR4A();\
    Disable_INT_OCR4B();\
    Disable_INT_OCR5A();\
    Disable_INT_OCR5B();\
    Enable_INT_OCR2A();} while(0)

#define Ena_INT_X_Rapid()     do {Disable_INT0();\
    Disable_INT_OCR2A();\
    Disable_INT_OCR4A();\
    Disable_INT_OCR4B();\
    Disable_INT_OCR5A();\
    Disable_INT_OCR5B();\
    Enable_INT_OCR2B();} while(0)


// ***** My Flags *****

typedef struct
{
  uint8_t bit0 : 1;
  uint8_t bit1 : 1;
  uint8_t bit2 : 1;
  uint8_t bit3 : 1;
  uint8_t bit4 : 1;
  uint8_t bit5 : 1;
  uint8_t bit6 : 1;
  uint8_t bit7 : 1;
} FLAG0;
#define Spindle_Dir        ((volatile FLAG0*)_SFR_MEM_ADDR(GPIOR0))->bit0    // CW-0, CCW-1
#define Motor_Z_Dir        ((volatile FLAG0*)_SFR_MEM_ADDR(GPIOR0))->bit1    // CW-0, CCW-1
#define Joy_Z_flag         ((volatile FLAG0*)_SFR_MEM_ADDR(GPIOR0))->bit2    // On-1, Off-0
#define Step_Z_flag        ((volatile FLAG0*)_SFR_MEM_ADDR(GPIOR0))->bit3    // On-1, Off-0
#define Motor_X_Dir        ((volatile FLAG0*)_SFR_MEM_ADDR(GPIOR0))->bit4    // CW-0, CCW-1
#define Joy_X_flag         ((volatile FLAG0*)_SFR_MEM_ADDR(GPIOR0))->bit5    // On-1, Off-0
#define Step_X_flag        ((volatile FLAG0*)_SFR_MEM_ADDR(GPIOR0))->bit6    // On-1, Off-0
#define Cone_flag          ((volatile FLAG0*)_SFR_MEM_ADDR(GPIOR0))->bit7    // On-1, Off-0

//bool spindle_flag = OFF;
bool feed_Z_flag = OFF;
bool feed_X_flag = OFF;

bool rapid_step_Z_flag = OFF;
bool rapid_step_X_flag = OFF;
bool rapid_Z_flag = OFF;
bool rapid_X_flag = OFF;

bool limit_Left_flag = OFF;
bool limit_Right_flag = OFF;
bool limit_Front_flag = OFF;
bool limit_Rear_flag = OFF;

bool limit_button_flag = OFF;
bool button_flag = OFF;

bool a_flag = false;
bool b_flag = false;
bool c_flag = false;
bool d_flag = false;
bool cycle_flag = false;

bool err_0_flag = true;  // start without Joy_NoPressed is ERROR !
bool err_1_flag = false;
bool err_2_flag = false;
bool err_3_flag = false;

bool hand_X = OFF;
bool hand_Z = OFF;
bool flag_hand_X = OFF;
bool flag_hand_Z = OFF;
bool X_flag = OFF;                    // временный
bool Z_flag = OFF;                    // временный
bool flag_Scale_x1 = OFF;             // возможно только для отладки
bool flag_Scale_x10 = OFF;            // возможно только для отладки
//bool control_flag = OFF;
bool flag_j = OFF;
bool flag_cone = OFF;

// ***** MY VARIABLES *****
//int Tacho_Count = 0;
//int Tacho_Count_Old = 0;
//int Spindle_Count = 0;

volatile int16_t Enc_Pos = 0;

volatile long Hand_Count = 0;
long Hand_Count_Old = 0;
long Hand_Count_New = 0;
long Hand_Z_Pos = 0;
long Hand_X_Pos = 0;

byte Scale = HC_SCALE_1;

byte Ks_Count = 0;
int Km_Count = 0;
byte Ks_Divisor = 0;
byte tmp_Ks_Divisor = THRD_ACCEL;
int Km_Divisor = 0;
uint16_t Feed_Divisor = 0;
uint16_t aFeed_Divisor = 0;

byte Cs_Count = 0;
int Cm_Count = 0;
byte Cs_Divisor = 0;
int Cm_Divisor = 0;

byte tmp_Accel = THRD_ACCEL;
byte Repeat_Count = 0;

int Brake_Compens = 0;

byte Mode = Mode_Feed;
byte Sub_Mode_Thread = Sub_Mode_Thread_Man;
byte Sub_Mode_Feed = Sub_Mode_Feed_Man;
byte Sub_Mode_aFeed = Sub_Mode_aFeed_Man;
byte Sub_Mode_Cone = Sub_Mode_Cone_Man;
byte Sub_Mode_Sphere = Sub_Mode_Sphere_Man;
byte Thread_Step = 11;
byte Cone_Step = 0;

long Motor_Z_Pos = 0;
long Motor_X_Pos = 0;

long Limit_Pos_Left = Limit_Pos_Max;
long Limit_Pos_Right = Limit_Pos_Min;
long Limit_Pos_Front = Limit_Pos_Max;
long Limit_Pos_Rear = Limit_Pos_Min;

long tmp_Limit_Pos_Left  = Limit_Pos_Max;  // При аварии,
long tmp_Limit_Pos_Right = Limit_Pos_Min;  // или при возврате джойстика в нейтраль
long tmp_Limit_Pos_Front = Limit_Pos_Max;  // до завершения операции
long tmp_Limit_Pos_Rear  = Limit_Pos_Min;  // позиции лимитов сохраняться.

volatile long Limit_Pos = 0;
volatile long Limit_Pos_HC = 0;

uint16_t Feed_mm = 0;
uint16_t aFeed_mm = 0;

uint16_t Start_Speed = ENC_LINE_PER_REV / ((uint32_t)MOTOR_Z_STEP_PER_REV * McSTEP_Z * MIN_FEED / SCREW_Z) / FEED_ACCEL;
uint16_t max_OCR5A = ENC_LINE_PER_REV / ((uint32_t)MOTOR_Z_STEP_PER_REV * McSTEP_Z * MIN_FEED / SCREW_Z) / FEED_ACCEL;                                                                                    // Начальная скорость подачи при разгоне/торможении
uint16_t max_OCR4A = (250000 / ((uint32_t)MIN_aFEED * MOTOR_Z_STEP_PER_REV * McSTEP_Z / ((uint32_t)60 * SCREW_Z / 100) * 2) - 1) / FEED_ACCEL;

byte Total_Tooth = 1;
byte Current_Tooth = 1;

byte Pass_Total = 1;
byte Pass_Nr = 1;
long Null_X_Pos = 0;
long Null_Z_Pos = 0;
int Ap = 0;

int ADC_Feed = 0;
long Sum_ADC = 0;
int ADC_Array[16];
byte x = 0;

long Control_Count = 0;


// Sphere
const int Cutter_Width_array[] = {100, 150, 200, 250, 300};
#define TOTAL_CUTTER_WIDTH (sizeof(Cutter_Width_array) / sizeof(Cutter_Width_array[0]))
byte Cutter_Step = 2;
int Cutter_Width = Cutter_Width_array[Cutter_Step];

const int Cutting_Width_array[] = {10, 25, 50, 100};
#define TOTAL_CUTTING_STEP (sizeof(Cutting_Width_array) / sizeof(Cutting_Width_array[0]))
byte Cutting_Step = 1;
int Cutting_Width = Cutting_Width_array[Cutting_Step];

long Sph_R_mm = 1000;
long Sph_R = 0;
long R_Quad = Sph_R_mm * Sph_R_mm;
long Bar_R_mm = 0;
long Bar_R = 0;

#define KEYB_TIMER_FLAG       (TIFR1 & (1<<OCF1A))
#define CLEAR_KEYB_TIMER   do {TCNT1 = 0; (TIFR1 |= (1<<OCF1A));} while(0)

uint16_t max_OCR3A = HC_START_SPEED_1;
uint16_t min_OCR3A = HC_MAX_SPEED_1;

byte Blinc_Count = 0;
byte LCD_Page = 1;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  err_0_flag = true; // считаем, что ДЖОЙ не в нейтрали ...

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("   ELS v.7e2+TSS   ");
  _delay_ms(1000);

  //DDRG = B11111111;
  TIMSK0 = 0;

  Encoder_Init();
  Hand_Init();
  Motor_Init();
  Flags_Init();

  INT0_Init();
  INT2_Init();

  Timer2_Init();
  OCR2A = MIN_RAPID_MOTION;

  Timer3_Init();
  OCR3A = max_OCR3A;

  Timer4_Init();
  OCR4A = max_OCR4A;

  Timer5_Init();
  OCR5A = max_OCR5A;

  Ena_INT_Z_Feed();
  
  Limit_Init();
  Limit_Left_LED_Off();
  Limit_Right_LED_Off();
  Limit_Front_LED_Off();
  Limit_Rear_LED_Off();

  Menu_Buttons_Init();
  Joy_Init();
  Mode_Switch_Init();

  Beeper_Init();
  Beeper_Off();

  Spindle_Dir = CW;
  Motor_Z_Dir = CW;
  Joy_Z_flag  = OFF;
  Step_Z_flag = OFF;
  Motor_X_Dir = CW;
  Joy_X_flag  = OFF;
  Step_X_flag = OFF;
  Cone_flag   = OFF;

  TCCR1A = 0;
  TCCR1B = 0
         | (0 << ICNC1) | (0 << ICES1)
         | (0 << WGM13) | (1 << WGM12)
         | (1 << CS12)  | (0 << CS11) | (1 << CS10);
  OCR1A = 625;

  Menu();

  Motor_Z_RemovePulse();
  Motor_X_RemovePulse();

  // понты ;) (визуальные эффекты на кнопках лимитов)
  Limit_Led_Dance();

  if(err_0_flag){ Beep_Error(); }

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  #ifdef DEBUG_TIME_ON_LOOP
    DIAG_HIGH(); // "подняли" ногу
    //_delay_us(2);
  #endif
  //Spindle();
  
  Read_ADC_Feed();
  
  if (KEYB_TIMER_FLAG != 0) 
  {
    Menu();
    Blinc_Limit_Buttons();  // Blinc Limit Buttons LED if needed
  }


  #ifdef DEBUG_TIME_ON_LOOP
    //DIAG_LOW();
    //_delay_us(4);
    //DIAG_HIGH(); // "подняли" ногу
  #endif
  //if (Mode == Mode_Divider) Print(); // пока для теста !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  Print();                             // только для теста !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  #ifdef DEBUG_TIME_ON_LOOP
    //DIAG_HIGH(); // "подняли" ногу
    //_delay_us(20);
    DIAG_LOW();
  #endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *****  Thread ***** ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR(INT0_vect)
{
  #ifdef DEBUG_TIME_ON_INT0
    DIAG_HIGH(); // вошли в прерывание - "подняли" ногу
  #endif

  if (!Enc_Ch_A) {
    if (!Enc_Ch_B) {
      Spindle_Dir = CW;
      if (++Enc_Pos == ENC_TICK) {
        Enc_Pos = 0;
        if (Joy_Z_flag == ON)       { Step_Z_flag = ON; }
        else if (Joy_X_flag == ON)  { Step_X_flag = ON; }
      }
    } else {
      Spindle_Dir = CCW;
      if (--Enc_Pos < 0) {
        Enc_Pos = ENC_TICK - 1;
        if (Joy_Z_flag == ON)       { Step_Z_flag = ON; }
        else if (Joy_X_flag == ON)  { Step_X_flag = ON; }
      }
    }
  } else {
    if (!Enc_Ch_B) {
      Spindle_Dir = CCW;
      if (--Enc_Pos < 0) {
        Enc_Pos = ENC_TICK - 1;
        if (Joy_Z_flag == ON)       { Step_Z_flag = ON; }
        else if (Joy_X_flag == ON)  { Step_X_flag = ON; }
      }
    } else {
      Spindle_Dir = CW;
      if (++Enc_Pos == ENC_TICK) {
        Enc_Pos = 0;
        if (Joy_Z_flag == ON)       { Step_Z_flag = ON; }
        else if (Joy_X_flag == ON)  { Step_X_flag = ON; }
      }
    }
  }

  if (Step_Z_flag == ON)
  { 
    Motor_Z_RemovePulse();
    if ((Motor_Z_Dir == CW && Motor_Z_Pos > Limit_Pos) 
     || (Motor_Z_Dir == CCW && Motor_Z_Pos < Limit_Pos) 
     || (!Joy_Z_flag)) {
      if (tmp_Ks_Divisor < tmp_Accel) {
        Ks_Count++;
        if (Ks_Count > tmp_Ks_Divisor) {
          Motor_Z_SetPulse();
          if (Motor_Z_Dir == CW) { Motor_Z_Pos++; }
          else                   { Motor_Z_Pos--; }
          Ks_Count = 0;
          if (++Repeat_Count == REPEAt) {
            Repeat_Count = 0;
            tmp_Ks_Divisor++;
          }
        }
      } else  {
        Step_Z_flag = OFF;
      }
    }
    else
    {
      Ks_Count++;
      if (Ks_Count > tmp_Ks_Divisor)
      {
        Motor_Z_SetPulse();
        if (Motor_Z_Dir == CW) { Motor_Z_Pos++; }
        else                   { Motor_Z_Pos--; }

        if (tmp_Ks_Divisor > Ks_Divisor)
        {
          Ks_Count = 0;
          if (++Repeat_Count == REPEAt) {
            Repeat_Count = 0;
            tmp_Ks_Divisor--;
          }
        } else {
          Km_Count += Km_Divisor;
          if (Km_Count > Km_Divisor) {
            Km_Count -= 10000;
            Ks_Count = 0;
          } else {
            Ks_Count = 1;
          }
        }
      }
    }
  }

  if (Step_X_flag == ON) { 
    Motor_X_RemovePulse();
    if ((Motor_X_Dir == CW && Motor_X_Pos > Limit_Pos) 
     || (Motor_X_Dir == CCW && Motor_X_Pos < Limit_Pos) 
     || (!Joy_X_flag)) {
      if (tmp_Ks_Divisor < tmp_Accel) {
        Ks_Count++;
        if (Ks_Count > tmp_Ks_Divisor) {
          Motor_X_SetPulse();
          if (Motor_X_Dir == CW) { Motor_X_Pos++; }
          else                   { Motor_X_Pos--; }
          Ks_Count = 0;
          if (++Repeat_Count == REPEAt) {
            Repeat_Count = 0;
            tmp_Ks_Divisor++;
          }
        }
      } else  {
        Step_X_flag = OFF;
      }
    } else {
      Ks_Count++;
      if (Ks_Count > tmp_Ks_Divisor) {
        Motor_X_SetPulse();
        if (Motor_X_Dir == CW) { Motor_X_Pos++; }
        else                   { Motor_X_Pos--; }

        if (tmp_Ks_Divisor > Ks_Divisor) {
          Ks_Count = 0;
          if (++Repeat_Count == REPEAt) {
            Repeat_Count = 0;
            tmp_Ks_Divisor--;
          }
        } else {
          Km_Count += Km_Divisor;
          if (Km_Count > Km_Divisor) {
            Km_Count -= 10000;
            Ks_Count = 0;
          } else {
            Ks_Count = 1;
          }
        }
      }
    }
  }

  #ifdef DEBUG_TIME_ON_INT0
    DIAG_LOW(); // вошли в прерывание - "подняли" ногу
  #endif
}

//ISR(INT1_vect)
//{
  //DIAG_HIGH();
  //
  //DIAG_LOW();
//}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *****  Feed & Cone ***** //////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR (TIMER5_COMPA_vect)
{
  #ifdef DEBUG_TIME_ON_TIMER5_COMPA
    DIAG_HIGH(); // вошли в прерывание - "подняли" ногу
  #endif

  if (Joy_Z_flag == ON) {
    Motor_X_RemovePulse();
  }
  //Tacho_Count = Tacho_Count + (OCR5A + 1);
  //if (Tacho_Count > ENC_LINE_PER_REV)
  //{
  //  Tacho_Count = Tacho_Count - ENC_LINE_PER_REV;
  //}

  if ( (Motor_Z_Dir == CW && Motor_Z_Pos > Limit_Pos) 
    || (Motor_Z_Dir == CCW && Motor_Z_Pos < Limit_Pos) 
    || (!feed_Z_flag) )
  {
    if (OCR5A < max_OCR5A)
    {
      Motor_Z_InvertPulse();
      if (!Read_Z_State)
      {
        OCR5A++;
        if (Motor_Z_Dir == CW) {
          Motor_Z_Pos ++;
        }
        else {
          Motor_Z_Pos --;
        }
      }
    }
    else
    {
      Step_Z_flag = OFF;
      Step_X_flag = OFF;
    }
  }

  else
  {
    Step_Z_flag = ON;
    Motor_Z_InvertPulse();
    if (!Read_Z_State)
    {
      if (Motor_Z_Dir == CW) {
        Motor_Z_Pos ++;
      }
      else {
        Motor_Z_Pos --;
      }

      if (OCR5A > Feed_Divisor) {
        OCR5A--;
      }
      else if (OCR5A < Feed_Divisor) {
        OCR5A ++;
      }
    }
  }

  if (Step_X_flag == ON)
  {
    if (++Cs_Count > Cs_Divisor)
    {
      Motor_X_SetPulse();

      if (Motor_X_Dir == CW) {
        Motor_X_Pos ++;
      }
      else {
        Motor_X_Pos --;
      }

      Cm_Count = Cm_Count + Cm_Divisor;
      if (Cm_Count > Cm_Divisor)
      {
        Cm_Count = Cm_Count - 10000;
        Cs_Count = 0;
      }
      else {
        Cs_Count = 1;
      }
    }
  }
  #ifdef DEBUG_TIME_ON_TIMER5_COMPA
    DIAG_LOW(); // вышли из прерывания - "опустили" ногу
  #endif
}

////////////////////////////////////////////////////////////
ISR (TIMER5_COMPB_vect)
{
  //Tacho_Count = Tacho_Count + (OCR5A + 1);
  //if (Tacho_Count > ENC_LINE_PER_REV) {
  //  Tacho_Count = Tacho_Count - ENC_LINE_PER_REV;
  //}

  if ( (Motor_X_Dir == CW && Motor_X_Pos > Limit_Pos) 
    || (Motor_X_Dir == CCW && Motor_X_Pos < Limit_Pos) 
    || (!feed_X_flag) ) {
    if (OCR5A < max_OCR5A) {
      Motor_X_InvertPulse();
      if (!Read_X_State) {
        OCR5A++;
        if (Motor_X_Dir == CW)  { Motor_X_Pos ++; }
        else                    { Motor_X_Pos --; }
      }
    } else {
      Step_X_flag = OFF;
    }
  }

  else {
    Step_X_flag = ON;
    Motor_X_InvertPulse();
    {
      if (!Read_X_State) {
        if (Motor_X_Dir == CW)  { Motor_X_Pos ++;}
        else                    { Motor_X_Pos --; }

        if (OCR5A > Feed_Divisor) {
          OCR5A--;
        }
        else if (OCR5A < Feed_Divisor) {
          OCR5A ++;
        }
      }
    }
  }

  /////////////////////////////////////////////////////////
  // Режим Сфера
  /////////////////////////////////////////////////////////
  //if (Mode == Mode_Sphere)                               
  //{
    //
  //}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ***** Rapid Feed & Rapid Cone ***** ///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR (TIMER2_COMPA_vect) {
  Motor_X_RemovePulse();
  if ( (Motor_Z_Dir == CW && Motor_Z_Pos > Limit_Pos) 
    || (Motor_Z_Dir == CCW && Motor_Z_Pos < Limit_Pos) 
    || (!rapid_Z_flag) ) {
    if (OCR2A < MIN_RAPID_MOTION) {
      Motor_Z_InvertPulse();
      if (!Read_Z_State) {
        if (Motor_Z_Dir == CW)  { Motor_Z_Pos ++; }
        else                    { Motor_Z_Pos --; }

        if (++Repeat_Count == REPEAt) {
          Repeat_Count = 0;
          OCR2A ++;
        }
      }
    } else {
      rapid_step_Z_flag = OFF;
      Step_X_flag = OFF;
    }
  }  else  {
    rapid_step_Z_flag = ON;
    Motor_Z_InvertPulse();
    if (!Read_Z_State)    {
      if (Motor_Z_Dir == CW) { Motor_Z_Pos ++; }
      else                   { Motor_Z_Pos --; }

      if (OCR2A > MAX_RAPID_MOTION)      {
        if (++Repeat_Count == REPEAt) {
          Repeat_Count = 0;
          OCR2A --;
        }
      }
    }
  }

  ///////////////////////////////////////////////////////
  if (Step_X_flag == ON)
  {
    if (++Cs_Count > Cs_Divisor)
    {
      Motor_X_SetPulse();
      if (Motor_X_Dir == CW) {
        Motor_X_Pos ++;
      }
      else {
        Motor_X_Pos --;
      }

      Cm_Count = Cm_Count + Cm_Divisor;
      if (Cm_Count > Cm_Divisor)
      {
        Cm_Count = Cm_Count - 10000;
        Cs_Count = 0;
      }
      else {
        Cs_Count = 1;
      }
    }
  }
}

//////////////////////////////////////////////////////////
ISR (TIMER2_COMPB_vect) {
  if ( (Motor_X_Dir == CW && Motor_X_Pos > Limit_Pos) 
    || (Motor_X_Dir == CCW && Motor_X_Pos < Limit_Pos) 
    || (!rapid_X_flag) ) {
    if (OCR2A < MIN_RAPID_MOTION) {
      Motor_X_InvertPulse();
      if (!Read_X_State) {
        if (Motor_X_Dir == CW)  { Motor_X_Pos ++; }
        else                    { Motor_X_Pos --; }

        if (++Repeat_Count == REPEAt) {
          Repeat_Count = 0;
          OCR2A ++;
        }
      }
    } else {
      rapid_step_X_flag = OFF;
    }
  } else {
    rapid_step_X_flag = ON;
    Motor_X_InvertPulse();
    if (!Read_X_State) {
      if (Motor_X_Dir == CW)  { Motor_X_Pos ++; }
      else                    { Motor_X_Pos --; }

      if (OCR2A > MAX_RAPID_MOTION) {
        if (++Repeat_Count == REPEAt) {
          Repeat_Count = 0;
          OCR2A --;
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ***** Asynchron Feed ***** ////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR (TIMER4_COMPA_vect) {
  if ( (Motor_Z_Dir == CW && Motor_Z_Pos > Limit_Pos) 
    || (Motor_Z_Dir == CCW && Motor_Z_Pos < Limit_Pos) 
    || (!feed_Z_flag) )  {
    if (OCR4A < max_OCR4A) {
      Motor_Z_InvertPulse();
      if (!Read_Z_State) {
        OCR4A ++;
        if (Motor_Z_Dir == CW)  { Motor_Z_Pos ++; }  
        else                    { Motor_Z_Pos --; }
      }
    } else {
      Step_Z_flag = OFF;
      Step_X_flag = OFF;
    }
  }  else  {
    Step_Z_flag = ON;
    Motor_Z_InvertPulse();
    if (!Read_Z_State)    {
      if (Motor_Z_Dir == CW)  { Motor_Z_Pos ++; }
      else                    { Motor_Z_Pos --; }

      if (OCR4A > aFeed_Divisor) {
        OCR4A --;
      } else if (OCR4A < aFeed_Divisor) {
        OCR4A ++;
      }
    }
  }
}

//////////////////////////////////////////////////////////
ISR (TIMER4_COMPB_vect)
{
  if ( (Motor_X_Dir == CW && Motor_X_Pos > Limit_Pos) || (Motor_X_Dir == CCW && Motor_X_Pos < Limit_Pos) || (!feed_X_flag) )
  {
    if (OCR4A < max_OCR4A)
    {
      Motor_X_InvertPulse();
      if (!Read_X_State)
      {
        OCR4A ++;
        if (Motor_X_Dir == CW) {
          Motor_X_Pos ++;
        }
        else                   {
          Motor_X_Pos --;
        }
      }
    }    else    {
      Step_Z_flag = OFF;//Step_Z_flag = OFF;
      Step_X_flag = OFF;//Step_X_flag = OFF;
    }
  }  else  {
    Step_X_flag = ON;//Step_X_flag = ON;
    Motor_X_InvertPulse();
    if (!Read_X_State)
    {
      if (Motor_X_Dir == CW) {
        Motor_X_Pos ++;
      }
      else                   {
        Motor_X_Pos --;
      }

      if      (OCR4A > aFeed_Divisor) {
        OCR4A --;
      }
      else if (OCR4A < aFeed_Divisor) {
        OCR4A ++;
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ***** HandCoder ***** /////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR(INT2_vect)
{
  if (!Hand_Ch_A)
  {
    if (!Hand_Ch_B) {
      Hand_Count --;
    }
  }

  else
  {
    if (!Hand_Ch_B) {
      Hand_Count ++;
    }
  }

}

/////////////////////////////////////////////
ISR (TIMER3_COMPA_vect)
{
  if (Motor_Z_Dir == CW)
  {
    if (Motor_Z_Pos < Null_Z_Pos + Hand_Z_Pos)
    {
      Motor_Z_InvertPulse();
      if (!Read_Z_State)
      {
        Motor_Z_Pos ++;
        if ((Motor_Z_Pos > Limit_Pos_HC) || (hand_Z == OFF))
        {
          if (OCR3A < max_OCR3A) OCR3A ++;
        }
        else if (Motor_Z_Pos < Limit_Pos_HC)
        {
          if (OCR3A > min_OCR3A) OCR3A --;
        }
      }
    }
    else if (Motor_Z_Pos == Hand_Z_Pos)
    {
      //
    }
  }
  else// if (Motor_Z_Dir == CCW)
  {
    if (Motor_Z_Pos > Null_Z_Pos + Hand_Z_Pos)
    {
      Motor_Z_InvertPulse();
      if (!Read_Z_State)
      {
        Motor_Z_Pos --;
        if (Motor_Z_Pos < Limit_Pos_HC  || hand_Z == OFF)
        {
          if (OCR3A < max_OCR3A) OCR3A ++;
        }
        else if (Motor_Z_Pos > Limit_Pos_HC)
        {
          if (OCR3A > min_OCR3A) OCR3A --;
        }
      }
    }
    else if (Motor_Z_Pos == Hand_Z_Pos)
    {
      //
    }
  }
}

//////////////////////////////////////////////////////////
ISR (TIMER3_COMPB_vect)
{
  if (Motor_X_Dir == CW)
  {
    if (Motor_X_Pos < Null_X_Pos + Hand_X_Pos)
    {
      Motor_X_InvertPulse();
      if (!Read_X_State)
      {
        Motor_X_Pos ++;
        if ((Motor_X_Pos > Limit_Pos_HC) || (hand_X == OFF))
        {
          if (OCR3A < max_OCR3A) OCR3A ++;
        }
        else if (Motor_X_Pos < Limit_Pos_HC)
        {
          if (OCR3A > min_OCR3A) OCR3A --;
        }
      }
    }
    else if (Motor_X_Pos == Hand_X_Pos)
    {
      //
    }
  }

  else if (Motor_X_Dir == CCW)
  {
    if (Motor_X_Pos > Null_X_Pos + Hand_X_Pos)
    {
      Motor_X_InvertPulse();
      if (!Read_X_State)
      {
        Motor_X_Pos --;
        if ((Motor_X_Pos < Limit_Pos_HC) || (hand_X == OFF))
        {
          if (OCR3A < max_OCR3A) OCR3A ++;
        }
        else if (Motor_X_Pos > Limit_Pos_HC)
        {
          if (OCR3A > min_OCR3A) OCR3A --;
        }
      }
    }
    else if (Motor_X_Pos == Hand_X_Pos)
    {
      //
    }
  }
}


// ***** End ***** ///////////////////////////////////////////////////
