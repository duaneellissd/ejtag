#include "ejtag/ejtag.h"
#include <string.h>

EjtagBuffer::EjtagBuffer(uint8_t *pBuf, size_t len) :
	mBuffer(pBuf),
	mLen(len),
	mDataEnd(0),
	mDataStart(0)
{
	mBuffer = pBuf;
	mLen = len;
	mDataEnd = 0;
	mDataStart = 0;
}

void EjtagBuffer::reset(void)
{
	mDataEnd = 0;
	mDataStart = 0;
}

void EjtagBuffer::normalize(void)
{
	size_t n;

	/* is it already normalized? */
	if (this->mDataStart == 0) {
		/* then leave */
		return;
	}

	/* how much are we moving? */
	n = this->bytesAvail();
	if (n)
	{
		/* move it */
		memmove(mBuffer, removePointer(), n);
	}
	this->mDataStart = 0;
	this->mDataEnd = n;
}

size_t EjtagBuffer::bytesAvail(void)
{
	return mDataEnd - mDataStart;
}

size_t EjtagBuffer::spaceAvail(void)
{
	return mLen - mDataEnd;
}

void *EjtagBuffer::insertPointer(void)
{
	return mBuffer + mDataEnd;
}

void *EjtagBuffer::removePointer(void)
{
	return mBuffer + mDataStart;
}


void EjtagBuffer::insertData(const uint8_t *pBuf, size_t nbytes)
{
	if (nbytes) {
		EJTAG_ASSERT(spaceAvail() >= nbytes);
		memcpy(insertPointer(), pBuf, nbytes);
		mDataEnd += nbytes;

	}
}

void EjtagBuffer::skipWhiteSpace(void)
{
	char *cpStart;
	int c;
	int n;
	int x;


	cpStart = (char *)removePointer();
	n = bytesAvail();

	for( x = 0 ; x < n ; x++ ){
		c = cpStart[x];
		if ((c == ' ') || (c == '\t')) {
			continue;
		}
		else {
			break;
		}
	}
	removeData(x);
}

void EjtagBuffer::insertByte(int c)
{
	uint8_t *p8;
	EJTAG_ASSERT(spaceAvail() > 0);

	p8 = (uint8_t *)(insertPointer());
	p8[0] = 0;
	mDataEnd += 1;
}

void EjtagBuffer::removeData(size_t n)
{
	mDataStart += n;
	EJTAG_ASSERT(mDataStart <= mDataEnd);
	EJTAG_ASSERT(mDataEnd <= mLen);
}

int EjtagBuffer::findEndOfLine(void)
{
	char *s;
	char *cr;
	char *lf;
	int n;
	int c;
	int r;

	/* move the string to the front */
	normalize();

	/* use memchr it is faster then we can do this */
	s = (char *)(removePointer());

	/* memchr needs a length */
	n = bytesAvail();

	/* found our cr or lf */
	cr = (char *)memchr(s, '\r', n);
	lf = (char *)memchr(s, '\n', n);

	/* which did we find, 4 cases: 0,1,2,3 */
	c = 0;
	c += (cr == NULL) ? 0 : 1;
	c += (lf == NULL) ? 0 : 2;

	switch (c) {
	case 0:
		/* not found */
		if (n == getLength()) {
			/* we have overflowed */
			dbg.printf(EjtagDebug::DbgArea::IoBuffers, "overflow\n");
			/* remove all the data */
			removeData(n);
		}
		r = -1;
		break;
	case 1: /* cr found, no lf */
		*cr = 0;
		/* return length of string */
		r = (cr - s);
		break;
	case 2: /* lf found, no cr */
		*lf = 0;
		r = (lf - s);
		break;
	case 3:
		/* both found = make CR the first of the two */
		if (lf < cr) {
			cr = lf;
		}
		*cr = 0;
		r = (cr - s);
	}
	return r;
}
