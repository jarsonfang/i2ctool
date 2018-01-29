#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "i2c-util.h"

#define I2C_DEFAULT_RETRY       (2)
#define I2C_DEFAULT_TIMEOUT     (1)

int i2c_set(int fd, unsigned timeout, unsigned retry)
{
    if (fd < 2) {// stdin: 0; stdout: 1; stderr: 2
        fprintf(stderr, "Invalid file descriptor\n");
        return -1;
    }

    if (ioctl(fd, I2C_TIMEOUT, timeout ? timeout : I2C_DEFAULT_TIMEOUT) < 0) {
        return -1;
    }

    if (ioctl(fd, I2C_RETRIES, retry ? retry : I2C_DEFAULT_RETRY) < 0) {
        return -1;
    }

    return 0;
}

int i2c_read_reg(int fd,
        unsigned short addr, unsigned char reg, unsigned char *buf, int len)
{
    if ((NULL == buf)
            || (len <= 0))
    {
        return 0;
    }

    struct i2c_msg messages[2];
    struct i2c_rdwr_ioctl_data packet;

    packet.nmsgs = 2;
    packet.msgs = messages;

    messages[0].addr =addr;
    messages[0].flags = 0; // write
    messages[0].len = 1;
    messages[0].buf = (char *)&reg;

    messages[1].addr = addr;
    messages[1].flags = I2C_M_RD;
    messages[1].len = len;
    messages[1].buf = (char *)buf;

    int ret = ioctl(fd, I2C_RDWR, (unsigned long)&packet);
    if (ret < 0) {
        fprintf(stderr, "Error during I2C_RDWR ioctl: %s\n", strerror(ret));
        return -1;
    }

    return len;
}

int i2c_write_reg(int fd,
        unsigned short addr, unsigned char reg, unsigned char *buf, int len)
{
    if ((NULL == buf)
            || (len <= 0))
    {
        return 0;
    }

    struct i2c_msg msg;
    struct i2c_rdwr_ioctl_data packet;

    int pkg_len = len + 1;
    unsigned char data[pkg_len];

    data[0] = reg;
    memcpy((data + 1), buf, len);

    msg.addr = addr;
    msg.flags = 0; // write
    msg.len = pkg_len;
    msg.buf = (char *)data;

    packet.nmsgs = 1;
    packet.msgs = &msg;

    int ret = ioctl(fd, I2C_RDWR, (unsigned long)&packet);
    if (ret < 0) {
        fprintf(stderr, "Error during I2C_RDWR ioctl: %s\n", strerror(ret));
        return -1;
    }

    return len;
}
