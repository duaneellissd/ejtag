#include "ejtag/platform/config.h"


/** Windows/Linux/Mac applications call this from main().
 *
 * \param argc - command line argc
 * \apram argv - command line argv
 *
 * \param main application entry point.
 */
EXTERN_C int application_main(int argc, char **argv, void(*app_main)(void) );
