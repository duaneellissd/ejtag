#if !defined( EJTAG_PLATFORM_COMPILER_MSFT_H )
#define EJTAG_PLATFORM_COMPILER_MSFT_H 

/* put stuff here */

/**
 * \defgroup MICROSOFT_COMPILER Microsoft Compiler Specific items targeting windows hosts
 */

/* all windows things require windows.h */
#include <windows.h>
#undef EJTAG_PLATFORM_IS_HOST
#define EJTAG_PLATFORM_IS_HOST 1


/** @} */

#endif
