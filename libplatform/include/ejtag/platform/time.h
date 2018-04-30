#if !defined(EJTAG_PLATFORM_TIME_H)
#define EJTAG_PLATFORM_TIME_H
#include "ejtag/platform/config.h"

/**
 * PLATFORM: Initialize the platform specific time service.
 *
 * Effectively, this should set a global-ish 64 or 32bit variable
 * with the current time in Milliseconds 
 * See TIME_mSecsAbs() below.
 *
 * On windows, you can use GetTickCount()
 * On posix platforms, use gettimeofday()
 *
 * On embedded hosts, you should configure a hardware timer.
 *
 * For sake of discussion (below) name this variable: "start_time"
 */
EXTERN_C void TIME_initialize(void);


/**
 * PLATFORM: Return the number of milliseconds since "boot" of this application.
 *
 * On an embedded host - return the value of the hardware timer above.
 *
 * On windows, or Linux, get the current time of day
 * and subtract the value from the "start_time" set during initialization.
 */
 
EXTERN_C uint32_t TIME_mSecsNow(void);

/**
 * PLATFORM: Return the current absolute time in milliseconds.
 *
 * On Linux/Windows host you can use gettimeofday()
 * On embedded hosts, just return the same value as TIME_mSecsNow()
 */
EXTERN_C uint64_t TIME_mSecsAbs(void);

/** Start a light weight timer 
 *
 * The return value is a token used to mark the start of the period.
 */
intptr_t TIMER_start(void);

/** Return the elapsed time (in milliseconds) since the token */
int TIMER_mSecsElapsed(intptr_t token);

/** Time remaining */
int TIMER_mSecsRemain(intptr_t token, int timeoutperiod );

/** Return true if the timeout has expired */
int TIMER_isExpired(intptr_t token, int timeoutperiod);

/** PLATFORM: sleep for at least n mSecs */
void TIMER_sleep(uint32_t mSecs);

#endif

