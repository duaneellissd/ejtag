#include "ejtag/platform/platform.h"
#include "ejtag/platform/time.h"

intptr_t TIMER_start(void)
{
    return TIME_mSecsNow();
}

int TIMER_mSecsElapsed(intptr_t token)
{
    intptr_t now;

    now = TIME_mSecsNow();
    now = now - token;
    return (now);
}

int TIMER_mSecsRemain(intptr_t token, int timeoutperiod)
{
    int r;
    r = timeoutperiod - TIMER_mSecsElapsed(token);
    return r;
}

int TIMER_timerIsExpired(intptr_t token, int timeoutperiod)
{
    if (TIMER_mSecsRemain(token, timeoutperiod) <= 0) {
        return 1;
    }
    else {
        return 0;
    }
}


