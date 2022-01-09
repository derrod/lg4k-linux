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
install -m 644 driver/gc573.ko $MODULE_INSTALL_DIR
install -m 644 driver/aver_custom_copy_protection.bmp $MODULE_INSTALL_DIR
install -m 644 driver/aver_custom_no_signal.bmp $MODULE_INSTALL_DIR
install -m 644 driver/aver_custom_out_of_range.bmp $MODULE_INSTALL_DIR

echo "options gc573 no_signal_pic=\"$MODULE_INSTALL_DIR/aver_custom_no_signal.bmp\" out_of_range_pic=\"$MODULE_INSTALL_DIR/aver_custom_out_of_range.bmp\" copy_protection_pic=\"$MODULE_INSTALL_DIR/aver_custom_copy_protection.bmp\"" > /etc/modprobe.d/gc573.conf

/sbin/depmod -a
