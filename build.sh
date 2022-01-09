#!/bin/sh
make -C driver clean
make -C driver
cp driver/gc573.ko ./
