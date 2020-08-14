/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * entry.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
 
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/pci.h>
#include "aver_version.h"

//#define DRIVER_VER "1.0.0026"
int board_init(void);
void board_exit(void);
static int __init aver_init(void)
{
    int ret=0;

    ret = board_init();
    return ret;
}

static void __exit aver_fini(void)
{
	board_exit();
}

module_init(aver_init);
module_exit(aver_fini);

MODULE_VERSION(DRIVER_VER);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Driver for cx511h capture card" " v" DRIVER_VER);
MODULE_AUTHOR("AVerMedia Tech. Inc.");


static struct pci_device_id aver_ids[  ] = {
    { PCI_DEVICE(0x1461, 0x0054) },
    { 0, },
};

MODULE_DEVICE_TABLE(pci, aver_ids);

