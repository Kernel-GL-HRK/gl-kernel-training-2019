#!/bin/bash

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
export KERNELDIR=${HOME}/Kernels/orange-pi-4.19.59

echo -e "\t CROSS_COMPILE \t = ${CROSS_COMPILE}"
echo -e "\t BUILD_KERNEL \t = ${KERNELDIR}"

