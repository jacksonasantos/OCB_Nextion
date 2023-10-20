// ****************************************************************************************
// @name     OCB - Onboard Cumputer Beetle
// @subject  Funções Tela 4 - radio
// @author   Jackson Alessandro dos Santos
// @date     2023/10/05
// ***************************************************************************************
#define                   FIX_BAND        RADIO_BAND_FM             // < The band that will be tuned by this sketch is FM.
#define                   FIX_STATION     9830                      // < The station that will be tuned by this sketch is 89.30 MHz.
#define                   FIX_VOLUME      15                        // < The volume that will be set by this sketch is level 4.

#define                   encoderMin      radio.getMinFrequency()
#define                   encoderMax      radio.getMaxFrequency()
#define                   encoderStep     radio.getFrequencyStep()

char                      BAND[4][9]      = {"FM", "FM Mundo", "AM", "KW"};

int                       _volume         = FIX_VOLUME;
int                       _frequencia     = FIX_STATION;

void O_5_bt_homePopCallback(void *ptr) {
  #ifdef DEBUG
    Serial.print(F("Retornando ao Menu....... "));
  #endif

  active_page = 1;
  O_1_menu.show();
  _ficaLoop = false;

  #ifdef DEBUG
    Serial.println(F("ok, PAGE 1 ativada......."));
  #endif
}

void  O_5_tx_frequenciaPopCallback(void *ptr) { O_5_tx_frequencia.setText(ptr); }
void  O_5_tx_bandaPopCallback(void *ptr)      { O_5_tx_banda.setText(ptr); }
void  O_5_bt_setupPopCallback(void *ptr) { }
void  O_5_tx_estacaoPopCallback(void *ptr) { }
void  O_5_tx_programacaoPopCallback(void *ptr) { }
void  O_5_bt_nextPopCallback(void *ptr) { }
void  O_5_bt_prevPopCallback(void *ptr) { }
void  O_5_bt_p1PopCallback(void *ptr) { }
void  O_5_bt_upPopCallback(void *ptr) { }
void  O_5_bt_downPopCallback(void *ptr) { }
void  O_5_bt_mutePopCallback(void *ptr) { }
void  O_5_bt_p2PopCallback(void *ptr) { }
void  O_5_bt_p3PopCallback(void *ptr) { }
void  O_5_bt_p4PopCallback(void *ptr) { }
void  O_5_bt_p5PopCallback(void *ptr) { }
void  O_5_bt_p6PopCallback(void *ptr) { }
void  O_5_bt_playPopCallback(void *ptr) { }
void  O_5_bt_stopPopCallback(void *ptr) { }


////////////////////////////////////////////////////////////////
void RDS_process(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4) {
  rds.processData(block1, block2, block3, block4);
}
////////////////////////////////////////////////////////////////

