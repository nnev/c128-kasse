#ifndef __VDC_UTIL_H_
#define __VDC_UTIL_H_

extern void __fastcall__ vdc_load_thinfont(void);
extern unsigned char __fastcall__ vdc_read_reg(unsigned char reg);
extern unsigned __fastcall__ vdc_read_addr(unsigned char reg);
extern void __fastcall__ vdc_write_reg(unsigned char reg, unsigned char data);
extern void __fastcall__ vdc_write_addr(unsigned char reg, unsigned addr);
extern void __fastcall__ vdc_read_mem(unsigned dest, unsigned src, unsigned n);
extern void __fastcall__ vdc_write_mem(unsigned dest, const void *src,
                                       unsigned n);
extern void __fastcall__ vdc_load_thinfont(void);

#endif // __VDC_UTIL_H_
