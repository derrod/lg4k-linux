#!/bin/sh
make -C driver clean
make -C driver
cp driver/cx511h.ko ./
