  
void Limit_Led_Dance() {  
  uint8_t i = 0;
  while(i++ < 7) {
    Limit_Left_LED_On();    Limit_Rear_LED_Off();    _delay_ms(100);
    Limit_Front_LED_On();   Limit_Left_LED_Off();    _delay_ms(100);
    Limit_Right_LED_On();   Limit_Front_LED_Off();   _delay_ms(100);  
    Limit_Rear_LED_On();    Limit_Right_LED_Off();   _delay_ms(100);
  }
  Limit_Rear_LED_Off();
}
