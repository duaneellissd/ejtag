#include "ejtag/ejtag.h"
#include <string.h>

EjtagCmdInfo::EjtagCmdInfo(EjtagInstance *pInstance) :
	mCmdEntry(NULL),
	mInstance(pInstance),
	mState(EjtagCmdInfo::Disconnect),
	mLastBytesAvail(0),
	mRxBuffer(NULL,0)
{
	reset();
	/* we are disconnected */
	mState = EjtagCmdInfo::Disconnect;
}

void EjtagCmdInfo::reset(void)
{
	mCmdEntry = NULL;
	mState = EjtagCmdInfo::Start;
	memset((void *)(&mCmdMemory[0]), 0, sizeof(mCmdMemory));
	mRxExpected = 0;
	mRxBuffer.reset();
}

void EjtagCommandProcessor::disconnect(void)
{
	mCurCmd.mState = EjtagCmdInfo::Disconnect;
	if (mCurCmd.mCmdEntry) {
		mCurCmd.mCmdEntry->cmd_callback(&(mCurCmd));
		mCurCmd.mCmdEntry = NULL;
	}

	mMode = EjtagCommandProcessor::unknown;
}


EjtagCommandProcessor::EjtagCommandProcessor(EjtagInstance *pInstance):
	mInstance(pInstance),
	mCurCmd(pInstance),
	mMode(EjtagCommandProcessor::unknown)
{
	/* nothing to do */
}


void EjtagCommandProcessor::parseCmd(void)
{
	switch (mMode) {
	case EjtagCommandProcessor::unknown:
		EjtagCommandProcessor::parseCmd_unknown();
		break;
	case EjtagCommandProcessor::ascii:
		EjtagCommandProcessor::parseCmd_ascii();
		break;
	case EjtagCommandProcessor::xilinx:
		EjtagCommandProcessor::parseCmd_xilinx();
		break;
	case EjtagCommandProcessor::gdb:
		EjtagCommandProcessor::parseCmd_gdb();
		break;
	}
}


static void finish_binary(EjtagCmdInfo *pCmdInfo)
{
	EJTAG_ASSERT(pCmdInfo->mRxExpected != 0);
	EjtagBuffer *pRx;

	pRx = &(pCmdInfo->mInstance->mPlatform.rxBuffer);

	/* give the cmd the entire buffer */
	pCmdInfo->mRxBuffer = *pRx;
	pCmdInfo->mCmdEntry->cmd_callback(pCmdInfo);

	/* how many bytes where used? */
	int a, b, n;
	a = pRx->bytesAvail();
	b = pCmdInfo->mRxBuffer.bytesAvail();
	/* the command should have used some bytes */
	n = a - b;
	/* remove them from the rx buffer */
	if (n) {
		/* we took data out so we need to parse again */
		pCmdInfo->mLastBytesAvail = -1;
		pRx->removeData(n);
	}
	else {
		/* did not process any data */
		pCmdInfo->mLastBytesAvail = a;
	}
}

