#include "ejtag/platform/config.h"

/** 
 * \defgroup DEBUG Debug Functions
 * @{
 */

EXTERN_C_BEGIN ;

/* Debug Print routines, simple and complex
 *
 * These must be provided by the actual physical platform
 */

/** PLATFORM: Perform any initialization required */
void DEBUG_initialize(void);

/** PLATFORM: Return true if a key has been hit */
int DEBUG_kbhit(void);

/** PLATFORM: Return the key that is pressed
 *
 * Returns POSITIVE number (0x00 ... N) for various keys
 * 
 * Returns EOF - if no key is waiting
 */
int DEBUG_getKey(void);

/**
 * Wait forever for a key
 */
int DEBUG_getKeyBlocking(void);

/** PLATFORM: Write count bytes to the output blocking, does not perform cr/lf */
int DEBUG_writeRaw( const char *bytesToWrite, size_t count );

/** PLATFORM: Die with a horrible message, must not return */
void DEBUG_assertionFails( const char *filename, int lineno, const char *msg );



/** print run-time now in secs & milliseconds
 *
 *  Effectively this:
 *
 *  uint32_t time_now = TIME_mSecsNow();
 *
 *  DEBUG_printf( "%4d.%03d | ", time_now / 1000, time_now % 1000 );
 */
 
void DEBUG_printNow(void);

/** Write a single ASCII byte to the console, does not perform cr/lf mapping */
void DEBUG_putc(int c);

/** identical to "man 3 puts"
 *
 * terminates with a new line, returns number of bytes written */
int DEBUG_puts(const char *s);

/** like "man 3 puts" but without the new line
 * returns number of bytes written */
int DEBUG_puts_no_nl(const char *s);

/** Write N bytes, performing CR/LF as needed
 * returns number of bytes written */
int DEBUG_write(const char *s, size_t count);

/** prints an ASCII bar, "|" */
void DEBUG_bar(void);

/** prints an ASCII hyphen, "-" */
void DEBUG_hyphen(void);

/** prints a single space, " " */
void DEBUG_space(void);

/** prints a colon followed by a space, ": " */
void DEBUG_colon_space(void);

/** prints a string a colon and a space, ie: printf("%s: ", str ); */
void DEBUG_str_colon_space( const char *s );

/** prints a string a colon and a space, and a 0x prefix ie: printf("%s: 0x", str ); */
void DEBUG_str_colon_spaceX(const char *s);

/** prints a single colon char */
void DEBUG_colon(void);

/** Prints a newline (or cr/lf if needed) */
void DEBUG_newline(void);

/** print 2 hex digits */
void DEBUG_hex8(uint32_t value);

/** print 4 hex digits */
void DEBUG_hex16(uint32_t value);

/** print 8 hex digits */
void DEBUG_hex32(uint32_t value);

/** print 16 hex digits */
void DEBUG_hex64(uint64_t value);

/** Print an intptr_t as hex */
void DEBUG_intptr(intptr_t value);

/** prints an integer */
void DEBUG_int(int integer);

/** prints a string label, followed by an integer value
 *
 * much like:  printf("%s: %d\n", str, value );
 * does not use printf()
 */
void DEBUG_str_int( const char *str, int value );

/** prints a string label, followed by an integer value
 *
 * much like:  printf("%s: %02x\n", str, value );
 * does not use printf()
 */
void DEBUG_str_hex8(const char *str, uint32_t value);


/** prints a string label, followed by an integer value
 *
 * much like:  printf("%s: %04x\n", str, value );
 * does not use printf()
 */

void DEBUG_str_hex16(const char *str, uint32_t value);

/** prints a string label, followed by an integer value
 *
 * much like:  printf("%s: %08x\n", str, value );
 * does not use printf()
 */


void DEBUG_str_hex32(const char *str, uint32_t value);

/** prints a string label, followed by an integer value
 *
 * much like:  printf("%s: %016x\n", str, value );
 * does not use printf()
 */
void DEBUG_str_hex64(const char *str, uint64_t value);

/** printf(), to the platform specific debug output
 * Note the return value is the number of bytes transmitted
 * The return value does not account for cr/lf mapping.
 * meaning: The format specifier may contain: "\n" the return
 * value would be N+1
 */
int DEBUG_printf(const char *fmt, ...);

/** vprintf() to the platform specific debug output */
int DEBUG_vprintf(const char *fmt, va_list ap);

