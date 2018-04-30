#include "ejtag/platform/platform.h"
#include "ejtag/platform/debug.h"

void DEBUG_hyphen(void)
{
	DEBUG_putc('-');
}

void DEBUG_str_colon_space(const char *s)
{
	DEBUG_puts_no_nl(s);
	DEBUG_colon_space();
}

void DEBUG_colon_space(void)
{
	DEBUG_colon();
	DEBUG_space();
}

void DEBUG_bar(void)
{
	DEBUG_putc('|');
}

void DEBUG_colon(void)
{
	DEBUG_putc(':');
}

void DEBUG_space(void)
{
	DEBUG_putc(' ');
}

