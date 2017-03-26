#include <stdint.h>
#ifndef _MODBUSPROTOCOL
#define _MODBUSPROTOCOL

//Maximum device list for network
#define DEVMAX		10
//Maximum control register que size
#define QUEMAX		10
//Maximum serial wait in micro seconds
#define SERIALMAXDELAY	100
#define SERIALBAUD		9600
//the total silence time needed to signify an EOM or SOM in RTU mode

//Modbus function codes
#define READ_DO 	0x01  // чтение из регистров 1 биты
#define READ_DI		0x02  // чтение из регистров 100000 биты
#define READ_AO 	0x03  // чтение из регистров 400000 двух байтное число
#define READ_AI		0x04  // только чтение из регистров 300000 двух байтное число

#define WRITE_DO	0x05  //  запись в регистры 1 биты
#define WRITE_AO	0x06  //  запись в регистры 400000 двух байтное число

#define RTU 		0x01
#define ASCII		0x02

#define MASTER		0x01
#define SLAVE		0x02

#define DO			0x00
#define DI			0x01
#define AI			0x03
#define AO			0x04

#endif