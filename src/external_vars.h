#ifndef external_vars_h

#define external_vars_h

#include "main.h"
#include "struct.h"

/* Create and declare on extern variables to use in all files */
extern int ic, dc;
extern int error;
extern boolean error_exist;
extern labelPtr symbols_table;
extern extPtr ext_list;
extern const char *commands[];
extern const char *guidance[];
extern boolean entry_exists, extern_exists;

/* Arrays for storing the data and words */
extern unsigned int data[];
extern unsigned int instructions[];

#endif