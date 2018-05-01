#include "ejtag/platform/platform.h"
#include "ejtag/platform/debug.h"

static void _addchar(struct debug_memdump_line *pInfo, int c)
{
	pInfo->buffer[pInfo->buffer_idx + 0] = c;
	pInfo->buffer[pInfo->buffer_idx + 1] = 0;
	pInfo->buffer_idx += 1;
}

static void _addchar2(struct debug_memdump_line *pInfo, int c1, int c2)
{
	_addchar(pInfo, c1);
	_addchar(pInfo, c2);

}


static void _tohex(struct debug_memdump_line *pInfo, uint64_t value, size_t nbytes )
{
	int shiftamount = (nbytes * 8) - 4;
	int c;
	while (shiftamount >= 0) {
		uint64_t v2;
		v2 = value;
		v2 = v2 >> shiftamount;
		
		c = (int)(v2 & 0x0f);
		c = "0123456789abcdef"[c];
		_addchar(pInfo, c);
		shiftamount -= 4;
	}
}


void DEBUG_memdump_line(struct debug_memdump_line *pInfo)
{
	const uint8_t *p8;
	int c;
	int x;
	intptr_t offset_start;
	intptr_t offset_end;

	pInfo->buffer_idx = 0;
	pInfo->buffer[0] = 0;

	if (pInfo->n == 0) {
		return;
	}

	offset_start = pInfo->address;

	/* address field, normalized to 0 */
	_tohex(pInfo, offset_start & (~0x0f), pInfo->sizeof_address);

	/* determine what address on this line we start at */
	offset_start &= 0x0f;
	/*
	 * Normally this is at offset 0
	 */

	offset_end = offset_start + pInfo->n;

	/* where on this 16 byte line does the data end? */
	if (offset_end > 16) {
        offset_end = 16;
	}

	/* normalize our pInfo->address to a 16 byte boundary */
	pInfo->address -= offset_start;
	pInfo->data = CVOID_PTR_ADD(pInfo->data, -offset_start);
	pInfo->n += offset_start;

	/* 
	 * thus, our hex dump exists between these points:
	 * from pInfo->address[ offset_start ] and
	 *   to pInfo->address[ offset_end-1 ] inclusive.
	 */	

	/* point at our data as an 8bit pointer */
	p8 = (const uint8_t *)(pInfo->data);

	/* pass1 - space before vertical bar */
	_addchar(pInfo, ' ');

	/* HEX for each byte */
	for (x = 0; x < 16; x++) {
		/* are we at the hyphen in the center? */
		if (x == 8) {
			/* yes, insert the hyphen */
			_addchar(pInfo, '-');
		}

		/* do we have data at this position? */
		if ((x >= offset_start) && (x < offset_end)) {
			/* fetch the data */
			c = p8[x];
			_tohex(pInfo, c, 1);
		}
		else {
			_addchar2(pInfo, ' ', ' ');
		}
		switch (x) {
		case 7:
		case 15:
			/* do nothing */
			break;
		default:
			/* insert space between */
			_addchar(pInfo, ' ');
			break;
		}
	}

	_addchar(pInfo, ' ');
	_addchar(pInfo, '|');

	/* ASCII for each byte */
	for( x = 0 ; x < 16 ; x++ ){
		if (x == 8) {
			_addchar(pInfo, '-');
		}
		/* valid ?*/
		/* assume invalid */
		if ((x >= offset_start) && (x < offset_end)) {
			int tmp;
			tmp = p8[x];
			/* valid ASCII? */
			if ((tmp >= 0x20) && (tmp <= 0x7e)){
				/* then use it */
				c = tmp;
            }
            else {
                c = '.';
            }
        }
        else {
            c = ' ';
        }
		_addchar(pInfo, c);
	}
	_addchar(pInfo, '|');

	pInfo->address += 16;
    pInfo->data = CVOID_PTR_ADD(pInfo->data, 16);
    if (pInfo->n <= 16) {
        pInfo->n = 0;
    }
    else {
        pInfo->n -= 16;
    }
}


static void _memdump(
	intptr_t address, 
	const void *data, 
	size_t n, 
	size_t asize)
{
	struct debug_memdump_line info;

	info.address = address;
	info.sizeof_address = asize;
	info.data = data;
	info.n = n;

	while (info.n) {
		DEBUG_memdump_line(&info);
		DEBUG_puts(info.buffer);
	}
}

void DEBUG_memdump32(
	intptr_t address,
	const void *data,
	size_t n)
{
	_memdump(address, data, n, 4);
}

void DEBUG_memdump64(
	intptr_t address,
	const void *data,
	size_t n)
{
	_memdump(address, data, n, 8);
}
