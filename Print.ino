//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ***** Print ***** /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Print_ZX()
{
  ////////////////////////////////////////////////////////////////////////////////////////
  // Axis X
  /////////////////////////////////////////////////////////////////////////////////////////

  long Axis_X_mm = lround(-(float)(Motor_X_Pos - DRO_Shift_X) * SCREW_X / (MOTOR_X_STEP_PER_REV * McSTEP_X));  // инвертируем направление счета
  int A_X = Axis_X_mm / 100;
  int B_X = Axis_X_mm % 100;

  if ((A_X < 0) || (B_X < 0))  // negative value
  {
    A_X = abs(A_X);
    B_X = abs(B_X);
     
    if( Mode == Mode_Thread && Sub_Mode_Feed != Sub_Mode_Thread_Man ) {
      if (A_X < 10)        { snprintf(LCD_Row_1, 17, "X:    -%1d.%02d P:%02d", A_X, B_X, Pass_Total+THRD_PASS_FINISH-Pass_Nr+1 );}  // 1 знак, 3 строки для вывода точки на свое знакоместо
      else if (A_X < 100)  { snprintf(LCD_Row_1, 17, "X:   -%2d.%02d P:%02d", A_X, B_X, Pass_Total+THRD_PASS_FINISH-Pass_Nr+1 ); }  // 2 знака
      else if (A_X < 1000) { snprintf(LCD_Row_1, 17, "X:  -%3d.%02d P:%02d", A_X, B_X, Pass_Total+THRD_PASS_FINISH-Pass_Nr+1 ); }  // 3 знака
    } else {
      if (A_X < 10)        { snprintf(LCD_Row_1, 17, "X:    -%1d.%02d", A_X, B_X );}  // 1 знак, 3 строки для вывода точки на свое знакоместо
      else if (A_X < 100)  { snprintf(LCD_Row_1, 17, "X:   -%2d.%02d", A_X, B_X ); }  // 2 знака
      else if (A_X < 1000) { snprintf(LCD_Row_1, 17, "X:  -%3d.%02d", A_X, B_X ); }  // 3 знака
    }
  } else { // positive value
    if( Mode == Mode_Thread && Sub_Mode_Feed != Sub_Mode_Thread_Man ) 
      snprintf(LCD_Row_1, 17, "X:   %3d.%02d P:%02d", A_X, B_X, Pass_Total+THRD_PASS_FINISH-Pass_Nr+1 );  // 1 знак, 3 строки для вывода точки на свое знакоместо
    else
      snprintf(LCD_Row_1, 17, "X:   %3d.%02d", A_X, B_X );  // 1 знак, 3 строки для вывода точки на свое знакоместо
  }

  ////////////////////////////////////////////////////////////////////////////////////////
  // Axis Z
  /////////////////////////////////////////////////////////////////////////////////////////
  //long Axis_Z_mm = lround((float)(Motor_Z_Pos -DRO_Shift_Z) *SCREW_Z /(MOTOR_Z_STEP_PER_REV *McSTEP_Z));
  long Axis_Z_mm = lround(-(float)(Motor_Z_Pos - DRO_Shift_Z) * SCREW_Z / (MOTOR_Z_STEP_PER_REV * McSTEP_Z));  // инвертируем направление счета
  int A_Z = Axis_Z_mm / 100;
  int B_Z = Axis_Z_mm % 100;

  if ((A_Z < 0) || (B_Z < 0))  // negative value
  {
    A_Z = abs(A_Z);
    B_Z = abs(B_Z);
    if      (A_Z   < 10) { snprintf(LCD_Row_2, 17, "Z:    -%1d.%02d", A_Z, B_Z ); }  // 1 знак, 3 строки для вывода точки на свое знакоместо
    else if (A_Z  < 100) { snprintf(LCD_Row_2, 17, "Z:   -%2d.%02d", A_Z, B_Z ); }  // 2 знака
    else if (A_Z < 1000) { snprintf(LCD_Row_2, 17, "Z:  -%3d.%02d", A_Z, B_Z ); }  // 3 знака
  } else {// positive value
    snprintf(LCD_Row_2, 17, "Z:   %3d.%02d", A_Z, B_Z);
  }

   lcd.setCursor(0, 0);
   lcd.print(LCD_Row_1);
   lcd.print("     ");

   lcd.setCursor(0, 1);
   lcd.print(LCD_Row_2);
   lcd.print("     ");
}

