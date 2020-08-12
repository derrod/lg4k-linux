#!/bin/bash

SAMPLERATE=48000
#DEVICE_INDEX=$(cat /proc/asound/C353/pcm0c/info |grep card: | awk '{print $2}')
#DELAY=500000000 # 500 ms
DELAY=0
VDEV=0
ADEV=1

if [ "$2" != "" ]; then
	VDEV=$2
fi

if [ "$3" != "" ]; then
	ADEV=$3
fi

v4l2-ctl --device=/dev/video$VDEV --set-input=$1
#v4l2-ctl --device=/dev/video$VDEV --set-fmt-video=width=0,height=0,pixelformat=YV12

WIDTH=$(v4l2-ctl --device=/dev/video$VDEV -V|grep Width | awk '{print $3}' | cut -d '/' -f1)
HEIGHT=$(v4l2-ctl --device=/dev/video$VDEV -V|grep Width | awk '{print $3}' | cut -d '/' -f2)

gst-launch-1.0 v4l2src device=/dev/video$VDEV ! \
	"video/x-raw,width=$WIDTH,height=$HEIGHT,format=(string)YUY2" ! \
	timeoverlay text="time:" ! \
	queue ! \
	videoconvert !	\
	autovideosink sync=false async=false \
	alsasrc device=hw:$ADEV,0 ! \
	audio/x-raw,rate=$SAMPLERATE,channels=2,depth=16,blocksize=14400,buffer-time=50 ! \
	queue max-size-buffers=0 max-size-time=0 max-size-bytes=0 min-threshold-time=$DELAY ! \
	alsasink sync=false async=false
	
#gst-launch-1.0 alsasrc device=hw:$ADEV,0 ! \
#	audio/x-raw,rate=$SAMPLERATE,channels=2,depth=16,blocksize=14400,buffer-time=50 ! \
#	queue max-size-buffers=0 max-size-time=0 max-size-bytes=0 min-threshold-time=$DELAY ! \
#	alsasink sync=false async=false	\
#	v4l2src device=/dev/video$VDEV ! \
#	"video/x-raw,width=$WIDTH,height=$HEIGHT,format=(string)YUY2" ! \
#	timeoverlay text="time:" ! \
#	queue ! \
#	videoconvert !	\
#	xvimagesink sync=false async=false \
	

#async=false
#gst-launch-0.10 v4l2src device=/dev/video$VDEV ! \
#	"video/x-raw-yuv,width=$WIDTH,height=$HEIGHT,format=(fourcc)YV12" ! \
#	timeoverlay halign=left valign=bottom text="time:" ! \
#	queue ! \
#	ffmpegcolorspace ! ximagesink sync=false async=false \
#	alsasrc device=hw:$DEVICE_INDEX,0 ! \
#	audio/x-raw-int,rate=$SAMPLERATE,channel=2,depth=16 ! \
#	queue max-size-buffers=0 max-size-time=0 max-size-bytes=0 min-threshold-time=$DELAY ! \
#	alsasink sync=false async=false
