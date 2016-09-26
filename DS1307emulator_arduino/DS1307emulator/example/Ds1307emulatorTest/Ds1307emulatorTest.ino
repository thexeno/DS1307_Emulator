#include <Wire.h>
#include <DS1307emulator.h>


DS1307emulator RTCEmu;

const byte DS1307_Address = 0x68;
byte seconds = 45, oldseconds = 0, minutes = 52, hours = 23, date = 8, month = 12, day = 4, year = 11, protect = 0; // defaults just to start somewhere.
byte NEWseconds = 45, NEWminutes = 52, NEWhours = 23, NEWdate = 8, NEWmonth = 12, NEWday = 4, NEWyear = 11 ; // These are for setting.


void setup() {

  RTCEmu.attachsetDefaultPin(setpinmode);
  RTCEmu.attachresetPinDigitalMode(setpinmode);
  RTCEmu.attachsetPinDigitalMode(setpinmode);
  RTCEmu.attachsetPinDigitalValue(setpinvalue);
  RTCEmu.start();
  RTCsetSqwPinMode();
  Wire.begin(DS1307_Address);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  inittest();

}

void loop() {
  // put your main code here, to run repeatedly:
  rtcloop();
}

void requestEvent(){
  uint8_t i2cbuffer[BUFFER_LENGTH];
  uint8_t arrayptr;

  RTCEmu.freezeUserData();
  for (arrayptr = 0; arrayptr < BUFFER_LENGTH; arrayptr++){
    i2cbuffer[arrayptr] = RTCEmu.readUserData();
  }
  RTCEmu.setUserData();
  Wire.write(i2cbuffer, BUFFER_LENGTH);
}
 
void receiveEvent(int bytesReceived){
  while (Wire.available()){
    RTCEmu.freezeUserData();
    RTCEmu.writeToRTC(Wire.read());
    RTCEmu.setUserData();
  }
}

void setpinvalue(uint8_t value){
  digitalWrite(13, value);
}

void setpinmode(){
  pinMode(13, OUTPUT);
}

void inittest() {
  Serial.begin(9600);
  Serial.println(" DS1307 RTC based clock - Ugi 2012");
  delay(2000); // Give RTC some time to settle.
  RTCsetFromSerial(); //Chance to set time from serial each reset. Could allow for a timeout.
}

void rtcloop() {
  RTCread();
  RTCprintTime(); // sends time to serial.
  delay(1000);
} // End main loop

void RTCsetSqwPinMode(){
  RTCEmu.freezeUserData();
  RTCEmu.writeToRTC(0x07);
  RTCEmu.writeToRTC(0x10);
  RTCEmu.setUserData();
}

// Read time from RTC into time variables.
void RTCread() {
  byte data[11] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  RTCEmu.freezeUserData();
  RTCEmu.writeToRTC(0);
  RTCEmu.setUserData();
  RTCEmu.freezeUserData();

  for (byte _loop = 0; _loop < 10; _loop++) {
    data[_loop] = (RTCEmu.readUserData());
  }

  RTCEmu.setUserData();

  seconds = (((data[0] & B01110000) >> 4) * 10) + (data[0] & B00001111);
  minutes = (((data[1] & B01110000) >> 4) * 10) + (data[1] & B00001111);
  hours = (((data[2] & B00110000) >> 4) * 10) + (data[2] & B00001111);
  day = (data[3] & B00000111);
  date = (((data[4] & B00110000) >> 4) * 10) + (data[4] & B00001111);
  month = (((data[5] & B00010000) >> 4) * 10) + (data[5] & B00001111);
  year = (((data[6] & B11110000) >> 4) * 10) + (data[6] & B00001111);
}

// Write current time to RTC
void RTCwrite() { // writes all current data to RTC - if you only want to write one parameter then just read the clock first!

  byte data[7];
  data[0] = (seconds % 10) + ((seconds / 10) << 4);
  data[1] = (minutes % 10) + ((minutes / 10) << 4);
  data[2] = (hours % 10) + ((hours / 10) << 4);
  data[3] = day;
  data[4] = (date % 10) + ((date / 10) << 4);
  data[5] = (month % 10) + ((month / 10) << 4);
  data[6] = (year % 10) + ((year / 10) << 4);

  RTCEmu.freezeUserData();
  RTCEmu.writeToRTC(0);

  for (byte _loop = 0; _loop < 7; _loop++) {
    RTCEmu.writeToRTC(data[_loop]);
  }
  RTCEmu.setUserData();
  oldseconds = seconds;
} // End RTC Write function


