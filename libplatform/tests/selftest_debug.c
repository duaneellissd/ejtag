#include "ejtag/platform/platform.h"
#include "ejtag/platform/debug.h"
#include "ejtag/platform/string.h"

static uint8_t dumpdata[256];

void selftest_debug(void)
{
	int x;
	const char *cp;
	cp = "hello world";
	DEBUG_writeRaw(cp, strlen(cp));
	DEBUG_writeRaw("\r\n", 2);
	DEBUG_puts_no_nl(cp);
	DEBUG_newline();
	DEBUG_puts_no_nl(cp);
	DEBUG_putc('\n');
	DEBUG_puts(cp);
	DEBUG_str_hex8("value1", 0x12);
	DEBUG_str_hex16("value2", 0x1234);
	DEBUG_str_hex32("value3", 0x12345678);
	DEBUG_str_hex64("value4", 0x123456789abcdef0ULL);
	DEBUG_str_int("value5", 12345678);
	DEBUG_str_int("size-intptr", sizeof(intptr_t));
	intptr_t vip = 0x1234;
	DEBUG_str_int("value6", vip);

	for (x = 0; x < 256; x++) {
		dumpdata[x] = x;
	}
	DEBUG_puts("dump32");
	DEBUG_memdump32(0, (void *)(dumpdata), 256);
	DEBUG_puts("dump64");
	DEBUG_memdump64(0, (void *)(dumpdata), 256);

	for (x = 0; x < 16 ; x++) {
		DEBUG_str_int("dump-1-offset", x);
		DEBUG_memdump32(x, dumpdata+x, 1);
	}

	for (x = 0; x < 16; x++) {
		DEBUG_str_int("dump-4-offset", x);
		DEBUG_memdump32(x, dumpdata+x, 4);
	}

	for (x = 0; x < 16; x++) {
		DEBUG_str_int("dump-5-offset", x);
		DEBUG_memdump32(x, dumpdata+x, 32 + x );
	}
}

#if defined(EJTAG_PLATFORM_IS_HOST)
int main(int argc, char **argv)
{
	return application_main(argc, argv, selftest_debug);
}
#endif
