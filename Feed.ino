//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********** Режим "Подача" ********** //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Feed_Left(bool & a_flag, bool & b_flag)
{
   if (b_flag == true) return;
   a_flag = false;
   b_flag = true;

   Joy_X_flag = OFF;
   feed_X_flag = OFF;
   Start_Speed = ENC_LINE_PER_REV / ((float)MOTOR_Z_STEP_PER_REV * McSTEP_Z * MIN_FEED / SCREW_Z) / FEED_ACCEL /2 +0.5;
   if (Motor_Z_Pos < Limit_Pos_Left - Start_Speed * 2)
   {
      Feed_Divisor = ENC_LINE_PER_REV / ((float)MOTOR_Z_STEP_PER_REV * McSTEP_Z * Feed_mm / SCREW_Z) /2 +0.5;
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

   Motor_Z_Dir = CW;//Motor_Z_Dir = CW;
   Motor_Z_CW();
   if (Read_Z_Ena_State == false) Motor_Z_Enable();
   
   feed_Z_flag = ON;
   Joy_Z_flag = ON;//Joy_Z_flag = ON;
   Ena_INT_Z_Feed();
}

void Feed_Right(bool & a_flag, bool & b_flag)
{
   if (b_flag == true) return;
   a_flag = false;
   b_flag = true;

   Joy_X_flag = OFF;
   feed_X_flag = OFF;
   Start_Speed = ENC_LINE_PER_REV / ((float)MOTOR_Z_STEP_PER_REV * McSTEP_Z * MIN_FEED / SCREW_Z) / FEED_ACCEL /2 +0.5;
   if (Motor_Z_Pos > Limit_Pos_Right + Start_Speed * 2)
   {
      Feed_Divisor = ENC_LINE_PER_REV / ((float)MOTOR_Z_STEP_PER_REV * McSTEP_Z * Feed_mm / SCREW_Z) /2 +0.5;
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
   
   Brake_Compens = (max_OCR5A - Feed_Divisor) + 1;
   Limit_Pos = Limit_Pos_Right + Brake_Compens;

   Motor_Z_Dir = CCW;//Motor_Z_Dir = CCW;
   Motor_Z_CCW();
   if (Read_Z_Ena_State == false) Motor_Z_Enable();
   
   feed_Z_flag = ON;
   Joy_Z_flag = ON;//Joy_Z_flag = ON;
   Ena_INT_Z_Feed();
}

void Feed_Front(bool & a_flag, bool & b_flag)
{  
   if (b_flag == true) return;
   a_flag = false;
   b_flag = true;

   Joy_Z_flag = OFF;
   feed_Z_flag = OFF;
   Start_Speed = ENC_LINE_PER_REV / ((float)MOTOR_X_STEP_PER_REV * McSTEP_X * MIN_FEED / SCREW_X) / FEED_ACCEL /2 +0.5;
   if (Motor_X_Pos < Limit_Pos_Front - Start_Speed * 2)
   {
      Feed_Divisor = ENC_LINE_PER_REV / ((float)MOTOR_X_STEP_PER_REV * McSTEP_X * Feed_mm / SCREW_X) /2 +0.5;
      if (Feed_Divisor < Start_Speed)
      {
         max_OCR5A = Start_Speed;
         if (Step_X_flag == OFF)
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
   Limit_Pos = Limit_Pos_Front - Brake_Compens;

   Motor_X_Dir = CW;//  Motor_X_Dir = CW;
   Motor_X_CW();
   if (Read_X_Ena_State == false) Motor_X_Enable();
   
   feed_X_flag = ON;
   Joy_X_flag = ON;//Joy_X_flag = ON;
   Ena_INT_X_Feed();
   
}

void Feed_Rear(bool & a_flag, bool & b_flag)
{
   if (b_flag == true) return;
   a_flag = false;
   b_flag = true;

   Joy_Z_flag = OFF;
   feed_Z_flag = OFF;
   Start_Speed = ENC_LINE_PER_REV / ((float)MOTOR_X_STEP_PER_REV * McSTEP_X * MIN_FEED / SCREW_X) / FEED_ACCEL /2 +0.5;
   if (Motor_X_Pos > Limit_Pos_Rear + Start_Speed * 2)
   {
      Feed_Divisor = ENC_LINE_PER_REV / ((float)MOTOR_X_STEP_PER_REV * McSTEP_X * Feed_mm / SCREW_X) /2 +0.5;
      if (Feed_Divisor < Start_Speed)
      {
         max_OCR5A = Start_Speed;
         if (Step_X_flag == OFF)
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
   Limit_Pos = Limit_Pos_Rear + Brake_Compens;

   Motor_X_Dir = CCW;//Motor_X_Dir = CCW;
   Motor_X_CCW();
   if (Read_X_Ena_State == false) Motor_X_Enable();
   
   feed_X_flag = ON;
   Joy_X_flag = ON;//Joy_X_flag = ON;
   Ena_INT_X_Feed();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********** Режим "Быстрая Подача" ********** //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Rapid_Feed_Left(bool & a_flag, bool & b_flag)
{
   if (a_flag == true) return;
   b_flag = false;
   a_flag = true;

   Joy_X_flag = OFF;
   feed_X_flag = OFF;
   rapid_X_flag = OFF;
   Brake_Compens = (MIN_RAPID_MOTION - MAX_RAPID_MOTION) * REPEAt + 1;
   Limit_Pos = Limit_Pos_Left - Brake_Compens;

   Motor_Z_Dir = CW;//Motor_Z_Dir = CW;
   Motor_Z_CW();
   if (Read_Z_Ena_State == false) Motor_Z_Enable();
   
   Joy_Z_flag = ON;//Joy_Z_flag = ON;             
   rapid_Z_flag = ON;
   Ena_INT_Z_Rapid();
}

void Rapid_Feed_Right(bool & a_flag, bool & b_flag)
{
   if (a_flag == true) return;
   b_flag = false;
   a_flag = true;

   Joy_X_flag = OFF;
   feed_X_flag = OFF;
   rapid_X_flag = OFF;
   Brake_Compens = (MIN_RAPID_MOTION - MAX_RAPID_MOTION) * REPEAt + 1;
   Limit_Pos = Limit_Pos_Right + Brake_Compens;

   Motor_Z_Dir = CCW;//Motor_Z_Dir = CCW;
   Motor_Z_CCW();
   if (Read_Z_Ena_State == false) Motor_Z_Enable();
   
   Joy_Z_flag = ON;//Joy_Z_flag = ON;
   rapid_Z_flag = ON;
   Ena_INT_Z_Rapid();
}

void Rapid_Feed_Front(bool & a_flag, bool & b_flag)
{
   if (a_flag == true) return;
   b_flag = false;
   a_flag = true;

   Joy_Z_flag = OFF;
   feed_Z_flag = OFF;
   rapid_Z_flag = OFF;
   Brake_Compens = (MIN_RAPID_MOTION - MAX_RAPID_MOTION) * REPEAt + 1;
   Limit_Pos = Limit_Pos_Front - Brake_Compens;

   Motor_X_Dir = CW;//Motor_X_Dir = CW;
   Motor_X_CW();
   if (Read_X_Ena_State == false) Motor_X_Enable();
   
   Joy_X_flag = ON;//Joy_X_flag = ON;
   rapid_X_flag = ON;
   Ena_INT_X_Rapid();
}

void Rapid_Feed_Rear(bool & a_flag, bool & b_flag)
{
   if (a_flag == true) return;
   b_flag = false;
   a_flag = true;

   Joy_Z_flag = OFF;
   feed_Z_flag = OFF;
   rapid_Z_flag = OFF;
   Brake_Compens = (MIN_RAPID_MOTION - MAX_RAPID_MOTION) * REPEAt + 1;
   Limit_Pos = Limit_Pos_Rear + Brake_Compens;

   Motor_X_Dir = CCW;//Motor_X_Dir = CCW;
   Motor_X_CCW();
   if (Read_X_Ena_State == false) Motor_X_Enable();
   
   Joy_X_flag = ON;//Joy_X_flag = ON;
   rapid_X_flag = ON;
   Ena_INT_X_Rapid();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********** Режим "Цикл Синхронная Подача" ********** //////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Feed_Ext_Left()
{
   if ((Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Rear && Step_Z_flag == OFF) ||
       (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Null_X_Pos && Step_Z_flag == OFF))
   {  
      if (cycle_flag == false && Pass_Nr <= Pass_Total)
      {
         cycle_flag = true;
         a_flag = false;
         b_flag = false;

         long Infeed_Value = (int)((float)MOTOR_X_STEP_PER_REV * Ap / SCREW_X + 0.5) * McSTEP_X; // Величина текущего заглубления в микрошагах
         if (Infeed_Value == 0)                                                                  // Ap==0 и если расчетная величина подачи < 1
         {
            if (Pass_Nr == 1) Limit_Pos_Front = (Null_X_Pos + 1);                                // +1 mcStep не накапливающийся
            else              Limit_Pos_Front = (Limit_Pos_Rear + REBOUND_X);
         }
         else
         {
            if (Pass_Nr == 1) Limit_Pos_Front = (Null_X_Pos + Infeed_Value);
            else              Limit_Pos_Front = (Limit_Pos_Rear + REBOUND_X + Infeed_Value);
         }
         Limit_Front_LED_On();
         limit_Front_flag = ON;
         BeepBeep();
         Feed_Front(a_flag, b_flag);
      }
      
      else if (cycle_flag == false && Pass_Nr > Pass_Total) // На финальную точку
      {
         a_flag = false;
         b_flag = false;

         Limit_Pos_Front = Limit_Pos_Rear + REBOUND_X ;
         Feed_Front(a_flag, b_flag);

        // Limit_Front_LED_Off();
        // limit_Front_flag = OFF;
        // Limit_Rear_LED_Off();
        // limit_Rear_flag = OFF;
        // Limit_Pos_Front = Limit_Pos_Max;
        // Limit_Pos_Rear = Limit_Pos_Min;
        // Pass_Total = 1;
        // Pass_Nr = 1;
        // Print();
      }
   }

   else if (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Front && Step_X_flag == OFF)
   {
      if (Pass_Nr <= Pass_Total)
      {
         a_flag = false;
         b_flag = false;

         Feed_Left(a_flag, b_flag);
      }
   }

   else if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Front && Step_Z_flag == OFF)
   {
      cycle_flag = false;
      a_flag = false;
      b_flag = false;
      
      Limit_Pos_Rear = (Limit_Pos_Front - REBOUND_X);
      Limit_Rear_LED_On();
      limit_Rear_flag = ON;
      Feed_Rear(a_flag, b_flag);
   }

   else if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Rear && Step_X_flag == OFF)
   {
      a_flag = false;
      b_flag = false;

      Pass_Nr++;
      //Print();
      Rapid_Feed_Right(a_flag, b_flag);
   }

   if (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Front && Pass_Nr > Pass_Total) // Финиш
   {
      Limit_Front_LED_Off();                    //
      limit_Front_flag = OFF;
      Limit_Rear_LED_Off();
      limit_Rear_flag = OFF;
      Limit_Pos_Front = Limit_Pos_Max;
      Limit_Pos_Rear = Limit_Pos_Min;
      Pass_Total = 1;
      Pass_Nr = 1;
      flag_end_operation = true;//////////////////
      //Print();
      BeepComplete();
      Motor_Z_RemovePulse();
      Motor_X_RemovePulse();
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void Feed_Ext_Right()
{
   if ((Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Rear && Step_Z_flag == OFF) ||
       (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Null_X_Pos && Step_Z_flag == OFF))
   {  
      if (cycle_flag == false && Pass_Nr <= Pass_Total)
      {
         cycle_flag = true;
         a_flag = false;
         b_flag = false;
         
         long Infeed_Value = (int)((float)MOTOR_X_STEP_PER_REV * Ap / SCREW_X + 0.5) * McSTEP_X;   // Величина текущего заглубления в микрошагах
         if (Infeed_Value == 0)                                                                    // Ap==0 и если расчетная величина подачи < 1
         {
            if (Pass_Nr == 1) Limit_Pos_Front = (Null_X_Pos + 1);                                   // +1 mcStep не накапливающийся
            else              Limit_Pos_Front = (Limit_Pos_Rear + REBOUND_X);
         }
         else
         {
            if (Pass_Nr == 1) Limit_Pos_Front = (Null_X_Pos + Infeed_Value);
            else              Limit_Pos_Front = (Limit_Pos_Rear + REBOUND_X + Infeed_Value);
         }
         Limit_Front_LED_On();
         limit_Front_flag = ON;
         BeepBeep();
         Feed_Front(a_flag, b_flag);
      }

      else if (cycle_flag == false && Pass_Nr > Pass_Total) // На финальную точку
      {
         a_flag = false;
         b_flag = false;
         
         Limit_Pos_Front = Limit_Pos_Rear + REBOUND_X;
         Feed_Front(a_flag, b_flag);

        // Limit_Front_LED_Off();
        // limit_Front_flag = OFF;
        // Limit_Rear_LED_Off();
        // limit_Rear_flag = OFF;
        // Limit_Pos_Front = Limit_Pos_Max;
        // Limit_Pos_Rear = Limit_Pos_Min;
        // Pass_Total = 1;
        // Pass_Nr = 1;
        // Print();
      }
   }

   else if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Front && Step_X_flag == OFF)  
   {
      if (Pass_Nr <= Pass_Total)
      {
         a_flag = false;
         b_flag = false;

         Feed_Right(a_flag, b_flag);
      }
   }

   else if (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Front && Step_Z_flag == OFF)
   {
      cycle_flag = false;
      a_flag = false;
      b_flag = false;
      
      Limit_Pos_Rear = (Limit_Pos_Front - REBOUND_X);
      Limit_Rear_LED_On();
      limit_Rear_flag = ON;
      Feed_Rear(a_flag, b_flag);
   }

   else if (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Rear && Step_X_flag == OFF)
   {
      a_flag = false;
      b_flag = false;

      Pass_Nr++;
      //Print();
      Rapid_Feed_Left(a_flag, b_flag);
   }

   if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Front && Pass_Nr > Pass_Total) // Финиш
   {
      Limit_Front_LED_Off();
      limit_Front_flag = OFF;
      Limit_Rear_LED_Off();
      limit_Rear_flag = OFF;
      Limit_Pos_Front = Limit_Pos_Max;
      Limit_Pos_Rear = Limit_Pos_Min;
      Pass_Total = 1;
      Pass_Nr = 1;
      flag_end_operation = true;//////////////////
      //Print();
      BeepComplete();
      Motor_Z_RemovePulse();
      Motor_X_RemovePulse();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void Feed_Int_Left()
{
   if ((Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Front && Step_Z_flag == OFF) ||
       (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Null_X_Pos && Step_Z_flag == OFF))
   {  
      if (cycle_flag == false && Pass_Nr <= Pass_Total)
      {
         cycle_flag = true;
         a_flag = false;
         b_flag = false;

         long Infeed_Value = (int)((float)MOTOR_X_STEP_PER_REV * Ap / SCREW_X + 0.5) * McSTEP_X;   // Величина текущего заглубления в микрошагах
         if (Infeed_Value == 0)                                                                    // Ap==0 и если расчетная величина подачи < 1
         {
            if (Pass_Nr == 1) Limit_Pos_Rear = (Null_X_Pos - 1);                                   // +1 mcStep не накапливающийся
            else              Limit_Pos_Rear = (Limit_Pos_Front - REBOUND_X);
         }
         else
         {
            if (Pass_Nr == 1) Limit_Pos_Rear = (Null_X_Pos - Infeed_Value);
            else              Limit_Pos_Rear = (Limit_Pos_Front - REBOUND_X - Infeed_Value);
         }
         Limit_Rear_LED_On();
         limit_Rear_flag = ON;
         BeepBeep();
         Feed_Rear(a_flag, b_flag);
      }
      
      else if (cycle_flag == false && Pass_Nr > Pass_Total) // На финальную точку
      {
         a_flag = false;
         b_flag = false;
         
         Limit_Pos_Rear = Limit_Pos_Front - REBOUND_X;
         Feed_Rear(a_flag, b_flag);

       //  Limit_Front_LED_Off();
       //  limit_Front_flag = OFF;
       //  Limit_Rear_LED_Off();
       //  limit_Rear_flag = OFF;
       //  Limit_Pos_Front = Limit_Pos_Max;
       //  Limit_Pos_Rear = Limit_Pos_Min;
       //  Pass_Total = 1;
       //  Pass_Nr = 1;
       //  Print();
      }
   }

   else if (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Rear && Step_X_flag == OFF)
   {
      if (Pass_Nr <= Pass_Total)
      {
         a_flag = false;
         b_flag = false;

         Feed_Left(a_flag, b_flag);
      }
   }

   else if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Rear && Step_Z_flag == OFF)
   {
      cycle_flag = false;
      a_flag = false;
      b_flag = false;
      
      Limit_Pos_Front = (Limit_Pos_Rear + REBOUND_X);
      Limit_Front_LED_On();
      limit_Front_flag = ON;
      Feed_Front(a_flag, b_flag);
   }

   else if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Front && Step_X_flag == OFF)
   {
      a_flag = false;
      b_flag = false;

      Pass_Nr++;
      //Print();
      Rapid_Feed_Right(a_flag, b_flag);
   }

   if (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Rear && Pass_Nr > Pass_Total) // Финиш
   {
      Limit_Front_LED_Off();
      limit_Front_flag = OFF;
      Limit_Rear_LED_Off();
      limit_Rear_flag = OFF;
      Limit_Pos_Front = Limit_Pos_Max;
      Limit_Pos_Rear = Limit_Pos_Min;
      Pass_Total = 1;
      Pass_Nr = 1;
      flag_end_operation = true;//////////////////
      //Print();
      BeepComplete();
      Motor_Z_RemovePulse();
      Motor_X_RemovePulse();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void Feed_Int_Right()
{
   if ((Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Front && Step_Z_flag == OFF) ||
       (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Null_X_Pos && Step_Z_flag == OFF))
   {  
      if (cycle_flag == false && Pass_Nr <= Pass_Total)
      {
         cycle_flag = true;
         a_flag = false;
         b_flag = false;
         
         long Infeed_Value = (int)((float)MOTOR_X_STEP_PER_REV * Ap / SCREW_X + 0.5) * McSTEP_X;   // Величина текущего заглубления в микрошагах
         if (Infeed_Value == 0)                                                                    // Ap==0 и если расчетная величина подачи < 1
         {
            if (Pass_Nr == 1) Limit_Pos_Rear = (Null_X_Pos - 1);                                   // +1 mcStep не накапливающийся
            else              Limit_Pos_Rear = (Limit_Pos_Front - REBOUND_X);
         }
         else
         {
            if (Pass_Nr == 1) Limit_Pos_Rear = (Null_X_Pos - Infeed_Value);
            else              Limit_Pos_Rear = (Limit_Pos_Front - REBOUND_X - Infeed_Value);
         }
         Limit_Rear_LED_On();
         limit_Rear_flag = ON;
         BeepBeep();
         Feed_Rear(a_flag, b_flag);
      }

      else if (cycle_flag == false && Pass_Nr > Pass_Total) // На финальную точку
      {
         a_flag = false;
         b_flag = false;
         
         Limit_Pos_Rear = Limit_Pos_Front - REBOUND_X;
         Feed_Rear(a_flag, b_flag);

       //  Limit_Front_LED_Off();
       //  limit_Front_flag = OFF;
       //  Limit_Rear_LED_Off();
       //  limit_Rear_flag = OFF;
       //  Limit_Pos_Front = Limit_Pos_Max;
       //  Limit_Pos_Rear = Limit_Pos_Min;
       //  Pass_Total = 1;
       //  Pass_Nr = 1;
       //  Print();
      }
   }

   else if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Rear && Step_X_flag == OFF)
   {
      if (Pass_Nr <= Pass_Total)
      {
         a_flag = false;
         b_flag = false;

         Feed_Right(a_flag, b_flag);
      }
   }

   else if (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Rear && Step_Z_flag == OFF)
   {
      cycle_flag = false;
      a_flag = false;
      b_flag = false;
      
      Limit_Pos_Front = (Limit_Pos_Rear + REBOUND_X);
      Limit_Front_LED_On();
      limit_Front_flag = ON;
      Feed_Front(a_flag, b_flag);
   }

   else if (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos == Limit_Pos_Front && Step_X_flag == OFF)
   {
      a_flag = false;
      b_flag = false;

      Pass_Nr++;
      //Print();
      Rapid_Feed_Left(a_flag, b_flag);
   }

   if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Rear && Pass_Nr > Pass_Total) // Финиш
   {
      Limit_Front_LED_Off();
      limit_Front_flag = OFF;
      Limit_Rear_LED_Off();
      limit_Rear_flag = OFF;
      Limit_Pos_Front = Limit_Pos_Max;
      Limit_Pos_Rear = Limit_Pos_Min;
      Pass_Total = 1;
      Pass_Nr = 1;
      flag_end_operation = true;//////////////////
      //Print();
      BeepComplete();
      Motor_Z_RemovePulse();
      Motor_X_RemovePulse();
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////
void Feed_Ext_Front() // торцевание к центру, к патрону
{
   if ((Motor_X_Pos == Limit_Pos_Rear && Motor_Z_Pos == Limit_Pos_Right && Step_X_flag == OFF) ||
       (Motor_X_Pos == Limit_Pos_Rear && Motor_Z_Pos == Null_Z_Pos && Step_X_flag == OFF))
   {  
      if (cycle_flag == false && Pass_Nr <= Pass_Total)
      {
         cycle_flag = true;
         a_flag = false;
         b_flag = false;

         long Infeed_Value = (int)((float)MOTOR_Z_STEP_PER_REV * Ap / SCREW_Z + 0.5) * McSTEP_Z;   // Величина текущего заглубления в микрошагах
         if (Infeed_Value == 0)                                                                    // Ap==0 и если расчетная величина подачи < 1
         {
            if (Pass_Nr == 1) Limit_Pos_Left = (Null_Z_Pos + 1);                                   // +1 mcStep не накапливающийся
            else              Limit_Pos_Left = (Limit_Pos_Right + REBOUND_Z);
         }
         else
         {
            if (Pass_Nr == 1) Limit_Pos_Left = (Null_Z_Pos + Infeed_Value);
            else              Limit_Pos_Left = (Limit_Pos_Right + REBOUND_Z + Infeed_Value);
         }
         Limit_Left_LED_On();
         limit_Left_flag = ON;
         BeepBeep();
         Feed_Left(a_flag, b_flag);
      }
      
      else if (cycle_flag == false && Pass_Nr > Pass_Total) // На финальную точку
      {
         a_flag = false;
         b_flag = false;
         
         Limit_Pos_Left = Limit_Pos_Right + REBOUND_Z;
         Feed_Left(a_flag, b_flag);

        // Limit_Left_LED_Off();
        // limit_Left_flag = OFF;
        // Limit_Right_LED_Off();
        // limit_Right_flag = OFF;
        // Limit_Pos_Left = Limit_Pos_Max;
        // Limit_Pos_Right = Limit_Pos_Min;
        // Pass_Total = 1;
        // Pass_Nr = 1;
        // Print();
      }
   }

   else if (Motor_X_Pos == Limit_Pos_Rear && Motor_Z_Pos == Limit_Pos_Left && Step_Z_flag == OFF)
   {
      if (Pass_Nr <= Pass_Total)
      {
         a_flag = false;
         b_flag = false;

         Feed_Front(a_flag, b_flag);
      }
   }

   else if (Motor_X_Pos == Limit_Pos_Front && Motor_Z_Pos == Limit_Pos_Left && Step_X_flag == OFF)
   {
      cycle_flag = false;
      a_flag = false;
      b_flag = false;
      
      Limit_Pos_Right = (Limit_Pos_Left - REBOUND_Z);
      Limit_Right_LED_On();
      limit_Right_flag = ON;
      Feed_Right(a_flag, b_flag);
   }

   else if (Motor_X_Pos == Limit_Pos_Front && Motor_Z_Pos == Limit_Pos_Right && Step_Z_flag == OFF)
   {
      a_flag = false;
      b_flag = false;

      Pass_Nr++;
      //Print();
      Rapid_Feed_Rear(a_flag, b_flag);
   }

   if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Rear && Pass_Nr > Pass_Total) // Финиш
   {
      Limit_Left_LED_Off();
      limit_Left_flag = OFF;
      Limit_Right_LED_Off();
      limit_Right_flag = OFF;
      Limit_Pos_Left = Limit_Pos_Max;
      Limit_Pos_Right = Limit_Pos_Min;
      Pass_Total = 1;
      Pass_Nr = 1;
      flag_end_operation = true;//////////////////
      //Print();
      BeepComplete();
      Motor_Z_RemovePulse();
      Motor_X_RemovePulse();
   }
}

//////////////////////////////////////////////////////////////////////////////
void Feed_Ext_Rear()   // торцевание от центра, к патрону
{
   if ((Motor_X_Pos == Limit_Pos_Front && Motor_Z_Pos == Limit_Pos_Right && Step_X_flag == OFF) ||
       (Motor_X_Pos == Limit_Pos_Front && Motor_Z_Pos == Null_Z_Pos && Step_X_flag == OFF))
   {  
      if (cycle_flag == false && Pass_Nr <= Pass_Total)
      {
         cycle_flag = true;
         a_flag = false;
         b_flag = false;

         long Infeed_Value = (int)((float)MOTOR_Z_STEP_PER_REV * Ap / SCREW_Z + 0.5) * McSTEP_Z;   // Величина текущего заглубления в микрошагах
         if (Infeed_Value == 0)                                                                    // Ap==0 и если расчетная величина подачи < 1
         {
            if (Pass_Nr == 1) Limit_Pos_Left = (Null_Z_Pos + 1);                                   // +1 mcStep не накапливающийся
            else              Limit_Pos_Left = (Limit_Pos_Right + REBOUND_Z);
         }
         else
         {
            if (Pass_Nr == 1) Limit_Pos_Left = (Null_Z_Pos + Infeed_Value);
            else              Limit_Pos_Left = (Limit_Pos_Right + REBOUND_Z + Infeed_Value);
         }
         Limit_Left_LED_On();
         limit_Left_flag = ON;
         BeepBeep();
         Feed_Left(a_flag, b_flag);
      }
      
      else if (cycle_flag == false && Pass_Nr > Pass_Total) // На финальную точку
      {
         a_flag = false;
         b_flag = false;
         
         Limit_Pos_Left = Limit_Pos_Right + REBOUND_Z;
         Feed_Left(a_flag, b_flag);

        // Limit_Left_LED_Off();
        // limit_Left_flag = OFF;
        // Limit_Right_LED_Off();
        // limit_Right_flag = OFF;
        // Limit_Pos_Left = Limit_Pos_Max;
        // Limit_Pos_Right = Limit_Pos_Min;
        // Pass_Total = 1;
        // Pass_Nr = 1;
        // Print();
      }
   }

   else if (Motor_X_Pos == Limit_Pos_Front && Motor_Z_Pos == Limit_Pos_Left && Step_Z_flag == OFF)
   {
      if (Pass_Nr <= Pass_Total)
      {
         a_flag = false;
         b_flag = false;

         Feed_Rear(a_flag, b_flag);
      }
   }

   else if (Motor_X_Pos == Limit_Pos_Rear && Motor_Z_Pos == Limit_Pos_Left && Step_X_flag == OFF)
   {
      cycle_flag = false;
      a_flag = false;
      b_flag = false;
      
      Limit_Pos_Right = (Limit_Pos_Left - REBOUND_Z);
      Limit_Right_LED_On();
      limit_Right_flag = ON;
      Feed_Right(a_flag, b_flag);
   }

   else if (Motor_X_Pos == Limit_Pos_Rear && Motor_Z_Pos == Limit_Pos_Right && Step_Z_flag == OFF)
   {
      a_flag = false;
      b_flag = false;

      Pass_Nr++;
      //Print();
      Rapid_Feed_Front(a_flag, b_flag);
   }

   if (Motor_Z_Pos == Limit_Pos_Left && Motor_X_Pos == Limit_Pos_Front && Pass_Nr > Pass_Total) // Финиш
   {
      Limit_Left_LED_Off();
      limit_Left_flag = OFF;
      Limit_Right_LED_Off();
      limit_Right_flag = OFF;
      Limit_Pos_Left = Limit_Pos_Max;
      Limit_Pos_Right = Limit_Pos_Min;
      Pass_Total = 1;
      Pass_Nr = 1;
      flag_end_operation = true;//////////////////
      //Print();
      BeepComplete();
      Motor_Z_RemovePulse();
      Motor_X_RemovePulse();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void Feed_Int_Front()  // торцевание к центру, от патрона
{/*
   if ((Motor_X_Pos == Limit_Pos_Rear && Motor_Z_Pos == Limit_Pos_Left && Step_X_flag == OFF) ||
       (Motor_Z_Pos == Null_Z_Pos && Motor_X_Pos <= Limit_Pos_Rear + McSTEP_X/2 && Motor_X_Pos >= Limit_Pos_Rear - McSTEP_X/2 && Step_X_flag == OFF))
   {  
      if (cycle_flag == false && Pass_Nr <= Pass_Total)
      {
         cycle_flag = true;
         a_flag = false;
         b_flag = false;

         long Infeed_Value = (int)((float)MOTOR_Z_STEP_PER_REV * Ap / SCREW_Z + 0.5) * McSTEP_Z;   // Величина текущего заглубления в микрошагах
         if (Pass_Nr == 1) Limit_Pos_Right = (Null_Z_Pos - Infeed_Value);
         else              Limit_Pos_Right = (Limit_Pos_Left - REBOUND_Z - Infeed_Value);
         Limit_Right_LED_On();
         BeepBeep();
         Feed_Right(a_flag, b_flag);
      }
      
      else if (cycle_flag == false && Pass_Nr > Pass_Total) // На финальную точку
      {
         a_flag = false;
         b_flag = false;
         
         Limit_Pos_Right = Limit_Pos_Left - REBOUND_Z;
         Limit_Left_LED_Off();
         Limit_Right_LED_Off();
         BeepBeep();
         Feed_Right(a_flag, b_flag);

         Limit_Pos_Left = Limit_Pos_Max;
         Limit_Pos_Right = Limit_Pos_Min;
      }
   }

   else if ((Motor_X_Pos == Limit_Pos_Rear && Motor_Z_Pos == Limit_Pos_Right && Step_Z_flag == OFF) ||
            (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos <= Limit_Pos_Rear + McSTEP_X/2 && Motor_X_Pos >= Limit_Pos_Rear - McSTEP_X/2 && Step_Z_flag == OFF))
   {
      if (Pass_Nr <= Pass_Total)
      {
         a_flag = false;
         b_flag = false;

         Pass_Nr++;
         Feed_Front(a_flag, b_flag);
      }
   }

   else if (Motor_X_Pos == Limit_Pos_Front && Motor_Z_Pos == Limit_Pos_Right && Step_X_flag == OFF)
   {
      cycle_flag = false;
      a_flag = false;
      b_flag = false;
      
      Limit_Pos_Left = (Limit_Pos_Right + REBOUND_Z);
      Limit_Left_LED_On();
      Feed_Left(a_flag, b_flag);
   }

   else if (Motor_X_Pos == Limit_Pos_Front && Motor_Z_Pos == Limit_Pos_Left && Step_Z_flag == OFF)
   {
      a_flag = false;
      b_flag = false;

      Rapid_Feed_Rear(a_flag, b_flag);
   }*/
}

////////////////////////////////////////////////////////////////////////////////////////////////
void Feed_Int_Rear()  // торцевание от центра, от патрона
{
  /* if ((Motor_X_Pos == Limit_Pos_Front && Motor_Z_Pos == Limit_Pos_Left && Step_X_flag == OFF) ||
       (Motor_Z_Pos == Null_Z_Pos && Motor_X_Pos <= Limit_Pos_Front + McSTEP_X/2 && Motor_X_Pos >= Limit_Pos_Front - McSTEP_X/2 && Step_X_flag == OFF))
   {  
      if (cycle_flag == false && Pass_Nr <= Pass_Total)
      {
         cycle_flag = true;
         a_flag = false;
         b_flag = false;

         long Infeed_Value = (int)((float)MOTOR_Z_STEP_PER_REV * Ap / SCREW_Z + 0.5) * McSTEP_Z;   // Величина текущего заглубления в микрошагах
         if (Pass_Nr == 1) Limit_Pos_Right = (Null_Z_Pos - Infeed_Value);
         else              Limit_Pos_Right = (Limit_Pos_Left - REBOUND_Z - Infeed_Value);
         Limit_Right_LED_On();
         BeepBeep();
         Feed_Right(a_flag, b_flag);
      }
      
      else if (cycle_flag == false && Pass_Nr > Pass_Total) // На финальную точку
      {
         a_flag = false;
         b_flag = false;
         
         Limit_Pos_Right = Limit_Pos_Left - REBOUND_Z;
         Limit_Left_LED_Off();
         Limit_Right_LED_Off();
         BeepBeep();
         Feed_Right(a_flag, b_flag);

         Limit_Pos_Left = Limit_Pos_Max;
         Limit_Pos_Right = Limit_Pos_Min;
      }
   }

   else if ((Motor_X_Pos == Limit_Pos_Front && Motor_Z_Pos == Limit_Pos_Right && Step_Z_flag == OFF) ||
            (Motor_Z_Pos == Limit_Pos_Right && Motor_X_Pos <= Limit_Pos_Front + McSTEP_X/2 && Motor_X_Pos >= Limit_Pos_Front - McSTEP_X/2 && Step_Z_flag == OFF))
   {
      if (Pass_Nr <= Pass_Total)
      {
         a_flag = false;
         b_flag = false;

         Pass_Nr++;
         Feed_Rear(a_flag, b_flag);
      }
   }

   else if (Motor_X_Pos == Limit_Pos_Rear && Motor_Z_Pos == Limit_Pos_Right && Step_X_flag == OFF)
   {
      cycle_flag = false;
      a_flag = false;
      b_flag = false;
      
      Limit_Pos_Left = (Limit_Pos_Right + REBOUND_Z);
      Limit_Left_LED_On();
      Feed_Left(a_flag, b_flag);
   }

   else if (Motor_X_Pos == Limit_Pos_Rear && Motor_Z_Pos == Limit_Pos_Left && Step_Z_flag == OFF)
   {
      a_flag = false;
      b_flag = false;

      Rapid_Feed_Front(a_flag, b_flag);
   }*/
}
