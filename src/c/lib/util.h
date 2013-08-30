#ifndef _UTIL_H_
#define _UTIL_H_

#define FCY     16e6
#define TCY     62.5e-9

#ifndef NULL
#define NULL 0
#endif

#define peek(addr)              *(addr)
#define poke(addr, val)         *(addr) = val
#define bitread(addr, bit)      (((*(addr))&(1<<bit)) ? 1:0)
#define bitset(addr, bit)       *(addr) |= 1<<bit
#define bitclear(addr, bit)     *(addr) &= ~(1<<bit)
#define bitflip(addr, bit)      *(addr) ^= 1<<bit

//extern unsigned int peek(unsigned int *addr);
//extern void poke(unsigned int *addr, unsigned int val);
//extern unsigned int bitread(unsigned int *addr, unsigned char bit);
//extern void bitset(unsigned int *addr, unsigned char bit);
//extern void bitclear(unsigned int *addr, unsigned char bit);
//extern void bitflip(unsigned int *addr, unsigned char bit);

typedef unsigned char BYTE;

typedef union {
	int i;
	unsigned int w;
	BYTE b[2];
} WORD;

typedef union {
	long l;
	unsigned long ul;
	unsigned int w[2];
	BYTE b[4];
} WORD32;

#endif
