#!/bin/bash

export BUILD_KERNEL=$(pwd)/../../build/kernel_opi
export KERNEL_VERSION=5.4.0-rc1-00258-g2d00aee21a5d
export DTC=$BUILD_KERNEL/scripts/dtc/dtc
export CROSS_COMPILE=$(pwd)/../../toolchain/gcc-linaro-7.4.1-2019.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
export ARCH=arm
export SSH_ORANGE=root@192.168.1.104
#export INCLUDE_DTS=/home/tymbys/WORK/GL/linux/arch/arm/boot/dts
export DCT_EXTRA_PATH=/home/tymbys/WORK/GL/linux/


make clean && make && make upload