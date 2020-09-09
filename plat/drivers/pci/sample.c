/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Sample PCI driver
 *
 * Author: Punit Agrawal <punitagrawal@gmail.com>
 * Copyright: Punit Agrawal <punitagrawal@gmail.com>
 *
 */

#include <devfs/device.h>
#include <pci/pci_bus.h>
#include <uk/alloc.h>
#include <uk/arch/types.h>
#include <uk/print.h>

#define SAMPLE_VENDOR_ID 0x1CAB
#define SAMPLE_DEVICE_ID 0x4D0E

#define DEV_DUMMY_NAME "dummy"

struct sample_pci_dev {
	struct pci_device *pdev;
	struct device *dev;
	__u16 base_addr;
};

static struct uk_alloc *allocator;

static int dummy_open_op(struct device *device __unused, int mode __unused)
{
	return 0;
}


static int dummy_close_op(struct device *device __unused)
{
	return 0;
}

static int dummy_read_op(struct device *dev __unused, struct uio *uio __unused,
			 int flags __unused)
{
	uk_pr_info("Reading from dummy device\n");
	return 0;
}

static int dummy_write_op(struct device *dev __unused, struct uio *uio,
	int flags __unused)
{
	uio->uio_resid = 0;
	uk_pr_info("Writing to dummy device\n");

	return 0;
}

static struct devops dummy_devops = {
	.open  = dummy_open_op,
	.close = dummy_close_op,
	.read  = dummy_read_op,
	.write = dummy_write_op,
};

static struct driver drv_dummy = {
	.devops = &dummy_devops,
	.devsz  = 0,
	.name   = DEV_DUMMY_NAME,
};

static int sample_pci_add_dev(struct pci_device *pci_dev)
{
	struct sample_pci_dev *sdev;
	struct device *dev;

	sdev = uk_malloc(allocator, sizeof(*sdev));
	if (!sdev) {
		uk_pr_err("Failed to allocate sample device\n");
		return -ENOMEM;
	}

	sdev->pdev = pci_dev;
	sdev->base_addr = pci_dev->base;

	dev = device_create(&drv_dummy, DEV_DUMMY_NAME, D_CHR);
	if (!dev) {
		uk_pr_err("Failed to allocate devfs device\n");
		goto dev_err;
	}
	sdev->dev = dev;

	uk_pr_info("Registerd sample device\n");
	return 0;

dev_err:
	uk_free(allocator, sdev);
	return -ENOMEM;
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
