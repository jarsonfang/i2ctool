#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "i2c-util.h"

#define I2C_RD    (0)
#define I2C_WR    (1)

int main(int argc, char **argv)
{
    int fd;
    unsigned r_w;
    unsigned addr, reg;

    if (argc < 6) {
        printf("Usage:\n"
                "For Read: %s /dev/i2c-x addr reg 0 len\n"
                "For Write: %s /dev/i2c-x addr reg 1 val [val-2] ... [val-n]\n",
                argv[0], argv[0]);
        return 0;
    }

    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        printf("Error on opening the device file %s: %s\n",
                argv[1], strerror(errno));
        return -1;
    }

    if (i2c_set(fd, 1, 2) < 0) {
        return -1;
    }

    sscanf(argv[2], "%x", &addr);
    sscanf(argv[3], "%x", &reg);
    sscanf(argv[4], "%u", &r_w);

    if (I2C_RD == r_w) {
        int len = atoi(argv[5]);
        char buf[len];
        i2c_read_reg(fd, addr, reg, buf, len);

        printf("Read %s-%x reg %x, value:\n", argv[1], addr, reg);
        for (int i = 0; i < len; ++len) {
            printf("%x ", buf[i]);
        }
        printf("\n");
    }
    else { // I2C_WR
        int len = argc - 5;
        char buf[len];
        for (int i = 0; i < len; ++i) {
            buf[i] = atoi(argv[5+i]);
        }

        i2c_write_reg(fd, addr, reg, buf, len);
    }

    return 0;
}
