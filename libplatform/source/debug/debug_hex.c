#include "ejtag/platform/platform.h"
#include "ejtag/platform/debug.h"

static const char xdigits[] = "0123456789abcdef";

static void _hex(uint32_t value, int ndigits)
{
	uint32_t nibble;
	char buf[8];
	char *cp;
	int shft;

	shft = (ndigits * 4) - 4;
	cp = buf-1;

	while (shft >= 0) {
		nibble = value >> shft;
		nibble = nibble & 0x0f;
		cp++;
		*cp = xdigits[nibble];
		shft -= 4;
	}
	DEBUG_write(buf, ndigits);
}

void DEBUG_int(int value)
{
#define INT_BUF_SIZE 32
	char buf[INT_BUF_SIZE];
	int x;
	if (value < 0) {
		DEBUG_hyphen();
		value = -value;
	}
	buf[INT_BUF_SIZE-1] = 0;
	buf[INT_BUF_SIZE-2] = '0';
	x = INT_BUF_SIZE-2;

	while (value) {
		x--;
		buf[x] = '0' + (value % 10);
		value = value / 10;
	}
	DEBUG_write(buf + x, INT_BUF_SIZE - x - 1);
}

void DEBUG_hex8(uint32_t value)
{
	_hex(value, 2);
}

void DEBUG_hex16(uint32_t value)
{
	_hex(value, 4);
}

void DEBUG_hex32(uint32_t value)
{
	_hex(value, 8);
}
void DEBUG_hex64(uint64_t value)
{
	uint64_t vl;
	uint64_t vh;

	vl = value;
	vh = value;
	vh = vh >> 32;

	DEBUG_hex32(((uint32_t)vh));
	DEBUG_hex32(((uint32_t)vl));
}
