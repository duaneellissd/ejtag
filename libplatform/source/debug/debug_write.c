#include "ejtag/platform/platform.h"
#include "ejtag/platform/debug.h"
#include "ejtag/platform/time.h"
#include "ejtag/platform/string.h"

static int dbg_column;

void DEBUG_newline(void)
{
	DEBUG_writeRaw( "\r\n", 2);
	dbg_column = 0;
}

static void now_check(void)
{
#define NOWBUF_SIZE 32
	char buf[NOWBUF_SIZE];
	int x;
	uint32_t now;

	/* print now if at column 0 */
	if (dbg_column != 0) {
		return;
	}
	buf[NOWBUF_SIZE - 1] = 0;
	buf[NOWBUF_SIZE - 2] = ' ';
	buf[NOWBUF_SIZE - 3] = '|';
	buf[NOWBUF_SIZE - 4] = ' ';
	

	now = TIME_mSecsNow();
	buf[NOWBUF_SIZE - 5] = '0' + (now % 10);
	now = now / 10;
	buf[NOWBUF_SIZE - 6] = '0' + (now % 10);
	now = now / 10;
	buf[NOWBUF_SIZE - 7] = '0' + (now % 10);
	now = now / 10;
	buf[NOWBUF_SIZE - 8] = '.';

	x = NOWBUF_SIZE - 9;
	do {
		buf[x] = '0' + (now % 10);
		now /= 10;
		x--;
	} while (now)
		;

	switch (x) {
	case (NOWBUF_SIZE - 10):
		buf[x] = ' ';
		x--;
		/* fallthru; */
	case (NOWBUF_SIZE - 11):
		buf[x] = ' ';
		x--;
		/* fallthru; */
	case (NOWBUF_SIZE - 12):
		buf[x] = ' ';
		x--;
		/* fallthru; */
	case (NOWBUF_SIZE - 13):
		buf[x] = ' ';
		x--;
		/* fallthru; */
	default:
		/* do not space pad any more */
		break;
	}
	/* backup to the last byte written to the buffer */
	x++;

	dbg_column = NOWBUF_SIZE - x - 1;

	DEBUG_writeRaw( &buf[x], dbg_column );
}

void DEBUG_printNow(void)
{
	dbg_column = 0;
	now_check();
}

void DEBUG_putc(int c)
{
	now_check();
	if (c == '\n') {
		DEBUG_newline();
	}
	else {
		char buf[2];
		buf[0] = (char)(c);
		buf[1] = 0;
		DEBUG_writeRaw(buf, 1);
		dbg_column += 1;
	}
}

int DEBUG_write( const char *s, size_t n )
{
	int r;
	size_t n2;
	const char *cp;

	/* we return what we write, which is everything */
	/* the count does *NOT* include the cr/lf conversion */
	r = (int)(n);

	while(n > 0){
		/* look forward for a newline */
		cp = memchr(s, '\n', n);
		if (cp == NULL) {
			/* none, then we are done */
			break;
		}

		/* how many bytes until we find the newline? */
		n2 = (cp - s);
		if (n2) {
			/* write bytes prior to the newline */
			now_check();
			DEBUG_writeRaw(s, n2);
		}
		/* output the newline */
		DEBUG_newline();

		/* adjust pointer & count, and accounting for the newline */
		n2++;
		n = n - n2;
		s += n2;
	}
	if (n) {
		now_check();
		DEBUG_writeRaw(s, n);
	}
	return r;
}
