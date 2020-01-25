#include "mega65_memory.h"

struct dmagic_dmalist dmalist;
unsigned char dma_byte;

void do_dma(void)
{
  unsigned char i;
  m65_io_enable();

  //  for(i=0;i<24;i++)
  // screen_hex_byte(SCREEN_ADDRESS+i*3,PEEK(i+(unsigned int)&dmalist));
  
  // Now run DMA job (to and from anywhere, and list is in low 1MB)
  POKE(0xd702U,0);
  POKE(0xd704U,0x00);  // List is in $00xxxxx
  POKE(0xd701U,((unsigned int)&dmalist)>>8);
  POKE(0xd705U,((unsigned int)&dmalist)&0xff); // triggers enhanced DMA
}

unsigned char lpeek(long address)
{
  // Read the byte at <address> in 28-bit address space
  // XXX - Optimise out repeated setup etc
  // (separate DMA lists for peek, poke and copy should
  // save space, since most fields can stay initialised).

  dmalist.option_0b=0x0b;
  dmalist.option_80=0x80;
  dmalist.source_mb=(address>>20);
  dmalist.option_81=0x81;
  dmalist.dest_mb=0x00; // dma_byte lives in 1st MB
  dmalist.end_of_options=0x00;
  
  dmalist.command=0x00; // copy
  dmalist.count=1;
  dmalist.source_addr=address&0xffff;
  dmalist.source_bank=(address>>16)&0x7f;
  dmalist.dest_addr=(unsigned int)&dma_byte;
  dmalist.dest_bank=0;

  do_dma();
   
  return dma_byte;
}

void lpoke(long address, unsigned char value)
{  

  dmalist.option_0b=0x0b;
  dmalist.option_80=0x80;
  dmalist.source_mb=0x00; // dma_byte lives in 1st MB
  dmalist.option_81=0x81;
  dmalist.dest_mb=(address>>20);
  dmalist.end_of_options=0x00;
  
  dma_byte=value;
  dmalist.command=0x00; // copy
  dmalist.count=1;
  dmalist.source_addr=(unsigned int)&dma_byte;
  dmalist.source_bank=0;
  dmalist.dest_addr=address&0xffff;
  dmalist.dest_bank=(address>>16)&0x7f;

  do_dma(); 
  return;
}

void lcopy(long source_address, long destination_address,
	  unsigned int count)
{
  dmalist.option_0b=0x0b;
  dmalist.option_80=0x80;
  dmalist.source_mb=source_address>>20;
  dmalist.option_81=0x81;
  dmalist.dest_mb=(destination_address>>20);
  dmalist.end_of_options=0x00;

  dmalist.command=0x00; // copy
  dmalist.count=count;
  dmalist.sub_cmd=0;
  dmalist.source_addr=source_address&0xffff;
  dmalist.source_bank=(source_address>>16)&0x0f;
  if (source_address>=0xd000 && source_address<0xe000)
    dmalist.source_bank|=0x80;  
  dmalist.dest_addr=destination_address&0xffff;
  dmalist.dest_bank=(destination_address>>16)&0x0f;
  if (destination_address>=0xd000 && destination_address<0xe000)
    dmalist.dest_bank|=0x80;

  do_dma();
  return;
}

void lfill(long destination_address, unsigned char value,
	  unsigned int count)
{
  dmalist.option_0b=0x0b;
  dmalist.option_80=0x80;
  dmalist.source_mb=0x00;
  dmalist.option_81=0x81;
  dmalist.dest_mb=destination_address>>20;
  dmalist.end_of_options=0x00;

  dmalist.command=0x03; // fill
  dmalist.sub_cmd=0;
  dmalist.count=count;
  dmalist.source_addr=value;
  dmalist.dest_addr=destination_address&0xffff;
  dmalist.dest_bank=(destination_address>>16)&0x7f;
  if (destination_address>=0xd000 && destination_address<0xe000)
    dmalist.dest_bank|=0x80;

  do_dma();
  return;
}

void mega65_io_enable(void)
{
  // Gate C65 IO enable
  POKE(0xd02fU,0x47);
  POKE(0xd02fU,0x53);
  // Force to full speed
  POKE(0,65);
}
