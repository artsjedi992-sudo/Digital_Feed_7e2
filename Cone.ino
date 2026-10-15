/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********** Режим "Конус" ********** //////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Cone_Left(bool & a_flag, bool & b_flag)
{  
   if (b_flag == true) return;
   a_flag = false;
   b_flag = true;
   
   Cs_Divisor = Cone_Info[Cone_Step].Cs_Div;
   Cm_Divisor = Cone_Info[Cone_Step].Cm_Div;

   Start_Speed = ENC_LINE_PER_REV / ((uint32_t)MOTOR_Z_STEP_PER_REV * McSTEP_Z * MIN_FEED / SCREW_Z) / FEED_ACCEL /2;
   if (Motor_Z_Pos < Limit_Pos_Left - Start_Speed * 2)
   {
      Feed_Divisor = ENC_LINE_PER_REV / ((uint32_t)MOTOR_Z_STEP_PER_REV * McSTEP_Z * Feed_mm / SCREW_Z) /2;
      if (Feed_Divisor < Start_Speed)
      {
         max_OCR5A = Start_Speed;
         if (Step_Z_flag == OFF)
         {
            OCR5A = Start_Speed;
         }
      }
      else
      {
         max_OCR5A = Feed_Divisor;
         OCR5A = Feed_Divisor;
      }
   }
   else
   {
      Feed_Divisor = Start_Speed;
      max_OCR5A = Start_Speed;
      OCR5A = Start_Speed;
   }
   
   Brake_Compens = max_OCR5A - Feed_Divisor + 1;
   Limit_Pos = Limit_Pos_Left - Brake_Compens;

   if (Read_X_Ena_State == false) Motor_X_Enable();
   if      (Mode == Mode_Cone_L) {Motor_X_Dir = CW; Motor_X_CW();}
   else if (Mode == Mode_Cone_R) {Motor_X_Dir = CCW; Motor_X_CCW();}
   
   if (Read_Z_Ena_State == false) Motor_Z_Enable();
   Motor_Z_Dir = CW;//Motor_Z_Dir = CW;
   Motor_Z_CW();

   Step_X_flag = ON;//Step_X_flag = ON;
   feed_Z_flag = ON;
   Joy_Z_flag = ON;//Joy_Z_flag = ON;
   Ena_INT_Z_Feed();
}

void Cone_Right(bool & a_flag, bool & b_flag)
{
   if (b_flag == true) return;
   a_flag = false;
   b_flag = true; 
   
   Cs_Divisor = Cone_Info[Cone_Step].Cs_Div;
   Cm_Divisor = Cone_Info[Cone_Step].Cm_Div;

   Start_Speed = ENC_LINE_PER_REV / ((uint32_t)MOTOR_Z_STEP_PER_REV * McSTEP_Z * MIN_FEED / SCREW_Z) / FEED_ACCEL /2;
   if (Motor_Z_Pos > Limit_Pos_Right + Start_Speed * 2)
   {
      Feed_Divisor = ENC_LINE_PER_REV / ((uint32_t)MOTOR_Z_STEP_PER_REV * McSTEP_Z * Feed_mm / SCREW_Z) /2;
      if (Feed_Divisor < Start_Speed)
      {
         max_OCR5A = Start_Speed;
         if (Step_Z_flag == OFF)
         {
            OCR5A = Start_Speed;
         }
      }
      else
      {
         max_OCR5A = Feed_Divisor;
         OCR5A = Feed_Divisor;
      }
   }
   else
   {
      Feed_Divisor = Start_Speed;
      max_OCR5A = Start_Speed;
      OCR5A = Start_Speed;
   }
   
   Brake_Compens = max_OCR5A - Feed_Divisor + 1;
   Limit_Pos = Limit_Pos_Right + Brake_Compens;

   if (Read_X_Ena_State == false) Motor_X_Enable();
   if      (Mode == Mode_Cone_L) {Motor_X_Dir = CCW; Motor_X_CCW();}
   else if (Mode == Mode_Cone_R) {Motor_X_Dir = CW; Motor_X_CW();}

   if (Read_Z_Ena_State == false) Motor_Z_Enable();
   Motor_Z_Dir = CCW;//Motor_Z_Dir = CCW;
   Motor_Z_CCW();
   
   Step_X_flag = ON;//Step_X_flag = ON;
   feed_Z_flag = ON;
   Joy_Z_flag = ON;//Joy_Z_flag = ON;
   Ena_INT_Z_Feed();
}

