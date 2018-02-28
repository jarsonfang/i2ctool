#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "i2c-util.h"

#define I2C_DEFAULT_RETRY       (2)
#define I2C_DEFAULT_TIMEOUT     (1)

static int i2c_read_reg(int fd, unsigned short addr,
        void *reg, int reg_size, char *buf, int len)
{
    if ((NULL == reg)
            || (NULL == buf)
            || (len <= 0)
            || (reg_size <= 0))
    {
        fprintf(stderr, "Invalid arguments!\n");
        return 0;
    }

    struct i2c_msg messages[2];
    struct i2c_rdwr_ioctl_data packet;

    unsigned short reg_addr = 0;
    char reg_addr_buf[reg_size];

    if (1 == reg_size) {
        reg_addr = *((char *)reg);
        reg_addr_buf[0] = reg_addr & 0xff;
    }
    else if (2 == reg_size) {
        reg_addr = *((unsigned short *)reg);
        reg_addr_buf[0] = (reg_addr >> 8) & 0xff;
        reg_addr_buf[1] = reg_addr & 0xff;
    }
    else {
        fprintf(stderr, "Invalid reg size!\n");
        return 0;
    }

    packet.nmsgs = 2;
    packet.msgs = messages;

    messages[0].addr =addr;
    messages[0].flags = 0; // write
    messages[0].len = reg_size;
    messages[0].buf = reg_addr_buf;

    messages[1].addr = addr;
    messages[1].flags = I2C_M_RD;
    messages[1].len = len;
    messages[1].buf = buf;

    int ret = ioctl(fd, I2C_RDWR, (unsigned long)&packet);
    if (ret < 0) {
        fprintf(stderr, "Error during I2C_RDWR ioctl: %s\n", strerror(ret));
        return -1;
    }

    return len;
}

static int i2c_read_reg_delay(int fd, unsigned short addr,
        void *reg, int reg_size, char *buf, int len, int delay)
{
    if ((NULL == reg)
            || (NULL == buf)
            || (len <= 0)
            || (reg_size <= 0))
    {
        fprintf(stderr, "Invalid arguments!\n");
        return 0;
    }

    if (delay <= 0) {
        return i2c_read_reg(fd, addr, reg, reg_size, buf, len);
    }

    struct i2c_msg msg;
    struct i2c_rdwr_ioctl_data packet;

    unsigned short reg_addr = 0;
    char reg_addr_buf[reg_size];

    if (1 == reg_size) {
        reg_addr = *((char *)reg);
        reg_addr_buf[0] = reg_addr & 0xff;
    }
    else if (2 == reg_size) {
        reg_addr = *((unsigned short *)reg);
        reg_addr_buf[0] = (reg_addr >> 8) & 0xff;
        reg_addr_buf[1] = reg_addr & 0xff;
    }
    else {
        fprintf(stderr, "Invalid reg size!\n");
        return 0;
    }

    packet.nmsgs = 1;
    packet.msgs = &msg;

    msg.addr = addr;
    msg.flags = 0; // write
    msg.len = reg_size;
    msg.buf = reg_addr_buf;

    int ret = ioctl(fd, I2C_RDWR, (unsigned long)&packet);
    if (ret < 0) {
        fprintf(stderr, "Error during I2C_RDWR ioctl: %s\n", strerror(ret));
        return -1;
    }

    usleep(delay);

    msg.addr = addr;
    msg.flags = I2C_M_RD;
    msg.len = len;
    msg.buf = buf;

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&packet);
    if (ret < 0) {
        fprintf(stderr, "Error during I2C_RDWR ioctl: %s\n", strerror(ret));
        return -1;
    }

    return 0;
}

static int i2c_write_reg(int fd, unsigned short addr,
        void *reg, int reg_size, char *buf, int len)
{
    if ((NULL == reg)
            || (NULL == buf)
            || (len <= 0)
            || (reg_size <= 0))
    {
        fprintf(stderr, "Invalid arguments!\n");
        return 0;
    }

    struct i2c_msg msg;
    struct i2c_rdwr_ioctl_data packet;

    char data[reg_size + len];
    unsigned short reg_addr = 0;

    if (1 == reg_size) {
        reg_addr = *((char *)reg);
        data[0] = reg_addr & 0xff;
    }
    else if (2 == reg_size) {
        reg_addr = *((unsigned short *)reg);
        data[0] = (reg_addr >> 8) & 0xff;
        data[1] = reg_addr & 0xff;
    }
    else {
        fprintf(stderr, "Invalid reg size!\n");
        return 0;
    }

    memcpy((data + reg_size), buf, len);

    msg.addr = addr;
    msg.flags = 0; // write
    msg.len = reg_size + len;
    msg.buf = data;

    packet.nmsgs = 1;
    packet.msgs = &msg;

    int ret = ioctl(fd, I2C_RDWR, (unsigned long)&packet);
    if (ret < 0) {
        fprintf(stderr, "Error during I2C_RDWR ioctl: %s\n", strerror(ret));
        return -1;
    }

    return len;
}

int i2c_set(int fd, unsigned int timeout, unsigned int retry)
{
    if (fd < 2) { // stdin: 0; stdout: 1; stderr: 2
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

int i2c_read(int fd, unsigned short addr, unsigned short reg,
        void *buf, int len, int delay)
{
    if ((NULL == buf)
            || (len <= 0))
    {
        fprintf(stderr, "Invalid arguments!\n");
        return 0;
    }

    int reg_size = 1;
    if ((reg & 0xff00) != 0) {
        reg_size = 2;
    }

    return i2c_read_reg_delay(fd, addr, &reg, reg_size, (char *)buf, len, delay);
}

int i2c_write(int fd, unsigned short addr, unsigned short reg,
        void *buf, int len)
{
    if ((NULL == buf)
            || (len <= 0))
    {
        fprintf(stderr, "Invalid arguments!\n");
        return 0;
    }

    int reg_size = 1;
    if ((reg & 0xff00) != 0) {
        reg_size = 2;
    }

    return i2c_write_reg(fd, addr, &reg, reg_size, (char *)buf, len);
}
