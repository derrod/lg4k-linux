#!/bin/sh
#TARGET_KERNEL_VER=4.10.0-38-generic
#KERNEL_VER=`uname -r`
#if [ $KERNEL_VER!=$TARGET_KERNEL_VER ]
#then
#echo "current kernel version is not match $(TARGET_KERNEL_VER).
#echo "Please boot to correct kernel version then install again. Thanks"
#exit
#fi
MODULE_INSTALL_DIR=/lib/modules/`uname -r`/kernel/drivers/media/avermedia
install -d  $MODULE_INSTALL_DIR
install -m 644 cx511h.ko $MODULE_INSTALL_DIR
/sbin/depmod -a
