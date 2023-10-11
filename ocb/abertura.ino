// ****************************************************************************************
// @name     OCB - Onboard Cumputer Beetle
// @subject  Funções Tela 0 - abertura
// @author   Jackson Alessandro dos Santos
// @date     2023/10/02
// ****************************************************************************************
void O_0_m0PopCallback(void *ptr) { 
  #ifdef DEBUG
      Serial.print(F("Chamando o Menu....... "));
  #endif

  active_page = 1;
  O_1_menu.show();
  
  #ifdef DEBUG
      Serial.println(F("ok, PAGE 1 ativada......."));
  #endif
}