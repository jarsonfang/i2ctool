#ifndef _I2C_UTIL_H
#define _I2C_UTIL_H

#include "i2c-dev.h"

extern int i2c_set(int fd, unsigned timeout, unsigned retry);

extern int i2c_read_reg(int fd, unsigned short addr, unsigned char reg,
        unsigned char *buf, int len);

extern int i2c_write_reg(int fd, unsigned short addr, unsigned char reg,
        unsigned char *buf, int len);

extern int i2c_read_reg_delay(int fd, unsigned short addr, unsigned char reg,
        unsigned char *buf, int len, int delay);

#endif // _I2C_UTIL_H
