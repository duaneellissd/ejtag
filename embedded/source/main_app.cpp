#include "ejtag/ejtag.h"

EjtagInstance::EjtagInstance(void) :
	mCmdProcessor(this),
	mPlatform(this)

{
	/* nothing to do here */
}


void EjtagInstance::do_events(void)
{
	/* if we are no longer connected, reset */
	if (!mPlatform.ioIsCoconnected()) {
		mCmdProcessor.disconnect();
		return;
	}

	/* if we can't send, we do nothing */
	if (mPlatform.txIsBusy()) {
		return;
	}

	size_t n;
	n = mPlatform.rxBuffer.bytesAvail();
	if (n == 0) {
		return;
	}
	if (n == mCmdProcessor.mCurCmd.mLastBytesAvail) {
		/* no change */
		return;
	}

	mCmdProcessor.parseCmd();
}