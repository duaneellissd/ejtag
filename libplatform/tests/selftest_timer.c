#include "ejtag/platform/platform.h"
#include "ejtag/platform/debug.h"
#include "ejtag/platform/time.h"


void selftest_timer(void)
{
    intptr_t token;
    uint32_t tstart, tend;
    uint32_t etime;

    DEBUG_printf("timer-test-part1\n");
    tstart = TIME_mSecsNow();
    TIMER_sleep(1000);
    tend = TIME_mSecsNow();
    etime = (tend - tstart);

    if ((etime > 900) && (etime < 1100)) {
        DEBUG_printf("part1: SUCCESS\n");
    }
    else {
        DEBUG_assertionFails(__FILE__, __LINE__, "FAIL: bad time value");
    }

    DEBUG_printf("timer-test-part2\n");
    token = TIMER_start();
    tstart = TIME_mSecsNow();
    while (!TIMER_isExpired(token, 1000)) {
        /* we do nothing */
    }
    tend = TIME_mSecsNow();
    etime = (tend - tstart);
    if ((etime > 900) && (etime < 1100)) {
        /* this is good enough */
        DEBUG_printf("SUCCESS\n");
    }
    else {
        DEBUG_assertionFails(__FILE__, __LINE__, "FAIL: bad time value");
    }
    DEBUG_printf("SUCCESS\n");
}


#if defined(EJTAG_PLATFORM_IS_HOST)
int main(int argc, char **argv)
{
    return application_main(argc, argv, selftest_timer);
}
#endif
