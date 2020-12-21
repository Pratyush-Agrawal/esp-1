#ifndef _DCT_IDCT_H_
#define _DCT_IDCT_H_

#ifdef __KERNEL__
#include <linux/ioctl.h>
#include <linux/types.h>
#else
#include <sys/ioctl.h>
#include <stdint.h>
#ifndef __user
#define __user
#endif
#endif /* __KERNEL__ */

#include <esp.h>
#include <esp_accelerator.h>

struct dct_idct_access {
	struct esp_access esp;
	/* <<--regs-->> */
	unsigned input_cols;
	unsigned input_rows;
	unsigned dct_istrue;
	unsigned output_rows;
	unsigned output_cols;
	unsigned src_offset;
	unsigned dst_offset;
};

#define DCT_IDCT_IOC_ACCESS	_IOW ('S', 0, struct dct_idct_access)

#endif /* _DCT_IDCT_H_ */
