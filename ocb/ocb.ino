// ****************************************************************************************
// @name     OCB - Onboard Cumputer Beetle
// @author   Jackson Alessandro dos Santos
// @date     2023/10/02
// ****************************************************************************************
#include <Nextion.h>                                                  // Biblioteca Monitor Nextion 
#include "ocb.h"                                                      // Biblioteca de componentes das telas - atualizado via excel de mapeamento
#include <Wire.h>                                                     //
#include <DS3231.h>                                                   // Biblioteca Relogio Tempo Real Controler - RTC DS3231
#include <SI4703.h>                                                   // Biblioteca Radio Si4703
#include <RDSParser.h>                                                // Controlar o conteudo do RDS
#include <EEPROM.h>                                                   // To save configuration parameters such as channel and volume./*#include <OneWire.h>                                                // Biblioteca para controle dos Sensores de Temperatura
#include <DFRobotDFPlayerMini.h>                                      // Biblioteca da Leitora de CardSD e USB
#include <RotaryEncoderKY040.h>                                       // Biblioteca de Controle do Encoder KY-040
#include <RotaryEncoder.h>                                            // Biblioteca de Controle do Encoder
#include <DallasTemperature.h>                                        // Biblioteca Sensores Temperatura DS18B20 - Interna e Externa
#include <PT2314.h>                                                   // Biblioteca Processador de Audio  

#define DEBUG                                                         // Habilita o monitoramento Serial do Projeto
#define PT2314_SERIAL_MONITOR                                         // Habilita o monitoramento Serial do Processador de Audio

// Cria as listas de objetos das telas para o Loop
NexTouch *nex_listen_list_0[] = {                                     // Tela 0 - Abertura
  &O_0_m0,
  NULL
};

NexTouch *nex_listen_list_1[] = {                                     // Tela 1 - Menu
  &O_1_tx_dia,
  &O_1_tx_mes,
  &O_1_tx_ano,
  &O_1_tx_diasemana,
  &O_1_tx_hora,
  &O_1_tx_minuto,
  &O_1_tx_interna,
  &O_1_tx_int_min,
  &O_1_tx_int_max,
  &O_1_tx_externa,
  &O_1_tx_ext_min,
  &O_1_tx_ext_max,
  &O_1_bt_radio,
  &O_1_bt_sd,
  &O_1_bt_usb,
  &O_1_bt_auxiliar,
  NULL
};

NexTouch *nex_listen_list_5[] = {                                     // Tela 5 - Radio
  &O_5_bt_home,
  &O_5_bt_setup,
  &O_5_bt_next,
  &O_5_bt_prev,
  &O_5_bt_up,
  &O_5_bt_down,
  &O_5_bt_mute,
  &O_5_bt_p1,
  &O_5_bt_p2,
  &O_5_bt_p3,
  &O_5_bt_p4,
  &O_5_bt_p5,
  &O_5_bt_p6,
  &O_5_bt_play,
  &O_5_bt_stop,
  NULL
};

NexTouch *nex_listen_list_6[] = {                                     // Tela 6 - SDCard
  &O_6_bt_home,
  NULL
};

