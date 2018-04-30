#if !defined(EJTAG_PLATFORM_COMPILER_H)
/* DO NOT: #define EJTAG_PLATFORM_COMPILER_H - 
 * instead do this in the compiler specific step 
 */


#if defined(_MSC_VER) /* microsoft visual studio */
#define EJTAG_PLATFORM_COMPILER_H
#include "ejtag/platform/compiler_msft.h"
#endif


#if !defined(EJTAG_PLATFORM_COMPILER_H)
/* One of the above should have found the compiler we are using */
#error "Cannot determine compiler"
#endif

#endif
