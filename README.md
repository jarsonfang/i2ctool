# i2ctool
simple i2ctool usefull for bringup/debugging

Dependencies:
  This tool requires i2c device interface supported and enabled in the kernel configuration (see https://www.kernel.org/doc/Documentation/i2c/dev-interface)

Usage:  
For Read: ./i2ctool /dev/i2c-x addr reg r len  
For Write: ./i2ctool /dev/i2c-x addr reg w val [val-2] ... [val-n]