void Print()
{
   if ( (LCD_Page == 2) && ( !flag_end_operation ) ) Print_ZX();
   else if (Mode == Mode_Thread)  //////////////////////////////////////////////////////////
   {
      //switch (LCD_Page) {
      //  case 1:
          if (Sub_Mode_Thread == Sub_Mode_Thread_Man) snprintf(LCD_Row_1, 17, "Thrd      %s", Thread_Info[Thread_Step].Thread_Print);
          else                                        snprintf(LCD_Row_1, 17, "Thrd P:%02d %s", Pass_Total+THRD_PASS_FINISH-Pass_Nr+1, Thread_Info[Thread_Step].Thread_Print);
          if      (Sub_Mode_Thread == Sub_Mode_Thread_Int) snprintf(LCD_Row_2, 17, "Int Max:%s rpm", Thread_Info[Thread_Step].Limit_Print);
          else if (Sub_Mode_Thread == Sub_Mode_Thread_Man) snprintf(LCD_Row_2, 17, "Man Max:%s rpm", Thread_Info[Thread_Step].Limit_Print);
          else if (Sub_Mode_Thread == Sub_Mode_Thread_Ext) snprintf(LCD_Row_2, 17, "Ext Max:%s rpm", Thread_Info[Thread_Step].Limit_Print);
      //  break;
      //}
   } 
   else if (Mode == Mode_Feed)  //////////////////////////////////////////////////////////
   {
      switch (LCD_Page) {
        case 1:
          snprintf(LCD_Row_1, 17, "Feed mm/rev %1d.%02dmm", Feed_mm/100, Feed_mm%100);
          if      (Sub_Mode_Feed == Sub_Mode_Feed_Int) snprintf(LCD_Row_2, 17, "Int P:%02d A:%02d.%02d", Pass_Total-Pass_Nr+1, Ap/100, Ap%100);
          else if (Sub_Mode_Feed == Sub_Mode_Feed_Man) snprintf(LCD_Row_2, 17, "Man P:%02d A:%02d.%02d", Pass_Total, Ap/100, Ap%100);
          else if (Sub_Mode_Feed == Sub_Mode_Feed_Ext) snprintf(LCD_Row_2, 17, "Ext P:%02d A:%02d.%02d", Pass_Total-Pass_Nr+1, Ap/100, Ap%100);
        break;
      }
   }
   else if (Mode == Mode_aFeed)  //////////////////////////////////////////////////////////
   {
      switch (LCD_Page) {
        case 1:
          snprintf(LCD_Row_1, 17, "aFeed mm/min %03d", aFeed_mm);
          if      (Sub_Mode_aFeed == Sub_Mode_aFeed_Int) snprintf(LCD_Row_2, 17, "Int P:%02d A:%02d.%02d", Pass_Total, Ap/100, Ap%100);
          else if (Sub_Mode_aFeed == Sub_Mode_aFeed_Man) snprintf(LCD_Row_2, 17, "Man P:%02d A:%02d.%02d", Pass_Total, Ap/100, Ap%100);
          else if (Sub_Mode_aFeed == Sub_Mode_aFeed_Ext) snprintf(LCD_Row_2, 17, "Ext P:%02d A:%02d.%02d", Pass_Total, Ap/100, Ap%100);
        break;
      }
   }
   else if (Mode == Mode_Cone_L)  //////////////////////////////////////////////////////////////
   {
      snprintf(LCD_Row_1, 17, "Cone < %s %1d.%02dmm", Cone_Info[Cone_Step].Cone_Print, Feed_mm/100, Feed_mm%100);
      if      (Sub_Mode_Cone == Sub_Mode_Cone_Int) snprintf(LCD_Row_2, 17, "Int P:%02d A:%02d.%02d", Pass_Total, Ap/100, Ap%100);
      else if (Sub_Mode_Cone == Sub_Mode_Cone_Man) snprintf(LCD_Row_2, 17, "Man P:%02d A:%02d.%02d", Pass_Total, Ap/100, Ap%100);
      else if (Sub_Mode_Cone == Sub_Mode_Cone_Ext) snprintf(LCD_Row_2, 17, "Ext P:%02d A:%02d.%02d", Pass_Total, Ap/100, Ap%100);
   }
   else if (Mode == Mode_Cone_R)  //////////////////////////////////////////////////////////////
   {
      snprintf(LCD_Row_1, 17, "Cone > %s %1d.%02dmm", Cone_Info[Cone_Step].Cone_Print, Feed_mm/100, Feed_mm%100);
      if      (Sub_Mode_Cone == Sub_Mode_Cone_Int) snprintf(LCD_Row_2, 17, "Int P:%02d A:%02d.%02d", Pass_Total, Ap/100, Ap%100);
      else if (Sub_Mode_Cone == Sub_Mode_Cone_Man) snprintf(LCD_Row_2, 17, "Man P:%02d A:%02d.%02d", Pass_Total, Ap/100, Ap%100);
      else if (Sub_Mode_Cone == Sub_Mode_Cone_Ext) snprintf(LCD_Row_2, 17, "Ext P:%02d A:%02d.%02d", Pass_Total, Ap/100, Ap%100);
   }
   else if (Mode == Mode_Reserve)  //////////////////////////////////////////////////////////////
   {
      snprintf(LCD_Row_1, 17, " Reserve/PE3EPB ");
      snprintf(LCD_Row_2, 17, " Reserve/PE3EPB ");
   }
   else if (Mode == Mode_Sphere)  //////////////////////////////////////////////////////////////
   {
      if (!key_sel_flag)
      {
         snprintf(LCD_Row_1, 17, "Sphr %2ld.%01ldmm %1d.%02dmm", Sph_R_mm * 2 / 100, Sph_R_mm * 2 / 10 %10, Feed_mm/100, Feed_mm%100);
         if      (Sub_Mode_Sphere == Sub_Mode_Sphere_Int) snprintf(LCD_Row_2, 17, "Mode not Exist  ");
         else if (Sub_Mode_Sphere == Sub_Mode_Sphere_Man) snprintf(LCD_Row_2, 17, "     BarDia %2ld.%01ld", Bar_R_mm*2/100, Bar_R_mm*2%100);
         else if (Sub_Mode_Sphere == Sub_Mode_Sphere_Ext) snprintf(LCD_Row_2, 17, "Ext  BarDia %2ld.%01ld", Bar_R_mm*2/100, Bar_R_mm*2%100);
      }
      else
      {
         snprintf(LCD_Row_1, 17, "Cut.Width %1d.%02dmm", Cutter_Width/100, Cutter_Width%100);
         snprintf(LCD_Row_2, 17, "Cut.StepZ %1d.%02dmm", Cutting_Width/100, Cutting_Width%100);
      }
   }
   else if (Mode == Mode_Divider)  /////////////////////////////////////////////////////////////
   { 
      long Spindle_Angle = Enc_Pos * 36000 / ENC_TICK;
      long Required_Angle = 36000 * (Current_Tooth - 1) / Total_Tooth;
      snprintf(LCD_Row_1, 17, "Req:%3ld.%02ld z:%3d", Required_Angle/100, Required_Angle%100, Total_Tooth);
      snprintf(LCD_Row_2, 17, "Rea:%3ld.%02ld a:%3d", Spindle_Angle/100, Spindle_Angle%100, Current_Tooth);
   }

   // Цикл Завершен
   if (flag_end_operation == true) {
     snprintf(LCD_Row_2, 17, "Процеса завърши ");
   }
   // Печать ошибки
   else if (err_0_flag)  // джойстик не в нейтрали
   {
     snprintf(LCD_Row_1, 17, "  ATENTION !!!  ");
     snprintf(LCD_Row_2, 17, "JOY not in N-pos");
   } else if (err_1_flag) {
     snprintf(LCD_Row_1, 17, "Operation Banned");
     snprintf(LCD_Row_2, 17, "Limits not Set  ");
   } else if (err_2_flag) {
     snprintf(LCD_Row_1, 17, "Operation Banned");
     snprintf(LCD_Row_2, 17, "Move to Init Pos");
   } else if (err_3_flag) {
     snprintf(LCD_Row_1, 17, "Operation Banned");
     snprintf(LCD_Row_2, 17, "High Spindle RPM");
   }
      
   lcd.setCursor(0, 0);
   lcd.print(LCD_Row_1);
   lcd.print("   ");

   lcd.setCursor(0, 1);
   lcd.print(LCD_Row_2);
   lcd.print("   ");
}
