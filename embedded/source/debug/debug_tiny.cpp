#include "ejtag/ejtag.h"
#include <string.h>

static const char xdigits[] = "0123456789abcdef";

static void _hex(EjtagDebug *pThis, uint32_t value, int ndigits )
{
	uint32_t v2;
	char buf[8];
	int x;

	for (x = 0; x < ndigits; x++) {
		v2 = value >> (4 * (ndigits - x - 1));
		buf[x] = xdigits[v2];
	}
	pThis->write(buf, ndigits);
}

EjtagDebug::EjtagDebug(EjtagInstance *pInstance)
{
	mInstance = pInstance;
}

void EjtagDebug::setInstance(EjtagInstance *pInstance)
{
	mInstance = pInstance;
}

const char *EjtagDebug::area2name(enum EjtagDebug::DbgArea area )
{
	return NULL;
}

void EjtagDebug::write_check(const char *s, size_t n)
{
	this->now();
	this->write(s, n);
}

void EjtagDebug::now(void)
{
	uint32_t now;
	char buf[32];
	int x;

	if (!this->need_now) {
		return;
	}

	now = mInstance->mPlatform.msecs_now();

	buf[31] = 0;
	buf[30] = ' ';
	buf[29] = '0' + (now % 10);
	now = now / 10;
	buf[28] = '0' + (now % 10);
	now = now / 10;
	buf[27] = '0' + (now % 10);
	now = now / 10;
	buf[26] = '.';

	if (now == 0) {
		buf[25] = '0';
		x = 24;
	}
	else {
		x = 25;
		while (now) {
			buf[x] = '0' + (now % 10);
			now /= 10;
			x--;
		}
	}
	while (x >= 21){
		buf[x] = ' ';
		x--;
	}
	this->write(buf + 1 + x, 32 - x);
}

void EjtagDebug::hyphen(void)
{
	this->write_check("-", 1);
}



void EjtagDebug::colon_space(void)
{
	this->write_check(": ",2);
}

void EjtagDebug::bar(void)
{
	this->write_check("|",1);
}


void EjtagDebug::colon(void)
{
	this->write_check(":",1);
}

void EjtagDebug::space(void)
{
	this->puts_no_nl(" ");
}

void EjtagDebug::newline(void)
{
	this->write("\r\n", 2);
	this->need_now = true;
}


void EjtagDebug::putc( int c)
{
	if (c == '\n') {
		this->newline();
		return;
	} 

	this->now();
	char buf[2];
	buf[0] = (char)(c);
	buf[1] = 0;
	this->write( buf, 1);
}

void EjtagDebug::puts_no_nl(const char *s)
{
	const char *cp;
	size_t len;

	for (;;) {
		cp = strchr(s, '\n');
		if (cp == NULL) {
			break;
		}
		len = cp - s;
		if (len) {
			this->write( s, len);
			s += (len + 1);
		}
		this->newline();
	}


}
void EjtagDebug::puts(const char *s)
{
	this->puts_no_nl(s);
	this->newline();
}


void EjtagDebug::hex8(uint32_t value)
{
	_hex(this,value, 2);
}

void EjtagDebug::hex16(uint32_t value)
{
	_hex(this, value, 4);
}

void EjtagDebug::hex32(uint32_t value)
{
	_hex(this, value, 8);
}
void EjtagDebug::hex64(uint64_t value)
{
	uint64_t vl;
	uint64_t vh;

	vl = value;
	vh = value;
	vh = vh >> 32;

	this->hex32(((uint32_t)vh));
	this->hex32(((uint32_t)vl));
}

static void _decimal(EjtagDebug *pThis, int value)
{
	if (value > 10) {
		_decimal(pThis, value / 10);
	}
	pThis->write(xdigits+value, 1);
}

void EjtagDebug::integer(int value)
{
	if (value == 0) {
		this->write("0", 1);
		return;
	}
	if (value < 0) {
		this->hyphen();
		value = -value;
	}
	_decimal(this, value);
}

static void scs(EjtagDebug *pThis, const char *str)
{
	pThis->puts_no_nl(str);
	pThis->colon_space();
}

void EjtagDebug::str_hex8(const char *str, uint32_t value)
{
	scs(this, str);
	this->hex8(value);
}

void EjtagDebug::str_hex16(const char *str, uint32_t value)
{
	scs(this, str);
	this->hex16(value);
}

void EjtagDebug::str_hex32(const char *str, uint32_t value)
{
	scs(this, str);
	this->hex32(value);
}

void EjtagDebug::str_hex64(const char *str, uint64_t value)
{
	scs(this, str);
	this->hex64(value);
}



void EjtagDebug::assertFails(const char *filename,
	int lineno,
	const char *msg)
{
	int c;
	/* avoid printf()*/
	for (;;) {
		this->newline();
		this->puts_no_nl(filename);
		this->colon();
		this->integer(lineno);
		this->colon();
		this->puts_no_nl("Assertion Fails: ");
		this->puts_no_nl(msg);
		this->newline();
		while (!this->kbhit())
			;
		c = this->getkey();
		if (c == 0x123456) {
			break;
		}
	}
}