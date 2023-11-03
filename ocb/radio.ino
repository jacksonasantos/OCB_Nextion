// ****************************************************************************************
// @name     OCB - Onboard Cumputer Beetle
// @subject  Funções Tela 5 - radio
// @author   Jackson Alessandro dos Santos
// @date     2023/10/05
// ***************************************************************************************
#define                   FIX_BAND          RADIO_BAND_FM             // The band that will be tuned by this sketch is FM.
#define                   FIX_STATION       9830                      // The station that will be tuned by this sketch is 98.30 MHz.
#define                   FIX_VOLUME        7                         // The volume that will be set by this sketch is level 4.

#define                   encoderMin        radio.getMinFrequency()
#define                   encoderMax        radio.getMaxFrequency()
#define                   encoderStep       radio.getFrequencyStep()

char                      BAND[4][9]        = {"FM", "FM Mundo", "AM", "KW"};

int                       _volume           = FIX_VOLUME;
int                       _frequencia       = FIX_STATION;
int                       lastPosFreq       = _frequencia;
               
String                    lastServiceName;                            // Variaveis para controle do RDS do Radio SI4703
unsigned long             startSeek; 
uint8_t                   g_RDS_hour;
uint8_t                   g_RDS_minute;
char                      *g_RDS_name;
char                      *g_RDS_txt;
RADIO_INFO                ri;
// ****************************************************************************************
// @name     @Botoes Nextion
// @subject  Configura ações para os objetos com Touch no monitor Nextion
// ****************************************************************************************
void O_5_bt_homePopCallback(void *ptr) {                              // Botão HOME
  #ifdef DEBUG
    Serial.print (F("Retornando ao Menu PAGE 1 ativada....... "));
  #endif
  active_page = 1;
  O_1_menu.show();
  _ficaLoop = false;
}
void O_5_tx_bandaPopCallback(void *ptr) {                             // Mostra dados da Banda de Transmição (FM, FM Mundo, .. )
  O_5_tx_banda.setText(ptr); 
}
void O_5_tx_frequenciaPopCallback(void *ptr) {                        // Mostra o Texto da Frequencia selecionada
  char s[12];
  radio.formatFrequency(s, sizeof(s));
  O_5_tx_frequencia.setText(s); 
}
void O_5_tx_estacaoPopCallback(void *ptr) {                           // Mostra o Texto da Estação - Nome da Estação - RDS
  O_5_tx_estacao.setText(g_RDS_name); 
}
void O_5_tx_programacaoPopCallback(void *ptr) {                       // Mostra o Texto da Programação - Programação da Estação - RDS
  O_5_tx_programacao.setText(g_RDS_txt); 
}
/*
void O_5_bt_setupPopCallback(void *ptr) { }
void O_5_bt_nextPopCallback(void *ptr) { }
void O_5_bt_prevPopCallback(void *ptr) { }
void O_5_bt_upPopCallback(void *ptr) { }
void O_5_bt_downPopCallback(void *ptr) { }
void O_5_bt_playPopCallback(void *ptr) { }
void O_5_bt_stopPopCallback(void *ptr) { }
*/
void O_5_bt_mutePopCallback() {                                       // Configura o botão de Mute
  radio.setMute(!radio.getMute());
  O_5_bt_mute.setValue(radio.getMute()?1:0);
}
void O_5_bt_p1PopCallback(void *ptr) {                                // Configura a leitura do botão de programação 1
  memset(buffer, 0, sizeof(buffer));
  O_5_bt_p1.getText(buffer, sizeof(buffer));
  float number = atof(buffer);
  number = number*100;
  FrequencyChange(number);

  #ifdef DEBUG
    Serial.print (F("Frequencia B1: "));
    Serial.print(buffer);
    Serial.print(" - ");
    Serial.println(number);
  #endif
}
void O_5_bt_p2PopCallback(void *ptr) {                                // Configura a leitura do botão de programação 2 
  memset(buffer, 0, sizeof(buffer));
  O_5_bt_p2.getText(buffer, sizeof(buffer));
  float number = atof(buffer);
  number = number*100;
  FrequencyChange(number);

  #ifdef DEBUG
    Serial.print (F("Frequencia B2: "));
    Serial.print(buffer);
    Serial.print(" - ");
    Serial.println(number);
  #endif
}
void O_5_bt_p3PopCallback(void *ptr) {                                // Configura a leitura do botão de programação 3 
  memset(buffer, 0, sizeof(buffer));
  O_5_bt_p3.getText(buffer, sizeof(buffer));
  float number = atof(buffer);
  number = number*100;
  FrequencyChange(number);

  #ifdef DEBUG
    Serial.print (F("Frequencia B3: "));
    Serial.print(buffer);
    Serial.print(" - ");
    Serial.println(number);
  #endif
}
void O_5_bt_p4PopCallback(void *ptr) {                                // Configura a leitura do botão de programação 4 
  memset(buffer, 0, sizeof(buffer));
  O_5_bt_p4.getText(buffer, sizeof(buffer));
  float number = atof(buffer);
  number = number*100;
  FrequencyChange(number);

  #ifdef DEBUG
    Serial.print (F("Frequencia B4: "));
    Serial.print(buffer);
    Serial.print(" - ");
    Serial.println(number);
  #endif
}
void O_5_bt_p5PopCallback(void *ptr) {                                // Configura a leitura do botão de programação 5 
  memset(buffer, 0, sizeof(buffer));
  O_5_bt_p5.getText(buffer, sizeof(buffer));
  float number = atof(buffer);
  number = number*100;
  FrequencyChange(number);

  #ifdef DEBUG
    Serial.print (F("Frequencia B5: "));
    Serial.print(buffer);
    Serial.print(" - ");
    Serial.println(number);
  #endif
}
void O_5_bt_p6PopCallback(void *ptr) {                                // Configura a leitura do botão de programação 6 
  memset(buffer, 0, sizeof(buffer));
  O_5_bt_p6.getText(buffer, sizeof(buffer));
  float number = atof(buffer);
  number = number*100;
  FrequencyChange(number);

  #ifdef DEBUG
    Serial.print (F("Frequencia B6: "));
    Serial.print(buffer);
    Serial.print(" - ");
    Serial.println(number);
  #endif
}

