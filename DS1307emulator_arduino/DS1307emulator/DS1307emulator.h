#ifndef DS1307EMULATOR_H_
#define DS1307EMULATOR_H_

#include <Arduino.h>

#define RTC_PROTOCOL_DEVICE_ADDRESS 0xD0 /* of DS1397 protocol */

class DS1307emulator{
   private:
      static void rtcProtocol_init(void);
      static void rtcProtocol_freezeUserData(void);
      static void rtcProtocol_setUserData(void);
      static void rtcProtocol_tickIncrementISR(void);
      static void rtcProtocol_putYear(uint8_t data);
      static void rtcProtocol_putMonth(uint8_t data);
      static void rtcProtocol_putMday(uint8_t data);
      static void rtcProtocol_putWday(uint8_t data);
      static void rtcProtocol_putHour(uint8_t data);
      static void rtcProtocol_putMinute(uint8_t data);
      static void rtcProtocol_putSecond(uint8_t data);
      static void rtcProtocol_putNvRam(uint8_t data, uint8_t w_addr);
      static void rtcProtocol_putControlHandler(uint8_t data);

      static uint8_t rtcProtocol_writeToRTC(unsigned char data);
      static uint8_t rtcProtocol_readUserData(void);
      static uint8_t rtcProtocol_leapCalc(uint8_t year);
      
      static unsigned char bcdToDec(unsigned char data);
      static unsigned char decToBcd(unsigned char data);
      
      static void rtcHal_init(void);
      static void rtcHal_setRtcTick(void);
      static void rtcHal_RtcTick(void);
      static void rtcHal_setDefaultPin(void);
      static void rtcHal_resetRtcTick(void);
      static void rtcHal_startRtcTick(void);
      static void rtcHal_stopRtcTick(void);
      static void rtcHal_resetPinDigitalMode(void);
      static void rtcHal_setPinDigitalMode(void);
      static void rtcHal_setPinDigitalValue(uint8_t value);
     
      static void rtcProtocol_getControlHandler(uint8_t* val);
      static void rtcProtocol_getYear(uint8_t* data);
      static void rtcProtocol_getMonth(uint8_t *data);
      static void rtcProtocol_getMday(uint8_t *data);
      static void rtcProtocol_getWday(uint8_t *data);
      static void rtcProtocol_getHour(uint8_t *data);
      static void rtcProtocol_getMinute(uint8_t *data);
      static void rtcProtocol_getSecond(uint8_t *data);
      static void rtcProtocol_getNvRam(uint8_t *data, uint8_t w_addr);
      
   public:
      DS1307emulator();
      void start();
      void freezeUserData();
      void writeToRTC(uint8_t data);
      void setUserData();
      uint8_t readUserData();
      
      static uint8_t rtcProtocol_getFromRTC(unsigned char *data);

      static void halcbDefaultUnused();

      static void (*HalCb_setDefaultPin)();
      static void (*HalCb_resetPinDigitalMode)();
      static void (*HalCb_setPinDigitalMode)();
      static void (*HalCb_setPinDigitalValue)(uint8_t value);
      
      void attachsetDefaultPin(void (*cbf)());
      void attachresetPinDigitalMode(void (*cbf)());
      void attachssetPinDigitalMode(void (*cbf)());
      void attachsetPinDigitalValue(void (*cbf)());

};




#endif /* DS1307EMULATOR_H_ */
