#!/bin/bash

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)

modprobe snd
modprobe snd-pcm
modprobe videobuf2-dma-contig
modprobe videobuf2-dma-sg
modprobe videobuf2-v4l2
modprobe videobuf2-vmalloc
modprobe videodev
insmod driver/gc573.ko no_signal_pic="$SCRIPT_DIR/driver/aver_custom_no_signal.bmp" copy_protection_pic="$SCRIPT_DIR/driver/aver_custom_copy_protection.bmp" out_of_range_pic="$SCRIPT_DIR/driver/aver_custom_out_of_range.bmp"
