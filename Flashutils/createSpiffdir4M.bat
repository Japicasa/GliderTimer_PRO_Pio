.\mkspiffs.exe -c ..\data\ -s 0x110000 -p 256 -b 4096 spiffdata4M.bin
esptool --baud 921600 write_flash  0x2B0000 spiffdata4M.bin