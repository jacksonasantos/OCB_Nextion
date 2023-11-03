// ****************************************************************************************
// @name     OCB - Onboard Cumputer Beetle
// @subject  Funções Tela 6 - sdcard
// @author   Jackson Alessandro dos Santos
// @date     2023/10/23
// ***************************************************************************************
void O_6_bt_homePopCallback(void *ptr) {
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

// ***************************************************************************************
bool setup_midi() {
  bool _ok = true;
  
  if (!myDFPlayer.begin(FPSerial, true, true)) {                      // Use serial to communicate with mp3.
    _ok = false;
  } else {
    Serial.println(F("DFPlayer Mini  - online"));
    myDFPlayer.setTimeOut(500);                                       // Set serial communictaion time out 500ms
    myDFPlayer.enableDAC();                                           // Enable On-chip DAC
    myDFPlayer.volume(20);                                            // Set volume value (0~30)
    #ifdef DEBUG
      Serial.println(F("Config. DFPlayer Mini - Midi"));
      Serial.print(F("   Type    : "));    printDetailMidi(myDFPlayer.readType(), myDFPlayer.read());   // Print the detail message from DFPlayer to handle different errors and states.
      Serial.print(F("   State   : "));    Serial.println(myDFPlayer.readState());                  // read mp3 state
      Serial.print(F("   Volume  : "));    Serial.println(myDFPlayer.readVolume());                 // read current volume
      Serial.print(F("   Eq      : "));    Serial.println(myDFPlayer.readEQ());                     // read EQ setting
      Serial.print(F("   Files   : "));    Serial.println(myDFPlayer.readFileCounts());             // read all file counts in SD card
      Serial.print(F("   Folder  : "));    Serial.println(myDFPlayer.readFolderCounts());           // read all folder counts SD card
    #endif
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
  }
  return _ok;
}

// ***************************************************************************************
void executaSDCard() {

  pt2314.setChannel(1);                                             // Set all audio processor setting to the fixed values.  
  
  active_page = 6;
  _ficaLoop   = true;
  O_6_midi.show();
  O_6_tx_midi.setText("SD Card");
  delay(500);
  
  static unsigned long _now          = millis();
  static unsigned long _nextFreqTime = 1;

  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.volume(10);
  
  myDFPlayer.outputSetting(true, 15); //output setting, enable the output and set the gain to 15

  #ifdef DEBUG
    Serial.println(F("Config. DFPlayer Mini - Midi"));
    Serial.print(F("   Type    : "));    printDetailMidi(myDFPlayer.readType(), myDFPlayer.read());   // Print the detail message from DFPlayer to handle different errors and states.
    Serial.print(F("   State   : "));    Serial.println(myDFPlayer.readState());                  // read mp3 state
    Serial.print(F("   Volume  : "));    Serial.println(myDFPlayer.readVolume());                 // read current volume
    Serial.print(F("   Eq      : "));    Serial.println(myDFPlayer.readEQ());                     // read EQ setting
    Serial.print(F("   Files   : "));    Serial.println(myDFPlayer.readFileCounts());             // read all file counts in SD card
    Serial.print(F("   Folder  : "));    Serial.println(myDFPlayer.readFolderCounts());           // read all folder counts SD card
    
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

  myDFPlayer.play(1);

  while(_ficaLoop) {
    _now = millis();
    
    nexLoop(nex_listen_list_6);                                       // Testa objetos do monitor

    if (_now > _nextFreqTime ) {                                      // Atualiza tela de midia
      
     _nextFreqTime = _now + 1000;
    }
  }
}

// ***************************************************************************************
void executaUSB() {

  pt2314.setChannel(1);                                               // Set all audio processor setting to the fixed values.  
  
  active_page = 6;
  _ficaLoop   = true;
  O_6_midi.show();
  O_6_tx_midi.setText("USB");
  delay(500);
  
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_U_DISK);                    // Habilita a midia de USB
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.volume(10);
}

// ***************************************************************************************
void executaAuxiliar() {
  O_6_tx_midi.setText("AUXILIAR");
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_AUX);

}

// ***************************************************************************************
void printDetailMidi(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError: "));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}