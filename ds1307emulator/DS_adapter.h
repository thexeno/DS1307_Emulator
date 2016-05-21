/* 
 * File:   DS_adapter.h
 * Author: thexeno
 *
 * Created on 2 novembre 2014, 10.28
 */

#ifndef DS_ADAPTER_H
#define	DS_ADAPTER_H

#ifdef	__cplusplus
extern "C" {
#endif


/*DS RTC defines*/
#define SEC 0x00
#define MIN 0x01
#define HOUR 0x02
#define WDAY 0x03
#define MDAY 0x04
#define MONTH 0x05
#define YEAR 0x06

#define MAX_ADDR 0x3F
#define CONTROL_ADDRESS 0x07
#define MAX_DATA_ADDRESS 0x06
#define NVRAM_BEGIN_ADDR 0x08

extern uint8_t buff_ovf;
extern uint8_t RxBuffer;
extern uint8_t temp;
extern uint8_t rxCount;
extern uint8_t rx_begin;
extern uint8_t rx_complete;
extern char timetable[];
extern uint8_t w_addr;
extern uint8_t dsNvRam[];
extern uint8_t __12_not24, Pm_notAm, CH;


unsigned char DS_auxGet() ;
void DS_auxPut(unsigned char data, unsigned char w_addr) ;
void rtcGet();
void rtcPut(unsigned char data, unsigned char w_addr);
void readReg();
void updReg();
void dsEmulator_handler();

#ifdef	__cplusplus
}
#endif

#endif	/* DS_ADAPTER_H */

