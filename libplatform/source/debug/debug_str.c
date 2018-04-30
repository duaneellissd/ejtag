#include "ejtag/platform/platform.h"
#include "ejtag/platform/debug.h"
#include "ejtag/platform/string.h"

int DEBUG_puts_no_nl(const char *s)
{
	return DEBUG_write(s, strlen(s));
}

int DEBUG_puts(const char *s)
{
	int r;
	r = DEBUG_puts_no_nl(s);
	r += 1; /* for the newline */
	DEBUG_newline();
	return r;
}

void DEBUG_str_hex8(const char *str, uint32_t value)
{
	DEBUG_str_colon_space(str);
	DEBUG_hex8(value);
	DEBUG_newline();
}

void DEBUG_str_hex16(const char *str, uint32_t value)
{
	DEBUG_str_colon_space(str);
	DEBUG_hex16(value);
	DEBUG_newline();
}

void DEBUG_str_hex32(const char *str, uint32_t value)
{
	DEBUG_str_colon_space(str);
	DEBUG_hex32(value);
	DEBUG_newline();
}

void DEBUG_str_hex64(const char *str, uint64_t value)
{
	DEBUG_str_colon_space(str);
	DEBUG_hex64(value);
	DEBUG_newline();
}

void DEBUG_str_int(const char *str, int value)
{
	DEBUG_str_colon_space(str);
	DEBUG_int(value);
	DEBUG_newline();
}
