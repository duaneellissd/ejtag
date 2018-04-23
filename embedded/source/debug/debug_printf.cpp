#include "ejtag/ejtag.h"
#include <stdio.h>
#include <stdarg.h>


int EjtagDebug::printf(EjtagDebug::DbgArea area, const char *fmt, ...)
{
	va_list ap;
	int r;
	va_start(ap, fmt);
	r = this->vprintf(area, fmt, ap);
	va_end(ap);
	return r;
}

int EjtagDebug::vprintf( EjtagDebug::DbgArea area, const char *fmt, va_list ap)
{
	int r;
	const char *cp;

	cp = this->area2name(area);
	if (cp) {
		this->puts_no_nl(cp);
		this->colon_space();
	}

	char buf[128];
	r = vsnprintf(buf, sizeof(buf), fmt, ap);
	buf[sizeof(buf) - 1] = 0;

	this->puts_no_nl(buf);
	return r;
}


