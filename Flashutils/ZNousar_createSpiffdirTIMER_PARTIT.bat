.\mkspiffs.exe -c ..\data\ -s 0x250000 -p 256 -b 4096 spiffdata.bin
esptool --baud 921600 write_flash  0xDB0000 spiffdata.bin