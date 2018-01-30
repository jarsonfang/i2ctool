#ifndef _I2C_UTIL_H
#define _I2C_UTIL_H

#include "i2c-dev.h"

extern int i2c_set(int fd, unsigned int timeout, unsigned int retry);

extern int i2c_read(int fd, unsigned short addr, unsigned short reg,
        void *buf, int len, int delay);

extern int i2c_write(int fd, unsigned short addr, unsigned short reg,
        void *buf, int len);

#endif // _I2C_UTIL_H