void Cone_Front(bool & a_flag, bool & b_flag)
{
   if (b_flag == true) return;
   a_flag = false;
   b_flag = true;
}

void Cone_Rear(bool & a_flag, bool & b_flag)
{
   if (b_flag == true) return;
   a_flag = false;
   b_flag = true; 
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********** Режим "Конус! Быстрая Подача" ********** ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Rapid_Cone_Left(bool & a_flag, bool & b_flag)
{
   if (a_flag == true) return;
   b_flag = false;
   a_flag = true;
   
   Brake_Compens = (MIN_RAPID_MOTION - MAX_RAPID_MOTION) * REPEAt + 1;
   Limit_Pos = Limit_Pos_Left - Brake_Compens;
   
   Cs_Count = 0;
   Cm_Count = 0;            

   if (Read_X_Ena_State == false) Motor_X_Enable();
   if      (Mode == Mode_Cone_L) {Motor_X_Dir = CW; Motor_X_CW();}
   else if (Mode == Mode_Cone_R) {Motor_X_Dir = CCW; Motor_X_CCW();}
   
   if (Read_Z_Ena_State == false) Motor_Z_Enable();
   Motor_Z_Dir = CW;//Motor_Z_Dir = CW;
   Motor_Z_CW();

   Step_X_flag = ON;//Step_X_flag = ON;
   rapid_Z_flag = ON;
   Joy_Z_flag = ON;//Joy_Z_flag = ON;
   Ena_INT_Z_Rapid();
}

void Rapid_Cone_Right(bool & a_flag, bool & b_flag)
{
   if (a_flag == true) return;
   b_flag = false;
   a_flag = true;
   
   Brake_Compens = (MIN_RAPID_MOTION - MAX_RAPID_MOTION) * REPEAt + 1;
   Limit_Pos = Limit_Pos_Right + Brake_Compens;
   
   Cs_Count = 0;
   Cm_Count = 0;            

   if (Read_X_Ena_State == false) Motor_X_Enable();
   if      (Mode == Mode_Cone_L) {Motor_X_Dir = CCW; Motor_X_CCW();}
   else if (Mode == Mode_Cone_R) {Motor_X_Dir = CW; Motor_X_CW();}
   
   if (Read_Z_Ena_State == false) Motor_Z_Enable();
   Motor_Z_Dir = CCW;//Motor_Z_Dir = CCW;
   Motor_Z_CCW();

   Step_X_flag = ON;//Step_X_flag = ON;
   rapid_Z_flag = ON;
   Joy_Z_flag = ON;//Joy_Z_flag = ON;
   Ena_INT_Z_Rapid();  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********** Режим "Цикл Конус" ********** //////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Cone_Ext_Right()
{  
   if (flag_end_operation ==false && cycle_flag ==false && Motor_Z_Pos ==Limit_Pos_Right)
   {
      a_flag = false;
      b_flag = false;
      cycle_flag = true;

      if (flag_cone == ON)
      {
         Limit_Pos_Rear = (Null_X_Pos - REBOUND_X);
         limit_Rear_flag = ON;
         Limit_Rear_LED_On();
         BeepBeep();
      }
      else                                // на финальную точку
      {
         Limit_Pos_Rear = Null_X_Pos;
      //   limit_Rear_flag = OFF;
      //   Limit_Rear_LED_Off();
      //   limit_Front_flag = OFF;
      //   Limit_Front_LED_Off();
         pass_end_flag = true;
      }
      
      if (Motor_X_Pos > (Limit_Pos_Rear + ((MIN_RAPID_MOTION - MAX_RAPID_MOTION) * REPEAt) *2)) {Rapid_Feed_Rear(a_flag, b_flag);}
      else                                                                                      {Feed_Rear(a_flag, b_flag);}
   }


   else if (flag_cone == ON && Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Rear)
   {
      a_flag = false;
      b_flag = false;
         
      float Cone_Div = (float)Cs_Divisor + ((float)Cm_Divisor /10000);
      long Cath_X = Ap * Pass_Nr / ((float)SCREW_X / MOTOR_X_STEP_PER_REV / McSTEP_X *2);       // Катет_X в микрошагах
      long Cath_Z = (long)(Cath_X * Cone_Div +McSTEP_Z /2) & ~(McSTEP_Z -1);                    // Катет_Z в микрошагах

      if (Limit_Pos_Right + Cath_Z >= tmp_Limit_Pos_Left)
      {
         flag_cone = OFF;  ////////////////////////////////////////////////////////////////
         Limit_Pos_Left = tmp_Limit_Pos_Left;
      }
      else
      {
         Limit_Pos_Left = Limit_Pos_Right + Cath_Z;
      }
      
      Pass_Nr++;
      if (Motor_Z_Pos < (Limit_Pos_Left - ((MIN_RAPID_MOTION - MAX_RAPID_MOTION) * REPEAt) *2)) {Step_X_flag = OFF; Rapid_Feed_Left(a_flag, b_flag);}
      else                                                                                      {Step_X_flag = OFF; Feed_Left(a_flag, b_flag);}
   }

   else if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Rear)
   {
      a_flag = false;
      b_flag = false;
      
      Limit_Pos_Front = Null_X_Pos;
      limit_Front_flag = ON;
      Limit_Front_LED_On();

      Feed_Front(a_flag, b_flag); 
   }

   else if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Front)
   {
      a_flag = false;
      b_flag = false;
      cycle_flag = false;
      
      Cone_Right(a_flag, b_flag); 
   }
   if (flag_end_operation == false && pass_end_flag == true && Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Rear)   // Финиш 
   {
      Limit_Front_LED_Off();
      limit_Front_flag = OFF;
      Limit_Rear_LED_Off();
      limit_Rear_flag = OFF;
      Limit_Pos_Front = Limit_Pos_Max;
      Limit_Pos_Rear = Limit_Pos_Min;
      flag_end_operation = true;          // в меню Joy в нейтрали - false
      pass_end_flag = false;              // дублируем в меню Joy_Neutral, если переключили Joy до остановки и вызова текущего "if"
      //Print();
      BeepComplete();
      Motor_Z_RemovePulse();
      Motor_X_RemovePulse();
   }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Cone_Ext_Left()
{  
   if (flag_end_operation ==false && cycle_flag == false && Motor_Z_Pos == Limit_Pos_Left)
   {
      a_flag = false;
      b_flag = false;
      cycle_flag = true;

      if (flag_cone == ON)
      {
         Limit_Pos_Rear = (Null_X_Pos - REBOUND_X);
         limit_Rear_flag = ON;
         Limit_Rear_LED_On();
         BeepBeep();
      }
      else                                // на финальную точку
      {
         Limit_Pos_Rear = Null_X_Pos;
      //   limit_Rear_flag = OFF;
      //   Limit_Rear_LED_Off();
      //   limit_Front_flag = OFF;
      //   Limit_Front_LED_Off();
         pass_end_flag = true;
      }
      
      if (Motor_X_Pos > (Limit_Pos_Rear + ((MIN_RAPID_MOTION - MAX_RAPID_MOTION) * REPEAt) *2)) {Rapid_Feed_Rear(a_flag, b_flag);}
      else                                                                                      {Feed_Rear(a_flag, b_flag);}
   }

   else if (flag_cone == ON && Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Rear)
   {
      a_flag = false;
      b_flag = false;
         
      float Cone_Div = (float)Cs_Divisor + ((float)Cm_Divisor /10000);
      long Cath_X = Ap * Pass_Nr / ((float)SCREW_X / MOTOR_X_STEP_PER_REV / McSTEP_X *2);       // Катет_X в микрошагах
      long Cath_Z = (long)(Cath_X * Cone_Div +McSTEP_Z /2) & ~(McSTEP_Z -1);                    // Катет_Z в микрошагах

      if (Limit_Pos_Left - Cath_Z <= tmp_Limit_Pos_Right)
      {
         flag_cone = OFF;  ////////////////////////////////////////////////////////////////
         Limit_Pos_Right = tmp_Limit_Pos_Right;
      }
      else
      {
         Limit_Pos_Right = Limit_Pos_Left - Cath_Z;
      }
      
      Pass_Nr++;
      if (Motor_Z_Pos > (Limit_Pos_Right + ((MIN_RAPID_MOTION - MAX_RAPID_MOTION) * REPEAt) *2)) {Step_X_flag = OFF; Rapid_Feed_Right(a_flag, b_flag);}  // !!!!!!! + -
      else                                                                                       {Step_X_flag = OFF; Feed_Right(a_flag, b_flag);}
   }

   else if (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Rear)
   {
      a_flag = false;
      b_flag = false;
      
      Limit_Pos_Front = Null_X_Pos;
      limit_Front_flag = ON;
      Limit_Front_LED_On();

      Feed_Front(a_flag, b_flag);
   }

   else if (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Front)
   {
      a_flag = false;
      b_flag = false;
      cycle_flag = false;
      
      Cone_Left(a_flag, b_flag); 
   }

   if (flag_end_operation == false && pass_end_flag == true && Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Rear)   // Финиш 
   {
      Limit_Front_LED_Off();
      limit_Front_flag = OFF;
      Limit_Rear_LED_Off();
      limit_Rear_flag = OFF;
      Limit_Pos_Front = Limit_Pos_Max;
      Limit_Pos_Rear = Limit_Pos_Min;
      flag_end_operation = true;          // в меню Joy в нейтрали - false
      pass_end_flag = false;              // дублируем в меню Joy_Neutral, если переключили Joy до остановки и вызова текущего "if"
      //Print();
      BeepComplete();
      Motor_Z_RemovePulse();
      Motor_X_RemovePulse();
   }
}


//////////////////////
void Cone_Int_Left()
{  
   if (flag_end_operation ==false && cycle_flag == false && Motor_Z_Pos == Limit_Pos_Right)
   {
      a_flag = false;
      b_flag = false;
      cycle_flag = true;

      if (flag_cone == ON)
      {
         Limit_Pos_Front = (Null_X_Pos + REBOUND_X);
         limit_Front_flag = ON;
         Limit_Front_LED_On();
         BeepBeep();
      }
      else                                // на финальную точку
      {
         Limit_Pos_Front = Null_X_Pos;
      //   limit_Rear_flag = OFF;
      //   Limit_Rear_LED_Off();
      //   limit_Front_flag = OFF;
      //   Limit_Front_LED_Off();
         pass_end_flag = true;
      }
      
      if (Motor_X_Pos < (Limit_Pos_Front - ((MIN_RAPID_MOTION - MAX_RAPID_MOTION) * REPEAt) *2)) {Rapid_Feed_Front(a_flag, b_flag);}
      else                                                                                       {Feed_Front(a_flag, b_flag);}
   }


   else if (flag_cone == ON && Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Front)
   {
      a_flag = false;
      b_flag = false;
         
      float Cone_Div = (float)Cs_Divisor + ((float)Cm_Divisor /10000);
      long Cath_X = Ap * Pass_Nr / ((float)SCREW_X / MOTOR_X_STEP_PER_REV / McSTEP_X *2);       // Катет_X в микрошагах
      long Cath_Z = (long)(Cath_X * Cone_Div +McSTEP_Z /2) & ~(McSTEP_Z -1);                    // Катет_Z в микрошагах

      if (Limit_Pos_Right + Cath_Z >= tmp_Limit_Pos_Left)
      {
         flag_cone = OFF;  ////////////////////////////////////////////////////////////////
         Limit_Pos_Left = tmp_Limit_Pos_Left;
      }
      else
      {
         Limit_Pos_Left = Limit_Pos_Right + Cath_Z;
      }
      
      Pass_Nr++;
      if (Motor_Z_Pos < (Limit_Pos_Left - ((MIN_RAPID_MOTION - MAX_RAPID_MOTION) * REPEAt) *2)) {Step_X_flag = OFF; Rapid_Feed_Left(a_flag, b_flag);}
      else                                                                                      {Step_X_flag = OFF; Feed_Left(a_flag, b_flag);}
   }

   else if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Front)
   {
      a_flag = false;
      b_flag = false;
      
      Limit_Pos_Rear = Null_X_Pos;
      limit_Rear_flag = ON;
      Limit_Rear_LED_On();

      Feed_Rear(a_flag, b_flag); 
   }

   else if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Rear)
   {
      a_flag = false;
      b_flag = false;
      cycle_flag = false;
      
      Cone_Right(a_flag, b_flag); 
   }

   if (flag_end_operation == false && pass_end_flag == true && Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Front)   // Финиш 
   {
      Limit_Front_LED_Off();
      limit_Front_flag = OFF;
      Limit_Rear_LED_Off();
      limit_Rear_flag = OFF;
      Limit_Pos_Front = Limit_Pos_Max;
      Limit_Pos_Rear = Limit_Pos_Min;
      flag_end_operation = true;        // в меню Joy_Neutral - false
      pass_end_flag = false;            // дублируем в меню Joy_Neutral, если переключили Joy до остановки и вызова текущего "if"
      //Print();
      BeepComplete();
      Motor_Z_RemovePulse();
      Motor_X_RemovePulse();
   }
}