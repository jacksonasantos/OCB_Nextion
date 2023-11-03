// ****************************************************************************************
// @name     OCB - Onboard Cumputer Beetle
// @subject  Funções Tela 1 - menu
// @author   Jackson Alessandro dos Santos
// @date     2023/10/02
// ****************************************************************************************
void O_1_tx_diaPopCallback(void *ptr)       {  O_1_tx_dia.setText(ptr); }
void O_1_tx_mesPopCallback(void *ptr)       {  O_1_tx_mes.setText(ptr); }
void O_1_tx_anoPopCallback(void *ptr)       {  O_1_tx_ano.setText(ptr); }
void O_1_tx_diasemanaPopCallback(void *ptr) {  O_1_tx_diasemana.setText(ptr); }
void O_1_tx_horaPopCallback(void *ptr)      {  O_1_tx_hora.setText(ptr); }
void O_1_tx_minutoPopCallback(void *ptr)    {  O_1_tx_minuto.setText(ptr); }

void O_1_tx_internaPopCallback(void *ptr)   {  O_1_tx_interna.setText(ptr); }
void O_1_tx_int_minPopCallback(void *ptr)   {  O_1_tx_int_min.setText(ptr); }
void O_1_tx_int_maxPopCallback(void *ptr)   {  O_1_tx_int_max.setText(ptr); }
void O_1_tx_externaPopCallback(void *ptr)   {  O_1_tx_externa.setText(ptr); }
void O_1_tx_ext_minPopCallback(void *ptr)   {  O_1_tx_ext_min.setText(ptr); }
void O_1_tx_ext_maxPopCallback(void *ptr)   {  O_1_tx_ext_max.setText(ptr); }

// ****************************************************************************************
void O_1_bt_radioPopCallback(void *ptr)    { 
  #ifdef DEBUG
      Serial.print(F("Chamando o POP Radio....... "));
  #endif

  executaRadio();

  #ifdef DEBUG
      Serial.println(F("ok, PAGE 5 ativada......."));
  #endif
}

// ****************************************************************************************

void O_1_bt_sdPopCallback(void *ptr) { 
  #ifdef DEBUG
      Serial.print(F("Chamando o POP SD....... "));
  #endif

  executaSDCard();

  #ifdef DEBUG
      Serial.println(F("ok, PAGE 6-SD ativada......."));
  #endif
}
/*
// ****************************************************************************************
void O_1_bt_usbPopCallback(void *ptr) { 
  #ifdef DEBUG
      Serial.print(F("Chamando o POP USB....... "));
  #endif

  active_page = 6;
  O_6_midi.show();
  executaUSB();

  #ifdef DEBUG
      Serial.println(F("ok, PAGE 6-SD ativada......."));
  #endif
}

// ****************************************************************************************
void O_1_bt_auxiliarPopCallback(void *ptr) { 
  #ifdef DEBUG
      Serial.print(F("Chamando o POP Auxiliar....... "));
  #endif

  active_page = 6;
  O_6_midi.show();
  executaAuxiliar();

  #ifdef DEBUG
      Serial.println(F("ok, PAGE 6-Auxiliar ativada......."));
  #endif
}
*/
// ****************************************************************************************
void atualizaTensao()                                                                           // Leitura do valor de Tensão da Bateria
{
  float                     valorTensaoDC;                                                      // variavel para receber o valor lido do sensor
  int                       amostragem      = 100;                                              // variavel para der uma media de calculos
  float                     mediaTensaoDC   = 0;                                                // variavel para soma das medias
  float                     finalTensaoDC   = 0;                                                // variavel para recerber o valor final de Volts
                                                                                                // VARIAVEIS COM OS VALORES DOS RESISTORES DO SENSOR DE TENSÃO DC
  float                     voltsporUnidade = 0.004887586;                                      // DECLARAÇÃO DA VARIÁVEL DA CONSTANTE DO ADC 5%/1023
  float                     R1              = 30400.0;                                          // Medir o resitor com o multimetro na escala 200k de omegas do lado negativo
  float                     R2              = 7720.0;                                           // Medir o resitor com o multimetro na escala 200k de omegas do lado positivo

  float                     passo           = 22.5;
  int                       pointer_pos     = 0;
  int                       pointer_min     = 270;
  int                       pointer_max     = 90;
  float                     voltage_ref     = 12.0;
  float                     voltage_min     = 8.0;
  float                     voltage_max     = 16.0;

  int                       incremento      = 0;

  for (int i = 0; i < amostragem ; i++) {                                                       // repete por n vezes para der uma precisão melhor
    valorTensaoDC = analogRead(pinoTensao);                                                     // recebe o valor do sensor que vai de 0 até 1023
    valorTensaoDC = (valorTensaoDC * voltsporUnidade);                                          // calcula o valor com base na resolução do ADC
    mediaTensaoDC = mediaTensaoDC + (valorTensaoDC / (R2 / (R1 + R2)));                         // faz o calculo matematico
  }

  finalTensaoDC = mediaTensaoDC / amostragem;                                                   // calcula a media dos valores

  if (finalTensaoDC < voltage_ref ) { incremento = 360; }
  pointer_pos       = (int) (((finalTensaoDC- voltage_ref) * passo) + incremento);          // Posição do Ponteiro no Gauge
  if (finalTensaoDC<voltage_min) {pointer_pos = pointer_min;}
  if (finalTensaoDC>voltage_max) {pointer_pos = pointer_max;}

  O_1_ma_bateria.setValue(pointer_pos);                                                         // Atualiza o ponteiro do Gauge no monitor

  #ifdef DEBUG
    Serial.print(F("Tensao: ")); 
    Serial.print(finalTensaoDC);
    Serial.print(F(" V"));
    Serial.print(F(" - Ponteiro: "));
    Serial.println(pointer_pos);
  #endif
}

