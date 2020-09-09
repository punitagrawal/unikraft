/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Sample PCI driver
 *
 * Author: Punit Agrawal <punitagrawal@gmail.com>
 * Copyright: Punit Agrawal <punitagrawal@gmail.com>
 *
 */

#include <pci/pci_bus.h>
#include <uk/alloc.h>
#include <uk/arch/types.h>
#include <uk/print.h>

#define SAMPLE_VENDOR_ID 0x1CAB
#define SAMPLE_DEVICE_ID 0x4D0E

struct sample_pci_dev {
	struct pci_device *pdev;
	__u16 base_addr;
};

static struct uk_alloc *allocator;

static int sample_pci_add_dev(struct pci_device *pci_dev)
{
	struct sample_pci_dev *sdev;

	sdev = uk_malloc(allocator, sizeof(*sdev));
	if (!sdev) {
		uk_pr_err("Failed to allocate sample device\n");
		return -ENOMEM;
	}

	sdev->pdev = pci_dev;
	sdev->base_addr = pci_dev->base;

	uk_pr_info("Registerd sample device\n");
	return 0;
}

static int sample_pci_drv_init(struct uk_alloc *drv_allocator)
{
	allocator = drv_allocator;
	return 0;
}

static const struct pci_device_id pci_ids[] = {
	{ PCI_DEVICE_ID(SAMPLE_VENDOR_ID, SAMPLE_DEVICE_ID) },
	{ PCI_ANY_DEVICE_ID },
};

static struct pci_driver sample_pci_drv = {
	.device_ids = pci_ids,
	.init       = sample_pci_drv_init,
	.add_dev    = sample_pci_add_dev,
};
PCI_REGISTER_DRIVER(&sample_pci_drv);