bool setup_radio() {
  bool _ok = true;

  pt2314.setChannel(0);                                             // Set all audio processor setting to the fixed values.  
  pt2314.setVolume(63);
  pt2314.setAttn(15, 15);  
  pt2314.setLoudness(false);
  pt2314.setMute(false);
  pt2314.setGain(0);

  #ifdef DEBUG
    Serial.println(F("Config. Processador Audio - PT2314"));
    Serial.print(F("   Channel : "));    Serial.println(pt2314.getChannel());
    Serial.print(F("   Volume  : "));    Serial.println(pt2314.getVolume());
    Serial.print(F("   Attn    : "));    Serial.print(pt2314.getAttnLeft()); Serial.print(F(" - ")); Serial.println(pt2314.getAttnRight());
    Serial.print(F("   Loudness: "));    Serial.println(pt2314.getLoudness());
    Serial.print(F("   Bass    : "));    Serial.println(pt2314.getBass());
    Serial.print(F("   Treble  : "));    Serial.println(pt2314.getTreble());
    Serial.print(F("   Mute    : "));    Serial.println(pt2314.getMute());
    Serial.print(F("   Gain    : "));    Serial.println(pt2314.getGain());
  #endif

  radio.setup(RADIO_RESETPIN, pinoRSTRadio);
  radio.setup(RADIO_SDAPIN, pinoSDA);
  radio.setup(RADIO_FMSPACING, RADIO_FMSPACING_200);                // Set FM Options for USA
  radio.setup(RADIO_DEEMPHASIS, RADIO_DEEMPHASIS_50);
  
  radio.initWire(Wire);                                             // Initialize the Radio

  radio.setBandFrequency(FIX_BAND, FIX_STATION);                    // Set all radio setting to the fixed values.
  radio.setMono(false);
  radio.setMute(false);
  radio.setSoftMute(false);

  radio.attachReceiveRDS(RDS_process);                              // setup the information chain for RDS data.

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

void executaRadio() { 
  
  #ifdef DEBUG
    Serial.println(F("Radio Ativado"));
  #endif

  radio.setVolume(FIX_VOLUME);
  char s[12];
  radio.formatFrequency(s, sizeof(s));

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

  static unsigned long _now          = millis();
  static unsigned long _nextFreqTime = 1;

  encoder->setPosition(FIX_STATION / encoderStep);
  int lastPos = encoder->getPosition() * encoderStep;               // Rotary Frequencia - teste de estado
  Counter     = _volume;                                            // Rotary Volume - Controle
  LastCount   = 0;                                                  // Rotary Volume - teste de estado

  O_5_tx_bandaPopCallback(BAND[radio.getBand()-1]);
  O_5_pb_volume.setValue((int)(radio.getVolume()*6.6667));
  O_5_sl_freq.setValue(lastPos);
  O_5_tx_frequenciaPopCallback(lastPos);
  O_5_bt_mute.setValue(radio.getMute()?1:0);

  while(_ficaLoop) {

    _now = millis();
    
    nexLoop(nex_listen_list_5);                                       // Testa objetos do monitor
    
    encoder->tick();
    
    int newPos = encoder->getPosition() * encoderStep;

    if (newPos < encoderMin) {
      encoder->setPosition(encoderMin / encoderStep);
      newPos = encoderMin;

    } else if (newPos > encoderMax) {
      encoder->setPosition(encoderMax / encoderStep);
      newPos = encoderMax;
    }

    if (lastPos != newPos) {
      #ifdef DEBUG
        Serial.print("Encoder Frequencia: ");
        Serial.println(newPos); 
      #endif
      radio.setFrequency(newPos);
      O_5_sl_freq.setValue(newPos);
      O_5_tx_frequenciaPopCallback(newPos);
      lastPos = newPos;
    }
    
    if (LastCount != Counter) {                                     // Atualiza o controle do Volume
      radio.setVolume(Counter);
      O_5_pb_volume.setValue((int)(Counter*6.6667));
      LastCount = Counter;    
    }
       
    if (rotary_vol.GetButtonDown()) {                               // Controle do Botao MUTE
      radio.setMute(!radio.getMute());
      O_5_bt_mute.setValue(radio.getMute()?1:0);
    }
    else if (_now > _nextFreqTime )                                 // Atualiza tela do radio
    {    
      radio.checkRDS();
      radio.getRadioInfo(&radioInfo);
      radio.getAudioInfo(&audioInfo);
      mostraRDS();

      radio.formatFrequency(s, sizeof(s));

      #ifdef DEBUG
        Serial.println(F("Execução Radio - SI4703"));
        Serial.print(F("   Banda      : "));    Serial.print(BAND[radio.getBand()-1]);
        Serial.print(F("   Frequencia : "));    Serial.println(s);
        Serial.print(F("   Volume     : "));    Serial.println(radio.getVolume());    
        Serial.print(F("   Radio      : "));    radio.debugRadioInfo();
        Serial.print(F("   Audio      : "));    radio.debugAudioInfo();
      #endif    
      
     _nextFreqTime = _now + 1000;
    } 
  }
}
// ****************************************************************************************
// @name     RotaryChangedVol
// @subject  Analiza a mudança de estado do Rotary KY-040
// ****************************************************************************************
void RotaryChangedVol()
{
  const unsigned int state = rotary_vol.GetState();
  
  if (state & DIR_CW) { 
    Counter++;
    if (Counter>radio.getMaxVolume()) {Counter=radio.getMaxVolume();}
  }
  if (state & DIR_CCW) {
    Counter--;    
    if (Counter<0) {Counter=0;}
  }
}
// ****************************************************************************************
// @name     mostraRDS
// @subject  Analiza os dados RDS do Radio
// ****************************************************************************************
void mostraRDS()
{
   rds.attachServiceNameCallback(DisplayServiceName);
   rds.attachTimeCallback(DisplayTime);
}
void DisplayServiceName(char *name) {                                 // Update the ServiceName text when in RDS mode.
  #ifdef DEBUG
    Serial.print("RDS-Name: "); Serial.println(name);
  #endif
} 
void DisplayTime(uint8_t hour, uint8_t minute) {                      // Updata the Time when in RDS-Time.
  #ifdef DEBUG
    Serial.print("RDS-Time: ");
    if (hour < 10) Serial.print('0'); 
      Serial.print(hour); 
    Serial.print(':'); 
    if (minute < 10) Serial.print('0');  
      Serial.println(minute);
  #endif
} 