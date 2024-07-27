#include <mega65/memory.h>

#ifdef __clang__
volatile struct dmagic_dmalist dmalist;
volatile uint8_t dma_byte;
#else
struct dmagic_dmalist dmalist;
uint8_t dma_byte;
#endif

/**
 * @brief Perform DMA operation
 */
void do_dma(void)
{
    //  unsigned char i;
    mega65_io_enable();

    //  for(i=0;i<24;i++)
    //    printf("%02x ",PEEK(i+(uint16_t)&dmalist));
    //  printf("\n");
    //  while(1) continue;

    // Now run DMA job (to and from anywhere, and list is in low 1MB)
    POKE(0xd702U, 0);
    POKE(0xd704U, 0x00); // List is in $00xxxxx
    POKE(0xd701U, ((uint16_t)&dmalist) >> 8);
    POKE(0xd705U, ((uint16_t)&dmalist) & 0xff); // triggers enhanced DMA
}

uint8_t dma_peek(uint32_t address)
{
    // Read the byte at <address> in 28-bit address space
    // XXX - Optimise out repeated setup etc
    // (separate DMA lists for peek, poke and copy should
    // save space, since most fields can stay initialised).

    dmalist.option_0b = 0x0b;
    dmalist.option_80 = 0x80;
    dmalist.source_mb = (uint8_t)(address >> 20);
    dmalist.option_81 = 0x81;
    dmalist.dest_mb = 0x00; // dma_byte lives in 1st MB
    dmalist.option_85 = 0x85;
    dmalist.dest_skip = 1;
    dmalist.end_of_options = 0x00;
    dmalist.sub_cmd = 0x00;

    dmalist.command = DMA_COPY_CMD;
    dmalist.count = 1;
    dmalist.source_addr = address & 0xffff;
    dmalist.source_bank = (address >> 16) & 0x0f;
    dmalist.dest_addr = (uint16_t)&dma_byte;
    dmalist.dest_bank = 0;

    do_dma();

    return dma_byte;
}

uint8_t db1, db2, db3;

uint8_t lpeek_debounced(uint32_t address)
{
    db1 = 0;
    db2 = 1;
    while (db1 != db2 || db1 != db3) {
        db1 = lpeek(address);
        db2 = lpeek(address);
        db3 = lpeek(address);
    }
    return db1;
}

// cc65 and llvm have specialized assembler versions of lpeek and lpoke
#if !defined(__clang__) && !defined(__CC65__)
uint8_t lpeek(uint32_t address)
{
    return dma_peek(address);
}
void lpoke(uint32_t address, uint8_t value)
{
    dma_poke(address, value);
}
#endif

void dma_poke(uint32_t address, uint8_t value)
{
    dmalist.option_0b = 0x0b;
    dmalist.option_80 = 0x80;
    dmalist.source_mb = 0x00; // dma_byte lives in 1st MB
    dmalist.option_81 = 0x81;
    dmalist.dest_mb = (uint8_t)(address >> 20);
    dmalist.option_85 = 0x85;
    dmalist.dest_skip = 1;
    dmalist.end_of_options = 0x00;
    dmalist.sub_cmd = 0x00;

    dma_byte = value;
    dmalist.command = DMA_COPY_CMD;
    dmalist.count = 1;
    dmalist.source_addr = (uint16_t)&dma_byte;
    dmalist.source_bank = 0;
    dmalist.dest_addr = address & 0xffff;
    dmalist.dest_bank = (address >> 16) & 0x0f;

    do_dma();
    return;
}

void lcopy(
    uint32_t source_address, uint32_t destination_address, size_t count)
{
    dmalist.option_0b = 0x0b;
    dmalist.option_80 = 0x80;
    dmalist.source_mb = (uint8_t)(source_address >> 20);
    dmalist.option_81 = 0x81;
    dmalist.dest_mb = (uint8_t)(destination_address >> 20);
    dmalist.option_85 = 0x85;
    dmalist.dest_skip = 1;
    dmalist.end_of_options = 0x00;
    dmalist.sub_cmd = 0x00;

    dmalist.command = DMA_COPY_CMD;
    dmalist.count = count;
    dmalist.sub_cmd = 0;
    dmalist.source_addr = source_address & 0xffff;
    dmalist.source_bank = (source_address >> 16) & 0x0f;
    // User should provide 28-bit address for IO
    // (otherwise we can't DMA to/from RAM under IO)
    //  if (source_address>=0xd000 && source_address<0xe000)
    //    dmalist.source_bank|=0x80;
    dmalist.dest_addr = destination_address & 0xffff;
    dmalist.dest_bank = (destination_address >> 16) & 0x0f;
    // User should provide 28-bit address for IO
    // (otherwise we can't DMA to/from RAM under IO)
    //  if (destination_address>=0xd000 && destination_address<0xe000)
    //    dmalist.dest_bank|=0x80;

    do_dma();
    return;
}

void lfill(uint32_t destination_address, uint8_t value, size_t count)
{
    dmalist.option_0b = 0x0b;
    dmalist.option_80 = 0x80;
    dmalist.source_mb = 0x00;
    dmalist.option_81 = 0x81;
    dmalist.dest_mb = (uint8_t)(destination_address >> 20);
    dmalist.option_85 = 0x85;
    dmalist.dest_skip = 1;
    dmalist.end_of_options = 0x00;

    dmalist.command = DMA_FILL_CMD;
    dmalist.sub_cmd = 0;
    dmalist.count = count;
    dmalist.source_addr = value;
    dmalist.dest_addr = destination_address & 0xffff;
    dmalist.dest_bank = (destination_address >> 16) & 0x0f;
    // User should provide 28-bit address for IO
    // (otherwise we can't DMA to/from RAM under IO)
    //  if (destination_address>=0xd000 && destination_address<0xe000)
    //    dmalist.dest_bank|=0x80;

    do_dma();
    return;
}

void lfill_skip(
    uint32_t destination_address, uint8_t value, size_t count, uint8_t skip)
{
    dmalist.option_0b = 0x0b;
    dmalist.option_80 = 0x80;
    dmalist.source_mb = 0x00;
    dmalist.option_81 = 0x81;
    dmalist.dest_mb = (uint8_t)(destination_address >> 20);
    dmalist.option_85 = 0x85;
    dmalist.dest_skip = skip;
    dmalist.end_of_options = 0x00;

    dmalist.command = DMA_FILL_CMD;
    dmalist.sub_cmd = 0;
    dmalist.count = count;
    dmalist.source_addr = value;
    dmalist.dest_addr = destination_address & 0xffff;
    dmalist.dest_bank = (destination_address >> 16) & 0x0f;

    do_dma();
    return;
}

void mega65_io_enable(void)
{
    POKE(0xd02fU, 0x47);
    POKE(0xd02fU, 0x53);
    // Force to full speed
    POKE(0, 65);
}
