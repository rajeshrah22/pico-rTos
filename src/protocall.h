#ifndef PROTOCALL_H
#define PROTOCALL_H

#include <stdint.h>
#include <stddef.h>

#define PROTOCALL_NOT_VALID 0
#define PROTOCALL_VALID 1
#define PROTOCALL_INVALID -1

int protocall_appended(uint8_t data);
uint8_t* protocall_getdata(size_t *len);
uint8_t protocall_getbrightness(void);
uint16_t protocall_getspeed(void);

#endif // PROTOCALL_H