// ****************************************************************************************
void RDS_process(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4) {
  rds.processData(block1, block2, block3, block4);
}
// ****************************************************************************************
// @name     setup_radio
// @subject  Configura o Radio e o Processador de Audio para o canal 0
// ****************************************************************************************
bool setup_radio() {
  bool _ok = true;

  // TODO - ver Processador de Audio
  if ( pt2314.isConnected() ) {
                                         
    pt2314.setVolume(30);
    pt2314.setAttn(10, 10);  
    pt2314.setLoudness(true);
    pt2314.setMute(false);
    pt2314.setGain(0);

    #ifdef DEBUG
      Serial.println(F("Config. Processador Audio - PT2314"));
      Serial.print(F("   Channel : "));  Serial.println(pt2314.getChannel());
      Serial.print(F("   Volume  : "));  Serial.println(pt2314.getVolume());
      Serial.print(F("   Attn    : "));  Serial.print(pt2314.getAttnLeft());  Serial.print(F(" - "));  Serial.println(pt2314.getAttnRight());
      Serial.print(F("   Loudness: "));  Serial.println(pt2314.getLoudness());
      Serial.print(F("   Bass    : "));  Serial.println(pt2314.getBass());
      Serial.print(F("   Treble  : "));  Serial.println(pt2314.getTreble());
      Serial.print(F("   Mute    : "));  Serial.println(pt2314.getMute());
      Serial.print(F("   Gain    : "));  Serial.println(pt2314.getGain());
    #endif
  }
  radio.setup(RADIO_RESETPIN, pinoRSTRadio);
  radio.setup(RADIO_SDAPIN, pinoSDA);
  radio.setup(RADIO_FMSPACING, RADIO_FMSPACING_200);                  // Set FM Options for USA
  radio.setup(RADIO_DEEMPHASIS, RADIO_DEEMPHASIS_50);
  
  radio.initWire(Wire);                                               // Initialize the Radio

  radio.setBandFrequency(FIX_BAND, FIX_STATION);                      // Set all radio setting to the fixed values.
  radio.setMono(false);
  radio.setMute(true);
  radio.setSoftMute(false);

  radio.attachReceiveRDS(RDS_process);                                // setup the information chain for RDS data.
  rds.attachServiceNameCallback(DisplayServiceName);
  rds.attachTextCallback(DisplayText);
  rds.attachTimeCallback(DisplayTime);

  #ifdef DEBUG
    radio.debugEnable(true);                                          // Enable information to the Serial port
    radio._wireDebug(true);
    
    Serial.println(F("Config. Radio - SI4703"));
    Serial.print(F("   Banda      : "));    Serial.println(BAND[radio.getBand()-1]);
    Serial.print(F("   Frequencia : "));    Serial.println(radio.getFrequency());
    Serial.print(F("   Faixa  Min : "));    Serial.print(radio.getMinFrequency()); 
    Serial.print(F(" Max : "));             Serial.print(radio.getMaxFrequency());
    Serial.print(F(" Step : "));            Serial.println(radio.getFrequencyStep());
    Serial.print(F("   Mono/Stereo: "));    Serial.println(radio.getMono()); 
    Serial.print(F("   Volume     : "));    Serial.println(radio.getVolume());
    Serial.print(F("   BassBoost  : "));    Serial.println(radio.getBassBoost());
    Serial.print(F("   Mute       : "));    Serial.println(radio.getMute());
    Serial.print(F("   SoftMute   : "));    Serial.println(radio.getSoftMute());
  #endif

  return _ok;
}
// ****************************************************************************************
// @name     executaRadio
// @subject  Executa o Radio - com Loop infinito
// ****************************************************************************************
void executaRadio() { 
  
  #ifdef DEBUG
    Serial.println(F("Radio Ativado"));
  #endif

  pt2314.setChannel(0); 
  
  active_page = 5;
  _ficaLoop   = true;
  O_5_radio.show();
  radio.setVolume(_volume);
  radio.setMute(false);
  char s[12];
  radio.formatFrequency(s, sizeof(s));

  static unsigned long _now          = millis();
  static unsigned long _nextFreqTime = 1;

  encoder->setPosition(_frequencia / encoderStep);
  lastPosFreq       = encoder->getPosition() * encoderStep;           // Rotary Frequencia - teste de estado
  _counter_vol      = _volume;                                        // Rotary Volume - Controle
  _lastCount_vol    = 0;                                              // Rotary Volume - teste de estado

  #ifdef DEBUG
    Serial.println(F("Config. Radio - SI4703"));
    Serial.print(F("   Banda      : "));    Serial.println(BAND[radio.getBand()-1]);
    Serial.print(F("   Frequencia : "));    Serial.println(s);
    Serial.print(F("   Volume     : "));    Serial.println(radio.getVolume());    
    Serial.print(F("   Mono/Stereo: "));    Serial.println(radio.getMono()); 
    Serial.print(F("   BassBoost  : "));    Serial.println(radio.getBassBoost());
    Serial.print(F("   Mute       : "));    Serial.println(radio.getMute());
    Serial.print(F("   SoftMute   : "));    Serial.println(radio.getSoftMute());
    Serial.print(F("   Radio      :" ));    radio.debugRadioInfo();
    Serial.print(F("   Audio      :" ));    radio.debugAudioInfo();
  #endif

  // Atualiza objetos da tela com a execução do rádio
  O_5_tx_bandaPopCallback(BAND[radio.getBand()-1]);
  O_5_tx_estacaoPopCallback(*g_RDS_name);
  O_5_tx_programacaoPopCallback(*g_RDS_txt);
  O_5_tx_frequenciaPopCallback(s);
  O_5_sl_freq.setValue(lastPosFreq/10);
  O_5_pb_volume.setValue((int)(radio.getVolume()*6.6667));
  O_5_bt_mute.setValue(radio.getMute()?1:0);

  while(_ficaLoop) {

    _now = millis();
    
    nexLoop(nex_listen_list_5);                                       // Testa objetos do monitor
    
    encoder->tick();
    int newPosFreq = (encoder->getPosition() * encoderStep)+(encoderStep==20?10:0);

    if (lastPosFreq != newPosFreq) {                                  // Atualiza a Frequencia da Estação 
      #ifdef DEBUG
        Serial.print("Encoder Frequencia: ");
        Serial.println(newPosFreq); 
      #endif
      FrequencyChange(newPosFreq);
    }
    
    if (_lastCount_vol != _counter_vol) {                             // Atualiza o controle do Volume
      radio.setVolume(_counter_vol);
      O_5_pb_volume.setValue((int)(_counter_vol*6.6667));
      _lastCount_vol = _counter_vol;    
    }
       
    if (rotary_vol.GetButtonDown()) {                                 // Controle do Botao MUTE
      O_5_bt_mutePopCallback();
    }

    if (_now > _nextFreqTime ) {                                      // Atualiza tela do radio    
      radio.formatFrequency(s, sizeof(s));

      radio.checkRDS();
      radio.getRadioInfo(&radioInfo);
      radio.getAudioInfo(&audioInfo);

      mostraRDS();

      #ifdef DEBUG
        Serial.println(F("Execução Radio - SI4703"));
        Serial.print(F("   Banda      : "));    Serial.print(BAND[radio.getBand()-1]);
        Serial.print(F("   Frequencia : "));    Serial.print(s);
        Serial.print(F("   Encoder    : "));    Serial.println(encoder->getPosition());
        Serial.print(F("   Volume     : "));    Serial.println(radio.getVolume());    
        Serial.print(F("   Radio      : "));    radio.debugRadioInfo();
        Serial.print(F("   Audio      : "));    radio.debugAudioInfo();
      #endif    
      
     _nextFreqTime = _now + 1000;
    } 
  }
}
// ****************************************************************************************
// @name     FrequencyChanged
// @subject  Muda a Frequencia do Radio e atualiza os componentes vinculados (Tela, Rotary, Chip)
// ****************************************************************************************
void FrequencyChange(int newFrequency) {
  
  #ifdef DEBUG
    Serial.print("Nova Frequencia: ");
    Serial.println(newFrequency); 
  #endif
  
  if (newFrequency < encoderMin) {                                    // Verifica os limites das estações
    encoder->setPosition(encoderMin / encoderStep);
    newFrequency = encoderMin;
  } else if (newFrequency > encoderMax) {
    encoder->setPosition(encoderMax / encoderStep);
    newFrequency = encoderMax;
  }

  radio.setFrequency(newFrequency);                                   // Atualiza o Radio
  char s[12];
  radio.formatFrequency(s, sizeof(s));
  
  O_5_sl_freq.setValue(newFrequency/10);                              // Atualiza a Tela
  O_5_tx_frequenciaPopCallback(s);
  
  // TODO - Ajustar o controle do encoderStep pois não pula fracionado
  encoder->setPosition(newFrequency / encoderStep);                   // Atualiza o Encoder
  lastPosFreq       = newFrequency;                                   // Rotary Frequencia - teste de estado    
}
// ****************************************************************************************
// @name     RotaryChangedVol
// @subject  Analiza a mudança de estado do Rotary KY-040
// ****************************************************************************************
void RotaryChangedVol() {
  const unsigned int state = rotary_vol.GetState();
  
  if (state & DIR_CW) { 
    _counter_vol++;
    if (_counter_vol>radio.getMaxVolume()) {_counter_vol=radio.getMaxVolume();}
  }
  if (state & DIR_CCW) {
    _counter_vol--;    
    if (_counter_vol<0) {_counter_vol=0;}
  }
}
// ****************************************************************************************
// @name     mostraRDS
// @subject  Analiza os dados RDS do Radio
// ****************************************************************************************
void mostraRDS() {
  if (ri.rssi >= 32) {                                                // si4703 usable threshold value
    if (ri.rds) {
      radio.checkRDS();
      startSeek = millis();
      do {
        radio.checkRDS();
      } while (lastServiceName.length()<=0 && (startSeek + 600 > millis()));
    } 
  }
  #ifdef DEBUG
    Serial.print(F("   RDS        : "));
    Serial.print(F(" Name: "));           Serial.print(g_RDS_name);
    Serial.print(F(" Text: "));           Serial.print(g_RDS_txt);
    Serial.print(F(" Time: "));
    if (g_RDS_hour < 10) Serial.print('0'); 
    Serial.print(g_RDS_hour); 
    Serial.print(":"); 
    if (g_RDS_minute < 10) Serial.print('0');  
    Serial.println(g_RDS_minute);
  #endif
  O_5_tx_estacaoPopCallback(*g_RDS_name);
  O_5_tx_programacaoPopCallback(*g_RDS_txt);
}
void DisplayTime(uint8_t hour, uint8_t minute) {                      // Update the Time when in RDS-Time.
  g_RDS_hour = hour;
  g_RDS_minute = minute;
}
void DisplayServiceName(const char *name) {
  bool found = false;
  for (uint8_t n = 0; n < 8; n++)
    if (name[n] != ' ')
      found = true;

  if (found) {
    lastServiceName = name;
    g_RDS_name = name;
  }
}
void DisplayText(const char *txt) {                                   // Update the ServiceName text on the LCD display.
  g_RDS_txt = txt;
}