// Pinagem do Arduino Mega 2560
#define                   pinoRSTRadio        2                       // 2              - PWM           - Reset do Radio
#define                   pinoTempInt         8                       // 8              - PWM           - Entrada Temperatura Interna
#define                   pinoTempExt         9                       // 9              - PWM           - Entrada Temperatura Externa
#define                   pinoEncoderVolDT    18                      // 18             - Communication - Encoder Volume DT  - DATA - TX1
#define                   pinoEncoderVolCLK   19                      // 19             - Communication - Encoder Volume CLK - CLOCK - RX1
#define                   pinoDFPlayerTX      14                      // 14             - Communication - DFPlayer TRX3
#define                   pinoDFPlayerRX      15                      // 15             - Communication - DFPlayer RX3
#define                   pinoNextionTX       16                      // 16             - Communication - Nextion TX2
#define                   pinoNextionRX       17                      // 17             - Communication - Nextion RX2
#define                   pinoSDA             20                      // 20             - Communication - SDA/SDIO - I2C
#define                   pinoSCL             21                      // 21             - Communication - SCL/SCLK - I2C
#define                   pinoSDA1            ?                       // Pino Extra     - Communication - SDA1/SDIO1 - I2C - RTC
#define                   pinoSCL1            ?                       // Pino Extra     - Communication - SCL1/SCLK1 - I2C - RTC
#define                   pinoFrqUp           A2                      // A2             - Analogica     - Encoder Frequencia - Pino 1 - UP
#define                   pinoFrqDw           A3                      // A3             - Analogica     - Encoder Frequencia - Pino 2 - Down
#define                   pinoTensao          A4                      // A4             - Analogica     - Entrada Tensao da Bateria
#define                   pinoEncoderVolSW    A6                      // A6             - Analogica     - Encoder Volume SW  - SWITCH

#define                   ONE_WIRE_BUS_1      pinoTempInt
#define                   ONE_WIRE_BUS_2      pinoTempExt

#define                   FPSerial            Serial3                 // 14,15          - Communication - DFPlayerMini SDCard
#define                   nexSerial           Serial2                 // 16,17          - Communication - Nextion - Monitor

// Instancia bibliotecas dos componentes (Analogicos)
RotaryEncoderKY040        rotary_vol(&RotaryChangedVol, pinoEncoderVolDT, pinoEncoderVolCLK, pinoEncoderVolSW);          // Pins 2 (DT), 3 (CLK), 4 (SW)

// Instancia bibliotecas dos componentes (TX/RX)
OneWire                   oneWire_in(ONE_WIRE_BUS_1);                 // Setup a oneWire instance to communicate with any OneWire devices
OneWire                   oneWire_ex(ONE_WIRE_BUS_2);
DallasTemperature         sensor_int(&oneWire_in);                    // Pass our oneWire reference to Dallas Temperature.
DallasTemperature         sensor_ext(&oneWire_ex);

RotaryEncoder             *encoder            = nullptr;              // Configura o RoraryEncoder de Frequencia
DFRobotDFPlayerMini       myDFPlayer;                                 // Instancia o DFPlayer

// Assign the addresses of your 1-Wire.
DeviceAddress             InThermo            = { 0x28, 0xFF, 0x77, 0x01, 0x83, 0x16, 0x05, 0x04 };         // See the tutorial on how to obtain these addresses: http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html
DeviceAddress             OutThermo           = { 0x28, 0xFF, 0x02, 0xE0, 0xA0, 0x16, 0x04, 0x53 };

// Conections in I2C (SDA/SCL)
PT2314                    pt2314;                                     // Processador de Audio - Midias    - PT2314_ADDRESS 0x44
SI4703                    radio;                                      // Radio                            - SI4703_ADDRESS 0x10
DS3231                    rtc;                                        // RTC                              - DS3231_ADDRESS 0x68
                                                                      // RTC  I²C eeprom (AT24C32)        -                0X57

RDSParser                 rds;                                        // Objeto de controle do Radio - Informacoes RDS
AUDIO_INFO                audioInfo;                                  // Objeto de controle do Radio - Informacoes Audio
RADIO_INFO                radioInfo;                                  // Objeto de controle do Radio - Informacoes Radio

