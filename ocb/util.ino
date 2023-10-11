// ****************************************************************************************
// @name     i2c_scanner
// @subject  Analiza e lista os endereços de I2C instalados
// @author   Jackson Alessandro dos Santos
// @date     2023/10/10
// ****************************************************************************************
void i2c_scanner() {
  byte error, address;
  Serial.println(F("Scanning I2C..."));
 
  int nDevices = 0;
  for(address = 1; address < 127; address++ ){
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print(F("I2C device found at address 0x"));
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4) {
      Serial.print(F("Unknown error at address 0x"));
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println(F("No I2C devices found"));
  else
    Serial.println(F("Done Scanning"));
}