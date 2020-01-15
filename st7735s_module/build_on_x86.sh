#!/bin/bash -e

NAME=st7735s
NAMELINUX=root@192.168.0.130
MODULEDIR=/lib/modules/4.19.83-sunxi/kernel/drivers/iio
DTSDIR=$PWD/dtsi

# parse commandline options
while [ ! -z "$1" ] ; do
        case $1 in
            --clean)
                echo "Clean module sources"
                make clean
		rm -f ${DTSDIR}/${NAME}.dtbo
                ;;
            --module)
                echo "Build module"
                make
                ;;
            --dtbo)
                echo "Compile dtbo"
		${KERNELDIR}/scripts/dtc/dtc -I dts -O dtb \
			-o ${DTSDIR}/${NAME}.dtbo \
			${DTSDIR}/${NAME}.dtsi
                ;;
            --copymod)
                echo "Copy module to board"
                scp ${NAME}.ko ${NAMELINUX}:${MODULEDIR}
                ;;
            --copydtbo)
                echo "Copy overlay to board"
		scp ${PWD}/dtsi/${NAME}.dtbo \
			${NAMELINUX}:/boot/overlay-user
                ;;
            --copysshid)
                echo "Copy ssh id to board"
		ssh-copy-id -i ~/.ssh/id_rsa.pub ${NAMELINUX}
                ;;
        esac
        shift
done

echo "Done!"

