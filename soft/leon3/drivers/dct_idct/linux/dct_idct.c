#include <linux/of_device.h>
#include <linux/mm.h>

#include <asm/io.h>

#include <esp_accelerator.h>
#include <esp.h>

#include "dct_idct.h"

#define DRV_NAME	"dct_idct"

/* <<--regs-->> */
#define DCT_IDCT_INPUT_COLS_REG 0x50
#define DCT_IDCT_INPUT_ROWS_REG 0x4c
#define DCT_IDCT_DCT_ISTRUE_REG 0x48
#define DCT_IDCT_OUTPUT_ROWS_REG 0x44
#define DCT_IDCT_OUTPUT_COLS_REG 0x40

struct dct_idct_device {
	struct esp_device esp;
};

static struct esp_driver dct_idct_driver;

static struct of_device_id dct_idct_device_ids[] = {
	{
		.name = "SLD_DCT_IDCT",
	},
	{
		.name = "eb_067",
	},
	{
		.compatible = "sld,dct_idct",
	},
	{ },
};

static int dct_idct_devs;

static inline struct dct_idct_device *to_dct_idct(struct esp_device *esp)
{
	return container_of(esp, struct dct_idct_device, esp);
}

static void dct_idct_prep_xfer(struct esp_device *esp, void *arg)
{
	struct dct_idct_access *a = arg;

	/* <<--regs-config-->> */
	iowrite32be(a->input_cols, esp->iomem + DCT_IDCT_INPUT_COLS_REG);
	iowrite32be(a->input_rows, esp->iomem + DCT_IDCT_INPUT_ROWS_REG);
	iowrite32be(a->dct_istrue, esp->iomem + DCT_IDCT_DCT_ISTRUE_REG);
	iowrite32be(a->output_rows, esp->iomem + DCT_IDCT_OUTPUT_ROWS_REG);
	iowrite32be(a->output_cols, esp->iomem + DCT_IDCT_OUTPUT_COLS_REG);
	iowrite32be(a->src_offset, esp->iomem + SRC_OFFSET_REG);
	iowrite32be(a->dst_offset, esp->iomem + DST_OFFSET_REG);

}

static bool dct_idct_xfer_input_ok(struct esp_device *esp, void *arg)
{
	/* struct dct_idct_device *dct_idct = to_dct_idct(esp); */
	/* struct dct_idct_access *a = arg; */

	return true;
}

static int dct_idct_probe(struct platform_device *pdev)
{
	struct dct_idct_device *dct_idct;
	struct esp_device *esp;
	int rc;

	dct_idct = kzalloc(sizeof(*dct_idct), GFP_KERNEL);
	if (dct_idct == NULL)
		return -ENOMEM;
	esp = &dct_idct->esp;
	esp->module = THIS_MODULE;
	esp->number = dct_idct_devs;
	esp->driver = &dct_idct_driver;
	rc = esp_device_register(esp, pdev);
	if (rc)
		goto err;

	dct_idct_devs++;
	return 0;
 err:
	kfree(dct_idct);
	return rc;
}

static int __exit dct_idct_remove(struct platform_device *pdev)
{
	struct esp_device *esp = platform_get_drvdata(pdev);
	struct dct_idct_device *dct_idct = to_dct_idct(esp);

	esp_device_unregister(esp);
	kfree(dct_idct);
	return 0;
}

static struct esp_driver dct_idct_driver = {
	.plat = {
		.probe		= dct_idct_probe,
		.remove		= dct_idct_remove,
		.driver		= {
			.name = DRV_NAME,
			.owner = THIS_MODULE,
			.of_match_table = dct_idct_device_ids,
		},
	},
	.xfer_input_ok	= dct_idct_xfer_input_ok,
	.prep_xfer	= dct_idct_prep_xfer,
	.ioctl_cm	= DCT_IDCT_IOC_ACCESS,
	.arg_size	= sizeof(struct dct_idct_access),
};

static int __init dct_idct_init(void)
{
	return esp_driver_register(&dct_idct_driver);
}

static void __exit dct_idct_exit(void)
{
	esp_driver_unregister(&dct_idct_driver);
}

module_init(dct_idct_init)
module_exit(dct_idct_exit)

MODULE_DEVICE_TABLE(of, dct_idct_device_ids);

MODULE_AUTHOR("Emilio G. Cota <cota@braap.org>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("dct_idct driver");