bool                      century             = false;                // RTC - define o controle de século
bool                      h12Flag             = false;                // RTC - define a apresentação da hora em 24hr
bool                      pmFlag              = false;                // RTC - define se mostra ou não o turno am/pm
bool                      _ficaLoop           = true;                 // Radio - Laço do Radio
int                       active_page         = 1;                    // Radio - Define a Pagina ativa no monitor nextion 0..6
int                       _counter_vol        = 0;                    // Radio - Contador da posição do Volume
int                       _lastCount_vol      = 0;                    // Radio - Rotary test de estado do Volume
int                       CounterFrq          = 0;                    // Radio - Contador da posição da Frequencia
int                       LastCountFrq        = 0;                    // Radio - Rotary test de estado da Frequencia
char                      buffer[100]         = {0};                  // Nextion - Buffer para apresentar valores na tela
int                       temperatura_in      = 0;                    // Temp.   - Temperatura Interna
int                       temperatura_in_min  = 99;                   // Temp.   - Temperatura Interna Minima
int                       temperatura_in_max  = 0;                    // Temp.   - Temperatura Interna Maxima
int                       temperatura_ex      = 0;                    // Temp.   - Temperatura Externa
int                       temperatura_ex_min  = 99;                   // Temp.   - Temperatura Externa Minima
int                       temperatura_ex_max  = 0;                    // Temp.   - Temperatura Externa Maxima