static void read_full_line(EjtagCmdInfo *pCmdInfo)
{
	/* our goal is to read a full line of text */
	int eol;
	char *colon;
	char *cmd;
	EjtagBuffer *pRx;

	pRx = &(pCmdInfo->mInstance->mPlatform.rxBuffer);

	eol = pRx->findEndOfLine();
	if (eol < 0) {
		return;
	}

	/* we have a whole line, and find made it null terminated */
	
	cmd = (char *)pRx->removePointer();

	colon = strchr(cmd, ':');
	if (colon == NULL) {
		dbg.printf(EjtagDebug::DbgArea::Errors, "missing colon\n");
		/* remove the offending line */
		pRx->removeData(eol + 1);
		pRx->normalize();
		return;
	}

	*colon = 0;

	/* FIND the command */
	const struct cmd_table_entry *pCe;
	for (pCe = cmd_table; pCe->cmd; pCe++) {
		if (0 == strcmp(pCe->cmd, cmd)) {
			break;
		}
	}

	if (pCe->cmd == NULL) {
		*colon = ':';
		dbg.printf(EjtagDebug::DbgArea::Errors, "unknown cmd: %s\n", cmd);
		pRx->removeData(eol + 1);
		pRx->normalize();

	/* look up the command */

	pRx = &(pCmdInfo->mInstance->mPlatform.rxBuffer);

	/* remove the command from the buffer */
	pRx->removeData(1 + (colon - cmd));

	pCmdInfo->mCmdEntry = pCe;
	pCmdInfo->mState = EjtagCmdInfo::CmdState::Start;
}


static int _unbase64(int c)
{
	// https://en.wikipedia.org/wiki/Base64
	if ((c >= 'A') && (c <= 'Z')) {
		return c - 'A';
	}
	if ((c >= 'a') && (c <= 'z')) {
		return c - 'a' + 26;
	}
	if ((c >= '0') && (c <= '9')) {
		return c - '0' + 52;
	}
	if (c == '+') {
		return 62;
	}
	if (c == '/') {
		return 63;
	}
	return -1;
}


static void decode_base64(EjtagCmdInfo *pCmdInfo)
{
	EjtagBuffer *pRx;
	char *cp;
	int n;
	int a, b, c;
	int b0, b1;
	int space;
	
	/* A challange we face here is memory 
	 * We may be receiving a megabyte of data via base64
	 * And we don't have a megabyte in the micro-controller
	 * based dongle, so we need to read the SINGLE LINE of
	 * text, process that single line of text... 
	 * then move on
	 */

	pRx = &(pCmdInfo->mInstance->mPlatform.rxBuffer);

	/* there can be leading white space */
	pRx->skipWhiteSpace();

	for (;;) {
		/* how full is our output buffer? */
		space = pCmdInfo->mRxBuffer.spaceAvail();
		if (space == 0) {
			/* we are full */
			return;
		}

		/* how much can we extract? */
		n = (int)pRx->bytesAvail();
		/* Base64 is always a multiple of 3 bytes */
		if (n < 3) {
			pRx->normalize();
			return;
		}

		cp = (char *)(pRx->removePointer());
		a = _unbase64(cp[0]);
		b = _unbase64(cp[1]);
		c = _unbase64(cp[2]);
		b0 = (a << 2) + (b >> 4);
		b1 = ((b & 3) << 6) | (c);


		pCmdInfo->mRxBuffer.insertByte(b0);
		if (c != -1)
		{
			pCmdInfo->mRxBuffer.insertByte(b1);
		}
		pRx->removeData(3);
	}
}

void EjtagCmdInfo::expectBinaryData(uint32_t nBytes)
{
	mRxExpected = nBytes;
	mState = EjtagCmdInfo::Binary;
}

void EjtagCmdInfo::expectBase64Data(uint32_t nBytes)
{
	mRxExpected = nBytes;
	mState = EjtagCmdInfo::Base64;
}

void EjtagCmdInfo::expectData(void)
{
	mRxExpected = 0;
	mState = EjtagCmdInfo::Data;
}

static void finish_base64(EjtagCmdInfo *pCmdInfo)
{
	/* we should be expecting data */
	EJTAG_ASSERT(pCmdInfo->mRxExpected != 0);

	if (!read_full_line(pCmdInfo)) {
		return;
	}

	/* decode data */
	decode_base64(pCmdInfo);

	pCmdInfo->mCmdEntry->cmd_callback(pCmdInfo);
}

static void start_cmd(EjtagCmdInfo *pCmdInfo)
{
	/* we need to read to the end of the line */

}

void EjtagCommandProcessor::parseCmd(void)
{
	/* what state are we in? */
	switch (mCurCmd.mState) {
	case EjtagCmdInfo::Binary:
		finish_binary(&mCurCmd);
		break;
	case EjtagCmdInfo::Base64:
		finish_base64(&mCurCmd);
		break;
	case EjtagCmdInfo::Start:
		start_cmd(&mCurCmd);
		break;
	}
}