int DEBUG_snprintf( char *buf, size_t bufsiz, const char *fmt, ... );

int DEBUG_vsnprintf( char *buf, size_t bufsiz, const char *fmt, va_list ap );

/** 
 * ASCII memory dump data structure.
 * For more details see:
 *  - \ref DEBUG_memdump32 
 *  - \ref DEBUG_memdump64
 *  - \ref DEBUG_memdump_line
 */
struct debug_memdump_line {
    /** Address to print in column */
    intptr_t address;

    /* to print a 32bit address, specify 4, to print 64bit specify 8 */
    size_t   sizeof_address;

    /** The data to print */
    const void *data;

    /** How many bytes to print */
    size_t n;

    /** the generated ASCII text is put here */
    char buffer[128];

    /** for internal use */
    int  buffer_idx;
};

/**
 * Memory Hex Dump, with a 32bit address field
 *
 * \param address - the address to print
 * \param data    - data pointer
 * \param n       - number of bytes to print.
 *
 * Also see \ref DEBUG_memdump64, and/or \ref debug_memdump_info
 */
void DEBUG_memdump32(intptr_t address, const void *data, size_t n);

/**
* Memory Hex Dump, with a 32bit address field
*
* \param address - the address to print
* \param data    - data pointer
* \param n       - number of bytes to print.
*
* Also see \ref DEBUG_memdump64, and/or \ref debug_memdump_info
*/
void DEBUG_memdump64(intptr_t address, const void *data, size_t n);

/**
 * Workhorse for \ref DEBUG_memdump32 and \ref DEBUG_memdump64
 *
 * \param pInfo data structure with printing details.
 *
 * Set the required elements and call this to produce
 * the line of ASCII text to print.
 *
 * Upon return, the values in the structure are updated
 * to effectively "point at" the next line to format.
 *
 * NOTE: When printing with a non 16byte aligned starting address
 * you may need to call this function more then once.
 */
void DEBUG_memdump_line(struct debug_memdump_line *pInfo);


#define DEBUG_KEY_UARROW	0x0100
#define DEBUG_KEY_DARROW	0x0101
#define DEBUG_KEY_LARROW	0x0102
#define DEBUG_KEY_RARROW	0x0103
#define DEBUG_KEY_PGUP		0x0104
#define DEBUG_KEY_PGDN		0x0105
#define DEBUG_KEY_HOME		0x0106
#define DEBUG_KEY_END		0x0107
#define DEBUG_KEY_INSERT	0x0108
#define DEBUG_KEY_FUNCbase  0x0180
#define DEBUG_KEY_F1	    (DEBUG_KEY_FUNCbase +  1)
#define DEBUG_KEY_F2	    (DEBUG_KEY_FUNCbase +  2)
#define DEBUG_KEY_F3	    (DEBUG_KEY_FUNCbase +  3)
#define DEBUG_KEY_F4	    (DEBUG_KEY_FUNCbase +  4)
#define DEBUG_KEY_F5	    (DEBUG_KEY_FUNCbase +  5)
#define DEBUG_KEY_F6	    (DEBUG_KEY_FUNCbase +  6)
#define DEBUG_KEY_F7	    (DEBUG_KEY_FUNCbase +  7)
#define DEBUG_KEY_F8	    (DEBUG_KEY_FUNCbase +  8)
#define DEBUG_KEY_F9	    (DEBUG_KEY_FUNCbase +  9)
#define DEBUG_KEY_F10       (DEBUG_KEY_FUNCbase + 10)
#define DEBUG_KEY_F11       (DEBUG_KEY_FUNCbase + 11)
#define DEBUG_KEY_F12       (DEBUG_KEY_FUNCbase + 12)

void selftest_debug(void);

/**
 * This structure is used for self-test reasons.
 * It provides a simple command line feature using
 * the DEBUG console.
 */
#define DEBUG_MAX_PARAMS 10
struct dbg_cmdline {
    const char *prompt;
    char buf[128];
    int argc;
    struct {
        int is_number;
        int value_int;
        char *value_cp;
    } argv[DEBUG_MAX_PARAMS];
    int expecting_lf;
    intptr_t magic_value;
};

/**
 * Read a line from the debug console.
 */
void DEBUG_readline(struct dbg_cmdline *pCmdLine);

/** @} */

EXTERN_C_END;

