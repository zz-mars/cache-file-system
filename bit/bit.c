#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MASK_32	0xffffffff

#define MASK_16	0xfffff

#define MASK_8	0xff

#define BIT_MAP_BYTES	8

static unsigned char bitmap[BIT_MAP_BYTES];

unsigned char *init_bitmap(int bytes)
{
	if(bytes <= 0) {
		fprintf(stderr,"invalid arg!\n");
		return NULL;
	}

	unsigned char *bitmap = (unsigned char*)malloc(bytes);
	if(!bitmap) {
		perror("malloc");
		return NULL;
	}

	memset(bitmap,0,bytes);

	return bitmap;
}

void destroy_bitmap(unsigned char *bitmap)
{
	if(bitmap) {
		free(bitmap);
	}
}

int bit_count(unsigned char *bitmap)
{
	int c = 0;
	int i;
	for(i=0;i<bytes;i++) {
		unsigned char b = bitmap[i];
		while(b) {
			c++;
			b &= (b-1);
		}
	}
	return c;
}

int first_zero_bit(unsigned char *bitmap)
{
	int int_32_nr = BIT_MAP_BYTES/4;

	unsigned int32_t *p = (unsigned int32_t *)bitmap;

	int index = 0;

	int i;
	for(i=0;i<int_32_nr;i++) {

		index += 32;

		unsigned int32_t x = p[i];
		if(x ^ MASK_32 == 0) {
			continue;
		}

		unsigned int16_t y;

		unsigned int16_t m = (x >> 16) & MASK_16;
		unsigned int16_t n = x & MASK_16;

		assert(!(m == MASK_16 && n == MASK_16));

		unsigned int16_t y;

		if(m ^ MASK_16) {
			y = m;
		}else {
			index += 16;
			y = n;
		}

		unsigned char s = (y >> 8) & MASK_8;
		unsigned char t = y & MASK_8;

		assert(!(s == MASK_8 && t == MASK_8));

		unsigned char z;
		if(s ^ MASK_8) {
			z = s;
		} else {
			index += 8;
			z = t;
		}

		int k;
		for(k=0;k<8;k++) {
			if(z & (1 << (7-k)) == 0) {
				return (index+k);
			}
		}

	}
}

int set_bit(unsigned char *bitmap,int index)
{
	int byte_idx = index/8;
	int byte_off = index%8;
	bitmap[byte_idx] |= (1 << (7-byte_off));
	return 0;
}

int get_bit(unsigned char *bitmap,int index)
{
	int byte_idx = index/8;
	int byte_off = index%8;
	return bitmap[byte_idx] & (1 << (7-byte_off));
}

int clear_bit(unsigned char *bitmap,int index)
{
	int byte_idx = index/8;
	int byte_off = index%8;
	bitmap[byte_idx] &= ~(1 << (7-byte_off));
	return 0;
}

int main()
{
}

