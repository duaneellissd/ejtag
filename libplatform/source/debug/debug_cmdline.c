
#include "ejtag/platform/platform.h"
#include "ejtag/platform/debug.h"
#include "ejtag/platform/ascii.h"
#include "ejtag/platform/string.h"
#include <stdlib.h>

static int const magic = 12345;

static int my_isdigit(int c)
{
    return (c >= '0') && (c <= '9');
}

static int my_isspace(int c)
{
    return (c == ' ') || (c == '\t');
}

static char *_parse_word(
    struct dbg_cmdline *pCmdLine,
    char *cp)
{
    int c;
    char *wr;
    char *rd;

    (void)(pCmdLine);

    wr = cp;
    rd = cp;

    for (;;) {
        c = *rd++;
        if (c == 0) {
            return NULL;
        }
        /* future: Fancy stuff, like \r becomes cr */
        if (my_isspace(c)) {
            *cp = 0;
            return cp + 1;
        }
        /* next char */
    }
}

static void _number_check(struct dbg_cmdline *pCmdLine)
{
    char *cp;
    char *ep;
    int value;

    cp = pCmdLine->argv[pCmdLine->argc - 1].value_cp;
    ep = cp;

    value = strtol(cp, &ep, 0);
        
    if ( (ep != cp) && (*ep != 0)) {
            /* not a number */
        return;
    }
    pCmdLine->argv[pCmdLine->argc - 1].value_int = value;
    pCmdLine->argv[pCmdLine->argc - 1].is_number = 1;
}

static void _parse(struct dbg_cmdline *pCmdLine)
{
    char *cp;
    int c;
    cp = pCmdLine->buf;
    for (;;) {
        if (cp == NULL) {
            /* DONE */
            break;
        }
        c = *cp;
        cp++;
        /* skip white space */
        if (my_isspace(c)) {
            continue;
        }
        /* we are only going to parse N items */
        if (pCmdLine->argc >= DEBUG_MAX_PARAMS) {
            break;
        }

        pCmdLine->argc += 1;
        pCmdLine->argv[pCmdLine->argc].value_cp = cp;
        if ((c == '"') || (c == '\'')) {
            /* FUTURE: parse quotes */
            // cp = _parse_quote(pCmdLine, cp, c);
            cp = _parse_word(pCmdLine, cp);
        }
        else {
            cp = _parse_word(pCmdLine, cp);
            _number_check(pCmdLine);
        }
    }
}

void DEBUG_readline(struct dbg_cmdline *pCmdLine)
{
    int x;
    int c;
    pCmdLine->argc = 0;
    memset((void *)(&(pCmdLine->argv[0])), 0, sizeof(pCmdLine->argv));
    memset((void *)(pCmdLine->buf), 0, sizeof(pCmdLine->buf));
    
    DEBUG_puts_no_nl(pCmdLine->prompt);

    if (pCmdLine->magic_value != ((intptr_t)(&magic)) ){
        pCmdLine->magic_value = ((intptr_t)(&magic));
        pCmdLine->expecting_lf = 0;
    }
    for (;;) {
        c = DEBUG_getKey();

        /* should we ignore the LF?
         * This happens when we have a serial interface
         * in test mode that transmits CR & LF
         *
         * The line parsing code terminates on the CR
         * The next byte received is a LF...
         * Which we should ignore
         */
        if (c == '\n') {
            /* Perhaps the host is only sending lf */
            if (pCmdLine->expecting_lf) {
                pCmdLine->expecting_lf = 0;
                continue;
            }
            /* treat as a cr */
            c = '\r';
        }
        if (c == '\r') {
            pCmdLine->expecting_lf = 1;
            _parse(pCmdLine);
            return;
        }

        if (c == ASCII_ESC) {
            for (;;) {
                x = strlen(pCmdLine->buf);
                if (x == 0) {
                    break;
                }
                DEBUG_puts_no_nl("\b \b");
                x--;
                pCmdLine->buf[x] = 0;
            }
            continue;
        }
        if (c == DEBUG_KEY_LARROW) {
            c = ASCII_BACK_SPACE;
        }
        if (c == ASCII_DEL) {
            c = ASCII_BACK_SPACE;
        }
        if (c == ASCII_BACK_SPACE) {
            x = strlen(pCmdLine->buf);
            /* how long is the text */
            if (x == 0) {
                /* beep if there is nothing */
                DEBUG_putc(ASCII_BEL);
                continue;
            }
            x--;
            DEBUG_puts_no_nl("\b \b");
            pCmdLine->buf[x] = 0;
            continue;
        }
        if ((c < 0x020) || (c >= 0x7f)) {
            /* non-ASCII */
            DEBUG_putc(ASCII_BEL);
            continue;
        }
        x = strlen(pCmdLine->buf);
        if (x > (sizeof(pCmdLine->buf) - 2)) {
            /* too many chars */
            DEBUG_putc(ASCII_BEL);
            continue;
        }
        pCmdLine->buf[x] = (char)c;
        x++;
        pCmdLine->buf[x] = 0;
        continue;
    }
}