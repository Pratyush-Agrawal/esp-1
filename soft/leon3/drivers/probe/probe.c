#include <stdio.h>
#include <stdlib.h>
#include <esp_probe.h>

void esp_flush(int coherence)
{
	int i;
	int cmd;
	struct esp_device *llcs = NULL;
	int nllc = 0;

	if (coherence == ACC_COH_NONE) {
		/* Look for LLC controller */
		cmd = 1 << ESP_CACHE_CMD_FLUSH_BIT;
		nllc = probe(&llcs, SLD_L3_CACHE, "llc_cache");
	}

	if (coherence != ACC_COH_FULL)
		/* Flush L2 */
		__asm__ __volatile__("sta %%g0, [%%g0] %0\n\t" : :
				"i"(ASI_LEON_DFLUSH) : "memory");

	if (coherence == ACC_COH_NONE) {
		/* Flus LLC */
		for (i = 0; i < nllc; i++) {
			struct esp_device *llc = &llcs[i];
			iowrite32(llc, ESP_CACHE_REG_CMD, cmd);
		}

		/* Wait for LLC flush to complete */
		for (i = 0; i < nllc; i++) {
			struct esp_device *llc = &llcs[i];
			/* Poll for completion */
			while (!(ioread32(llc, ESP_CACHE_REG_STATUS) & ESP_CACHE_STATUS_DONE_MASK));
			/* Clear IRQ */
			iowrite32(llc, ESP_CACHE_REG_CMD, 0);
		}
	}

}

int probe(struct esp_device **espdevs, unsigned devid, const char *name)
{
	int i;
	int ndev = 0;
	unsigned id_reg, bank_addr_reg;
	unsigned *devtable = (unsigned *) APB_PLUGNPLAY;
	unsigned vendor;
	unsigned id;
	unsigned number;
	unsigned irq;
	unsigned addr;
	for (i = 0; i < NAPBSLV; i++) {
		id_reg = devtable[2 * i];
		bank_addr_reg = devtable[2 * i + 1];
		vendor = (id_reg >> 24);
		id     = (id_reg >> 12) & 0x00fff;

		if (vendor == VENDOR_SLD && id == devid) {
			number = ndev;
			addr   = ((bank_addr_reg >> 20) << 8) + APB_BASE_ADDR;
			irq    = id_reg & 0x0000000f;

			ndev++;
			(*espdevs) = realloc((*espdevs), ndev * sizeof(struct esp_device));
			if (!(*espdevs)) {
				fprintf(stderr, "Error: cannot allocate esp_device list\n");
				exit(EXIT_FAILURE);
			}
			(*espdevs)[ndev-1].vendor = VENDOR_SLD;
			(*espdevs)[ndev-1].id = id;
			(*espdevs)[ndev-1].number = number;
			(*espdevs)[ndev-1].irq = irq;
			(*espdevs)[ndev-1].addr = addr;
			printf("Registered %s.%d:\n", name, (*espdevs)[ndev-1].number);
			printf("  addr = 0x%08x\n", (*espdevs)[ndev-1].addr);
			printf("  irq  = %d\n", (*espdevs)[ndev-1].irq);
		}
	}
	printf("\n");
	return ndev;
}

unsigned ioread32(struct esp_device *dev, unsigned offset)
{
	const long addr = dev->addr + offset;
	volatile unsigned *reg = (unsigned *) addr;
	return *reg;
}

void iowrite32(struct esp_device *dev, unsigned offset, unsigned payload)
{
	const long addr = dev->addr + offset;
	volatile unsigned *reg = (unsigned *) addr;
	*reg = payload;
}
