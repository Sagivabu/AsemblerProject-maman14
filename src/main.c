/* This is the main file of the program */

/* 206122459 Sagiv Abu */

#include "main.h"
#include "external_vars.h"
#include "pass.h"
#include "common.h"
#include "macroDeploy.h"

/* List of avaiable commands */
const char *commands[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop"};

/* List of avaiable guidance */
const char *guidance[] = {".data", ".string", ".entry", ".extern"};

/* Global variables (external_vars) */
unsigned int data[MAX_WORDS];         /* data image */
unsigned int instructions[MAX_WORDS]; /* Instrction image */
int ic;                               /* instructions counter */
int dc;                               /* data counter */
int error;                            /* to print errors */
labelPtr symbols_table;               /* The table of labels */
extPtr ext_list;                      /* The table of external labels */
boolean entry_exists, error_exist, extern_exists;

/* The fucnction that resets all flags for next file */
void reset_flags()
{
    symbols_table = NULL;
    ext_list = NULL;

    entry_exists = FALSE;
    extern_exists = FALSE;
    error_exist = FALSE;
}

int main(int argc, char *argv[])
{
    int i;
    char *fileName = NULL;
    FILE *fp, *nfp;
    if (argc < 2)
    {
        fprintf(stdout, "No argument found, please enter a file name to process\n");
        exit(1);
    }
    else
    {
        for (i = 1; i < argc; i++)
        {
            fileName = create_file_name(argv[i], FILE_INPUT); /* Uses the file name given in the command line */
            if ((fp = fopen(fileName, "r")) == NULL)
            { /* Open the file with reading permision */
                fprintf(stderr, "\nAn error occured while opening the file: %s\n", fileName);
            }
            else
            {
                if (feof(fp) == 1) /* Checks if EOF reached */
                    fprintf(stderr, "\nThe file is empty\n");
                else
                {
                    fprintf(stdout, "*******Started working on file: %s*******\n", fileName);

                    reset_flags();                                                  /* Resets all flags for the next file */
                    nfp = macroDeployment(fp, create_file_name(fileName, FILE_AM)); /* new file after macro deployment need to return file */
                    if (error == 0)
                    {
                        rewind(nfp); /*Starts the second pass from the start of the file*/
                        passOne(nfp);
                    }
                    if (error == 0) /* If there are no errors, continue with second pass */
                    {
                        rewind(nfp); /*Starts the second pass from the start of the file*/
                        passTwo(nfp, argv[i]);
                    }
                    else /* First pass contains errors, stop processing the file */
                    {
                        fprintf(stdout, "Errors found in file: %s, stopped working on file\n", fileName);
                    }
                    fprintf(stdout, "*******Finished working on file: %s*******\n", fileName);
                    fclose(nfp); /* Close file */
                }
            }
        }
    }
    return 0;
}