// Print current time to Serial...
void RTCprintTime() {
  RTCread();

  Serial.print("Time = ");
  Serial.print(hours, DEC);
  Serial.print(":");
  Serial.print(minutes, DEC);
  Serial.print(":");
  Serial.println(seconds, DEC);
  Serial.print("Date = ");
  Serial.print(date, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/20");
  Serial.print(year, DEC);

}

byte RTCgetSerial(byte _val, String _Str) {

  Serial.println(" ");
  Serial.println("   *** " + _Str + " ***");
  Serial.print("Current value = ");
  Serial.print(_val, DEC);
  Serial.println(" " + _Str);
  Serial.println("Enter new value (two digits e.g. '01'): ");

  Serial.flush();


  byte NEW_val = -1;
  while (Serial.available() < 2) {
    delay(50);
  }

  NEW_val = ((Serial.read() - 48) * 10);
  NEW_val = NEW_val + (Serial.read() - 48);

  Serial.flush();
  Serial.print("New value = ");
  Serial.print(NEW_val, DEC);
  Serial.println(" " + _Str);
  Serial.println(" ");

  if (NEW_val >= 0) {
    return NEW_val;//
  }
  else {
    return _val;
  }

}

boolean RTCconfirm() {
  Serial.println(" ");
  Serial.println("Currently:");
  RTCprintTime();

  Serial.flush();

  Serial.println(" ");
  Serial.println("Change to? (Y/N):");
  Serial.print("time = ");
  Serial.print(NEWhours, DEC);
  Serial.print(":");
  Serial.print(NEWminutes, DEC);
  Serial.print(":");
  Serial.println(NEWseconds, DEC);
  Serial.print("date = ");
  Serial.print(NEWdate, DEC);
  Serial.print("/");
  Serial.print(NEWmonth, DEC);
  Serial.print("/20");
  Serial.print(NEWyear, DEC);
  char _conf = -1;
  while (Serial.available() < 1) {
    delay(50);
  }

  _conf = Serial.read();

  Serial.flush();

  Serial.print("confirm digit = ");
  Serial.println(_conf);
  if (_conf == 'Y') {
    return true;
  }
  else {
    return false;
  }
}

boolean RTCsetFromSerial() {

  // Get new time setting from serial port
  Serial.flush(); // avoid any random data in buffer.

  // See if we want to update
  char _input = 'X';
  while (_input != 'R' && _input != 'Q') {
    while (Serial.available() < 1) {

      RTCprintTime(); // We'll keep displaying current time and see whether it needs updating.
      Serial.println("Enter 'R' by serial input to re-set or Q to quit setting routine");
      Serial.println(" ");
      delay(1000);
    }
    _input = Serial.read();
    Serial.flush();
  }

  // if(_input != 'R'){Serial.println(" "); Serial.println("   *** QUITTING ***"); Serial.println(" ");  return false;}
  Serial.println(" ");
  Serial.println(" *** RESETTING RTC ***");
  Serial.println(" ");

  RTCprintTime();
  NEWyear = RTCgetSerial(year, "years");
  RTCprintTime();
  NEWday = RTCgetSerial(day, "day of the week");
  RTCprintTime();
  NEWmonth = RTCgetSerial(month, "months");
  RTCprintTime();
  NEWdate = RTCgetSerial(date, "days of the month");
  RTCprintTime();
  NEWhours = RTCgetSerial(hours, "Hours");
  RTCprintTime();
  NEWminutes = RTCgetSerial(minutes, "Minutes");
  RTCprintTime();
  NEWseconds = RTCgetSerial(seconds, "Seconds");

  if (RTCconfirm()) {
    seconds = NEWseconds;
    minutes = NEWminutes;
    hours = NEWhours;
    date = NEWdate;
    month = NEWmonth;
    day = NEWday;
    year = NEWyear;
    RTCwrite();

    Serial.println (" ");
    Serial.println ("   *** RTC Updated ***");
    return true;
  }

  else {
    Serial.println ("   *** Update cancelled  ***");
  }

  return false;
}


