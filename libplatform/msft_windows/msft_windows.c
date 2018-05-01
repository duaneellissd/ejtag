#include "ejtag/platform/platform.h"
#include "ejtag/platform/debug.h"
#include "ejtag/platform/time.h"
#include "ejtag/platform/string.h"
#include "ejtag/platform/ascii.h"
#include <Windows.h>
#include <stdio.h>
#include <conio.h>

static uint64_t timezero_64 = 0;

void TIME_initialize(void)
{
    TIME_mSecsNow();
}

uint32_t TIME_mSecsNow(void)
{
    uint64_t u64;
    
    u64 = TIME_mSecsAbs();

    if (timezero_64 == 0) {
        timezero_64 = u64;
    }

    u64 = u64 - timezero_64;
    return (uint32_t)(u64);
}

void TIMER_sleep(uint32_t nMsecs)
{
    intptr_t start;
    start = TIMER_start();
    Sleep(nMsecs);
    while (!TIMER_isExpired(start, nMsecs)) {
        Sleep(1);
    }
}

uint64_t TIME_mSecsAbs(void)
{
	return GetTickCount64();
}

void DEBUG_initialize(void)
{
    /* nothing to do here */
}

int DEBUG_writeRaw(const char *bytesToWrite, size_t count)
{
    fwrite((const void *)(bytesToWrite), sizeof(char), count, stdout);
    fflush(stdout);
    return ((int)(count));
}

int DEBUG_kbhit(void)
{
    return _kbhit();
}

struct keyxlate { int in; int out; };
static  const struct keyxlate keytable[] = {

    /* arrows */

    { .in = 0x013b,.out = DEBUG_KEY_F1 },
    { .in = 0x013c,.out = DEBUG_KEY_F2 },
    { .in = 0x013d,.out = DEBUG_KEY_F3 },
    { .in = 0x013e,.out = DEBUG_KEY_F4 },
    { .in = 0x013f,.out = DEBUG_KEY_F5 },
    { .in = 0x0140,.out = DEBUG_KEY_F6 },
    { .in = 0x0141,.out = DEBUG_KEY_F7 },
    { .in = 0x0142,.out = DEBUG_KEY_F8 },
    { .in = 0x0143,.out = DEBUG_KEY_F9 },
    { .in = 0x0144,.out = DEBUG_KEY_F10},
    { .in = 0x0285,.out = DEBUG_KEY_F11 },
    { .in = 0x0286,.out = DEBUG_KEY_F12 },

    /* independent arrow keys */
    { .in = 0x0248,.out = DEBUG_KEY_UARROW },
    { .in = 0x0250,.out = DEBUG_KEY_DARROW },
    { .in = 0x024b,.out = DEBUG_KEY_LARROW },
    { .in = 0x024d,.out = DEBUG_KEY_RARROW },

    /* above the keyboard */
    {.in = 0x0252,.out = DEBUG_KEY_INSERT },
    {.in = 0x0253,.out = ASCII_DEL },
    { .in = 0x0247,.out = DEBUG_KEY_HOME },
    { .in = 0x024f,.out = DEBUG_KEY_END },
    { .in = 0x0249,.out = DEBUG_KEY_PGUP },
    { .in = 0x0251,.out = DEBUG_KEY_PGDN },

    /* numeric keypad */
    { .in = 0x0147,.out = DEBUG_KEY_HOME },
    { .in = 0x0148,.out = DEBUG_KEY_UARROW },
    { .in = 0x0149,.out = DEBUG_KEY_PGUP },

    { .in = 0x014b,.out = DEBUG_KEY_RARROW },
    /* dead key keypad 5 */
    { .in = 0x014d,.out = DEBUG_KEY_LARROW },

    { .in = 0x014f,.out = DEBUG_KEY_END  },
    { .in = 0x0150,.out = DEBUG_KEY_DARROW },
    { .in = 0x0151,.out = DEBUG_KEY_PGDN },
 
    {.in = 0x0152,.out = DEBUG_KEY_INSERT },
    {.in = 0x0153,.out = ASCII_DEL },

    /* terminate */
    { .in = 0, .out = 0 }
};

int DEBUG_getKey(void)
{
    if (!_kbhit()) {
        return EOF;
    }
    return DEBUG_getKeyBlocking();
}

int DEBUG_getKeyBlocking(void)
{
    const struct keyxlate *pKXL;
    int key;

    key = _getch();

    /* map extended keys to our common cross platform key numbers */
    if (key == 0xe0) {
        key = _getch() + 0x0200;
    } else if(key == 0x00) {
        key = _getch() + 0x0100;
    }

    if (key < 0x100) {
        return key;
    }

    pKXL = keytable;
    while (pKXL->in != 0) {
        if (pKXL->in == key) {
            return pKXL->out;
        }
        pKXL++;
    }
    /* don't know it we ignore it */
    return EOF;
}


void DEBUG_assertionFails(const char *filename, int lineno, const char *msg)
{
    fprintf(stdout, "\n%s:%d: %s\nAssertion Fails\n", filename, lineno, msg);
    if (IsDebuggerPresent()) {
        DebugBreak();
    }
    exit(1);
}

int application_main(int argc, char **argv, void (*app_main)(void) )
{
    (void)(argc);
    (void)(argv);
    TIME_initialize();
    DEBUG_initialize();

    (*app_main)();
    return 0;
}
