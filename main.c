#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "i2c-util.h"

#define I2C_RD    ('r')
#define I2C_WR    ('w')
#define I2C_RD_DELAY    (1000) // microseconds
#define OUTPUT_COLUMNS  (8)

int main(int argc, char **argv)
{
    int fd;
    char r_w;
    unsigned addr, reg;

    if (argc < 6) {
        printf("Usage:\n"
                "For Read: %s /dev/i2c-x addr reg r len\n"
                "For Write: %s /dev/i2c-x addr reg w val [val-2] ... [val-n]\n",
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
    sscanf(argv[4], "%c", &r_w);

    if (I2C_RD == r_w) {
        int len = atoi(argv[5]);
        unsigned char buf[len];
        i2c_read(fd, addr, reg, buf, len, I2C_RD_DELAY);

        printf("read salve: 0x%02x reg: 0x%02x, len: %d\n", addr, reg, len);

        int i = 0;
        for (; i < len; ++i) {
            printf("0x%02x ", buf[i]);
            if (((i + 1) % OUTPUT_COLUMNS) == 0) {
                printf("\n");
            }
        }

        if ((i % OUTPUT_COLUMNS) != 0) {
            printf("\n");
        }
    }
    else { // I2C_WR
        int len = argc - 5;
        unsigned char buf[len];
        for (int i = 0; i < len; ++i) {
            buf[i] = strtol(argv[5+i], NULL, 16);
        }
#if 1
        printf("write salve: 0x%02x reg: 0x%02x, len: %d\n", addr, reg, len);

        int i = 0;
        for (; i < len; ++i) {
            printf("0x%02x ", buf[i]);
            if (((i + 1) % OUTPUT_COLUMNS) == 0) {
                printf("\n");
            }
        }

        if ((i % OUTPUT_COLUMNS) != 0) {
            printf("\n");
        }
#endif
        i2c_write(fd, addr, reg, buf, len);
    }

    return 0;
}
