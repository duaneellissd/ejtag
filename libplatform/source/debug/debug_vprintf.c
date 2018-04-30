#include "ejtag/platform/platform.h"
#include "ejtag/platform/debug.h"
#include "ejtag/platform/string.h"
#include <stdio.h>
/* 
 * Note: One day, this might be replaced with a "poor mans printf"
 * but for now, we use the system provided solution 
 */

int DEBUG_vprintf(const char *fmt, va_list ap)
{
	char buf[128];
	DEBUG_vsnprintf(buf, sizeof(buf), fmt, ap);
	/* ensure null termination */
	buf[sizeof(buf) - 1] = 0;
	return DEBUG_write(buf, strlen(buf));
}

int DEBUG_vsnprintf(char *buf, size_t n, const char *fmt, va_list ap)
{
	/* Yes, this does not match the printf() standard
	 * which is - the numbe of bytes that would have been written
	 */
	snprintf(buf, n, fmt, ap);
	buf[n - 1] = 0;
	return (int)strlen(buf);
}