// ****************************************************************************************
void atualizaTemperatura(void)                                                                  // Leitura dos valores de Temperatura - Sensor de Tenpertura Interno e Externo
{
  sensor_int.requestTemperaturesByIndex(0);
  temperatura_in = (int)sensor_int.getTempCByIndex(0);
  temperatura_in_min = min(temperatura_in,temperatura_in_min);
  temperatura_in_max = max(temperatura_in,temperatura_in_max);
  
  memset(buffer, 0, sizeof(buffer));
  itoa(temperatura_in, buffer, 10); 
  O_1_tx_internaPopCallback(buffer);

  memset(buffer, 0, sizeof(buffer));
  itoa(temperatura_in_min, buffer, 10); 
  O_1_tx_int_minPopCallback(buffer);
  
  memset(buffer, 0, sizeof(buffer));
  itoa(temperatura_in_max, buffer, 10); 
  O_1_tx_int_maxPopCallback(buffer);

  sensor_ext.requestTemperaturesByIndex(0);
  temperatura_ex = (int)sensor_ext.getTempCByIndex(0);
  temperatura_ex_min = min(temperatura_ex,temperatura_ex_min);
  temperatura_ex_max = max(temperatura_ex,temperatura_ex_max);
  
  memset(buffer, 0, sizeof(buffer));
  itoa(temperatura_ex, buffer, 10); 
  O_1_tx_externaPopCallback(buffer);

  memset(buffer, 0, sizeof(buffer));
  itoa(temperatura_ex_min, buffer, 10); 
  O_1_tx_ext_minPopCallback(buffer);
  
  memset(buffer, 0, sizeof(buffer));
  itoa(temperatura_ex_max, buffer, 10); 
  O_1_tx_ext_maxPopCallback(buffer);
  
  #ifdef DEBUG
    Serial.print(F("Temperatura Interna:"));
    Serial.print(F("Atual "));
    Serial.print(temperatura_in);
    Serial.print(F(" Min "));
    Serial.print(temperatura_in_min);
    Serial.print(F(" Max "));
    Serial.println(temperatura_in_max);

    Serial.print(F("Temperatura Externa:"));
    Serial.print(F("Atual "));
    Serial.print(temperatura_ex);
    Serial.print(F(" Min "));
    Serial.print(temperatura_ex_min);
    Serial.print(F(" Max "));
    Serial.println(temperatura_ex_max);
  #endif
}

// ****************************************************************************************
void atualizaRelogio(void)                                                                      // Leitura dos dados do Relógio   
{
  char daysOfTheWeek[7][13] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};
  char months[12][9] = {"Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho", "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"};
 
  #ifdef DEBUG
    Serial.print(F("Relogio: "));
    Serial.print(rtc.getDate(), DEC);
    Serial.print(F("/"));
    Serial.print(months[rtc.getMonth(century)]);
    Serial.print(F("/"));
    Serial.print(rtc.getYear(), DEC);
    Serial.print(F(" - "));
    Serial.print(daysOfTheWeek[rtc.getDoW()]);
    Serial.print(F(" "));
    Serial.print(rtc.getHour(h12Flag, pmFlag), DEC);
    Serial.print(F(":"));
    Serial.println(rtc.getMinute(), DEC);
  #endif
  
  memset(buffer, 0, sizeof(buffer));
  itoa(rtc.getDate(), buffer, 10); 
  O_1_tx_diaPopCallback(buffer);
 
  O_1_tx_mesPopCallback(months[rtc.getMonth(century)]);

  memset(buffer, 0, sizeof(buffer));
  itoa(rtc.getYear()+2000, buffer, 10); 
  O_1_tx_anoPopCallback(buffer); 

  O_1_tx_diasemanaPopCallback(daysOfTheWeek[rtc.getDoW()]);

  memset(buffer, 0, sizeof(buffer));
  itoa(rtc.getHour(h12Flag, pmFlag), buffer, 10);  
  O_1_tx_horaPopCallback(buffer);

  memset(buffer, 0, sizeof(buffer));
  itoa(rtc.getMinute(), buffer, 10);  
  O_1_tx_minutoPopCallback(buffer);
}