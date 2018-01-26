#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "i2c-util.h"

int main(int argc, char **argv)
{
    int fd;
    char rw_val;
    unsigned r_w;
    unsigned w_val;
    unsigned addr, reg;

    if (argc < 5) {
        printf("Usage:\n%s /dev/i2c-x addr reg r|w[0|1] [val]\n", argv[0]);
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

    if (0 == r_w) {
        i2c_read_reg(fd, addr, reg, &rw_val, 1);
        printf("Read %s-%x reg %x, value = %x\n", argv[1], addr, reg, rw_val);
    } else {
        if (argc < 6) {
            printf("Usage:\n%s /dev/i2c-x addr reg r|w[0|1] [val]\n", argv[0]);
            return 0;
        }

        sscanf(argv[5], "%u", &w_val);
        if ((w_val & ~0xff) != 0) {
            printf("Error on written value %s\n", argv[5]);
        }

        rw_val = (char)w_val;
        i2c_write_reg(fd, addr, reg, &rw_val, 1);
    }

    return 0;
}