void setup() {
  
  pinMode(pinoTensao, INPUT);
  pinMode(pinoRSTRadio, INPUT);
  pinMode(pinoEncoderVolDT, INPUT);
  pinMode(pinoEncoderVolCLK, INPUT);
  pinMode(pinoEncoderVolSW, INPUT_PULLUP);
  pinMode(pinoFrqUp, INPUT);
  pinMode(pinoFrqDw, INPUT);
  pinMode(pinoTempInt, INPUT);
  pinMode(pinoTempExt, INPUT);
   
  Serial.begin(9600);
  FPSerial.begin(9600);

  if (!nexInit())                                                     // Inicializa o Display
  {
    #ifdef DEBUG
      Serial.println(F("Erro na Inicialização do Monitor Nextion......"));
    #endif
  }

  // tela 0-abertura
  //O_0_m0.attachPop(O_0_m0PopCallback, &O_0_m0);
        
  // tela 1-menu
  /*O_1_tx_dia.attachPop(O_1_tx_diaPopCallback);
  O_1_tx_mes.attachPop(O_1_tx_mesPopCallback);
  O_1_tx_ano.attachPop(O_1_tx_anoPopCallback);
  O_1_tx_diasemana.attachPop(O_1_tx_diasemanaPopCallback);
  O_1_tx_hora.attachPop(O_1_tx_horaPopCallback);
  O_1_tx_minuto.attachPop(O_1_tx_minutoPopCallback);
  O_1_tx_interna.attachPop(O_1_tx_internaPopCallback);
  O_1_tx_int_min.attachPop(O_1_tx_int_minPopCallback);
  O_1_tx_int_max.attachPop(O_1_tx_int_maxPopCallback);
  O_1_tx_externa.attachPop(O_1_tx_externaPopCallback);
  O_1_tx_ext_min.attachPop(O_1_tx_ext_minPopCallback);
  O_1_tx_ext_max.attachPop(O_1_tx_ext_maxPopCallback);*/
  O_1_bt_radio.attachPop(O_1_bt_radioPopCallback);
  O_1_bt_sd.attachPop(O_1_bt_sdPopCallback);
 // O_1_bt_usb.attachPop(O_1_bt_usbPopCallback);
 // O_1_bt_auxiliar.attachPop(O_1_bt_auxiliarPopCallback);

  // tela 5-Radio
  O_5_bt_home.attachPop( O_5_bt_homePopCallback); 
  O_5_bt_p1.attachPop( O_5_bt_p1PopCallback);
  O_5_bt_p2.attachPop( O_5_bt_p2PopCallback);
  O_5_bt_p3.attachPop( O_5_bt_p3PopCallback);
  O_5_bt_p4.attachPop( O_5_bt_p4PopCallback);
  O_5_bt_p5.attachPop( O_5_bt_p5PopCallback);
  O_5_bt_p6.attachPop( O_5_bt_p6PopCallback);
  O_5_bt_mute.attachPop( O_5_bt_mutePopCallback);
  /*O_5_bt_setup.attachPop( O_5_bt_setupPopCallback);
  O_5_bt_next.attachPop( O_5_bt_nextPopCallback);
  O_5_bt_prev.attachPop( O_5_bt_prevPopCallback); 
  O_5_bt_up.attachPop( O_5_bt_upPopCallback);
  O_5_bt_down.attachPop( O_5_bt_downPopCallback);
  O_5_bt_play.attachPop( O_5_bt_playPopCallback);
  O_5_bt_stop.attachPop( O_5_bt_stopPopCallback);*/
  
  // tela 6-SDCard
  O_6_bt_home.attachPop( O_6_bt_homePopCallback);

  Wire.begin();                                                       // Inicializa o RTC
  delay(100);
  if (Wire.available() > 2) {
    #ifdef DEBUG
      Serial.println(F("Wire - RTC com mal funcionamento ..."));
    #endif
  }
 
  sensor_int.begin();                                                 // Inicializa o Sensor Temperatura Interna
  sensor_int.setResolution(InThermo, 10);
  if (!sensor_int.isConnected(InThermo)) {
    #ifdef DEBUG
      Serial.println(F("DallasTemperature - Sensor Temperatura Interno com mal funcionamento ..."));
    #endif
  }
  
  sensor_ext.begin();                                                 // Inicializa o Sensor Temperatura Externa
  sensor_ext.setResolution(OutThermo, 10);
  if (!sensor_ext.isConnected(OutThermo)) {
    #ifdef DEBUG
      Serial.println(F("DallasTemperature - Sensor Temperatura Externo com mal funcionamento ..."));
    #endif
  }

  pt2314.begin();                                                     // Inicializa o Processdor de Audio - Midias 0 - Radio
  if (!pt2314.isConnected()) {
    #ifdef DEBUG
      Serial.println(F("PT2314 - Processador de Audio com mal funcionamento ..."));
    #endif
  }

  if(!setup_radio()) {                                                // Inicializa o Radio - SI4703
    #ifdef DEBUG
      Serial.println(F("SI4703 - Radio com mal funcionamento ..."));
    #endif
  }

  if(!setup_midi()) {                                                 // Inicializa o DFPlayer - Midi´s
    #ifdef DEBUG
      Serial.println(F("DFPlayer Mini - SDCard com mal funcionamento ..."));
    #endif
  }

  rotary_vol.setup();                                                 // Inicaliza o Botão de Volume/Mute
  encoder                   = new RotaryEncoder(pinoFrqUp, pinoFrqDw, RotaryEncoder::LatchMode::TWO03); // Inicia o RoraryEncoder de Frequencia

  #ifdef DEBUG
    Serial.println(F("setup done"));
    i2c_scanner();
  #endif

  delay(2000);

//  O_0_m0PopCallback(0);                                               // Após acabar o Setup chama a tela de menu

  // Temporario
  active_page = 5;
  //O_6_midi.show();
  //executaSDCard();
  O_5_radio.show();
  executaRadio();
}

void loop() {

  switch (active_page) {
    case 0:
      nexLoop(nex_listen_list_0);
      break;
    case 1:
      nexLoop(nex_listen_list_1);
      atualizaRelogio();
      atualizaTemperatura();
      atualizaTensao();
      break;
//    case 2:
//      nexLoop(nex_listen_list_2);
//      break;
//    case 3:
//      nexLoop(nex_listen_list_3);
//      break;
//    case 4:
//      nexLoop(nex_listen_list_4);
//      break;
    case 5:
      nexLoop(nex_listen_list_5);
      break;
    case 6:
      nexLoop(nex_listen_list_6);
      break;
    default:
      active_page = 0;
      O_0_abertura.show();
      nexLoop(nex_listen_list_0);
      break;
  }
  delay(1000);
}