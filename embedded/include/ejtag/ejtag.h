#if !defined(EJTAG_EJTAG_H)
#define EJTAG_EJTAG_H

#include <stdint.h>
#include <setjmp.h>

class EjtagInstance;

#define EJTAG_ASSERT( X )  EJTAG_ASSERT2( (X), #X )
#define EJTAG_ASSERT2(X,MSG)  if( !(X) ){ dbg.assertFails( __FILE__, __LINE__, MSG ); }

#include "ejtag/ejtag_buffer.h"
#include "ejtag/ejtag_platform.h"
#include "ejtag/ejtag_debug.h"
#include "ejtag/ejtag_cmds.h"

class EjtagInstance {
public:
	EjtagInstance(void);
	EjtagCommandProcessor mCmdProcessor;
	EjtagPlatform         mPlatform;

	void do_events(void);
	void disconnect(void);

	uint64_t membuf[ 128 / sizeof(uint64_t) ];
};

#endif