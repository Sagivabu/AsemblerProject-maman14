#ifndef macroDeploy_h

#define macroDeploy_h

#include "main.h"
#include "struct.h"
#include "common.h"
#include "external_vars.h"
#include "pass.h"

typedef struct macroNode
{
    char *name;
    char line[MACRO_MAX_LINES][MAX_INPUT];
    struct macroNode *next;
} macroNode;

FILE *macroDeployment(FILE *fp, char *newFileName); /* returns new file after macro deployment*/
void addNode(struct macroNode **, char *);          /* Add new node of macro to the list*/
void freeList(struct macroNode *);                  /* Free list of macro*/
macroNode *isMacro(char *, macroNode *);            /* TRUE if received token is a call for a macro */
boolean valid_macro_name(char *);                   /* This function checks if a sign is a label or not */
void pasteMacro(FILE *, macroNode *, int);          /* Paste the macro's lines in the new file*/
char *strdup(char *);                               /* Duplicate and return a new allocated string */
boolean isLastWord(char *, char *);                 /* Is there extra text after givne word */
/*print label and macro at the same line (merge between them)  */
void print_label_then_macro(FILE *nfp, char *label_name, macroNode *currentMacro, char *src_line);

#endif