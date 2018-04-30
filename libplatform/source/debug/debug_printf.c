#include "ejtag/platform/platform.h"
#include "ejtag/platform/debug.h"
#include <stdarg.h>


int DEBUG_printf(const char *fmt, ...)
{
	va_list ap;
	int r;
	va_start(ap, fmt);
	r = DEBUG_vprintf(fmt, ap);
	va_end(ap);
	return r;
}

int DEBUG_snprintf(char *buf, size_t n, const char *fmt, ...)
{
	va_list ap;
	int r;
	va_start(ap, fmt);
	r = DEBUG_vsnprintf(buf,n,fmt, ap);
	va_end(ap);
	return r;
}


