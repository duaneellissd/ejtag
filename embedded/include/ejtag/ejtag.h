#if !defined(EJTAG_EJTAG_H)
#define EJTAG_EJTAG_H

#include <stdint.h>
#include <setjmp.h>

namespace Ejtag {
	class Instance;
};

#define EJTAG_ASSERT( X )  EJTAG_ASSERT2( (X), #X )
#define EJTAG_ASSERT2(X,MSG)  if( !(X) ){ dbg.assertFails( __FILE__, __LINE__, MSG ); }

#include "ejtag/ejtag_platdefs.h"
#include "ejtag/ejtag_fifo.h"
#include "ejtag/ejtag_platform.h"
#include "ejtag/ejtag_debug.h"
#include "ejtag/ejtag_cmds.h"

namespace Ejtag {
	class Instance {
	public:
		Instance(void);
		CommandProcessor mCmdProcessor;
		Platform         mPlatform;

		void do_events(void);
		void disconnect(void);

		uint64_t membuf[128 / sizeof(uint64_t)];
	};

};

#endif