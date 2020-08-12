# Live Gamer 4K Linux driver

Based on the Linux driver for the [CL511HN](https://www.avermedia.com/professional/product/cl511hn/overview)
which can be downloaded here: [Generic_Ubuntu_CL511HN_Drv_1.0.0031.zip](http://storage.avermedia.com/web_release_www/CL511HN/Generic_Ubuntu_CL511HN_Drv_1.0.0031.zip)

The CL511HN and GC573 (Live Gamer 4K) are very similar and their drivers are *mostly* interchangable. Though the firmware differences mean that some modes won't work on the LG4K.

This was tested on Ubuntu 20.04 LTS.

**Caveats:**
* Driver needs to be recompiled/installed manually every time there's a kernel update
* RGB LEDs are not supported and will keep flashing red
* Scaler doesn't seem to work so selected resolution has to match input resolution
* May not support distros other than Ubuntu (based on CL511HN page)
* Driver isn't automatically loaded

## How to

* clone git repository
* run `./build.sh`
* run `sudo ./install.sh`
* run `sudo modprobe cx511h`

## ToDo

* Make driver load automatically
* Maybe see if we can send a signal to turn off the LEDs by looking at the Windows driver
* Check some audio issues in OBS and see why RGB24 doesn't work in there either
