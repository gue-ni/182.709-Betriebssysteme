# Raspberry Pi OS

https://github.com/s-matyukevich/raspberry-pi-os
https://www.unixmen.com/how-to-install-and-configure-qemu-in-ubuntu/
https://raspberrypi.stackexchange.com/questions/34733/how-to-do-qemu-emulation-for-bare-metal-raspberry-pi-images/85135#85135

## QEMU
qemu-system-aarch64 -m 128 -M raspi3 -cpu cortex-a53 -kernel kernel8.img -serial null -serial vc
qemu-system-aarch64 -m 128 -M raspi3 -cpu cortex-a53 -kernel kernel8.img -nographic -serial null -chardev stdio,id=uart1 -serial chardev:uart1 -monitor none
