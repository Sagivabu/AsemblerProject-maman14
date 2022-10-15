#ifndef common_H

#define common_H

/******************************************************
 * This file includes common functions in the project *
 ******************************************************/

#include "main.h"
#include "struct.h"

/***** Functions from common.c *****/

/* Functions for main */
char *create_file_name(char *original, int type); /* Return full name with proper endin: original+type (.am/.ob/.as/.ex/.en) */

/* Functions for passes */
int ignore_line(char *line);                          /* Checks if the line is empty or needs to be ignored (;) */
char *skip_spaces(char *ch);                          /* This function skips spaces (blanks)*/
int if_error();                                       /* This function check if the 'error' flag was changed meaning that an error while reading the line occured */
void write_error(int line_number);                    /* print the error and the line number */
int end_of_line(char *line);                          /* Checking if end of line reached */
void copy_sign(char *destination, char *line);        /* Copies the next word from line to destination */
char *next_sign(char *seq);                           /* This function gets the next sign in line */
boolean is_label(char *sign, int colon);              /* This function checks if a sign is a label or not */
int find_index(char *sign, const char *arr[], int n); /* This function finds an index of a string in an array of strings */
int find_command(char *sign);                         /* Check if a sign matches a command name */
boolean is_register(char *sign);                      /* Check if a token matches a register name */
int find_guidance(char *sign);                        /* Check if a token matches a directive name */
char *next_list_sign(char *dest, char *line);         /* Copies the next sign to the destination */
boolean is_number(char *seq);                         /* Checks if number, also with +/- signs */
char *next_sign_string(char *dest, char *line);       /* Copies next string into destination */
boolean is_string(char *string);                      /* This function checks if a given sequence is a valid string (wrapped with "") */
void write_string_to_data(char *str);                 /* This function encodes a given string to data */
void copy_sign(char *destination, char *line);        /* Copies the next word from line to destination */
int find_reg_number(char *sign);                      /* Finds register number */
int find_reg_number_in_index(char *sign);             /* Finds register number in method_index */
void encode_to_instructions(unsigned int word);       /* This function inserts a given word to instructions memory */
/* TRUE if it is following Index Adressing method rules */
boolean is_label_index_method(char *operand, char **label_name, char **register_name);
unsigned int extract_bits(unsigned int word, int start, int end); /* This function extracts bits, given start and end positions of the bit-sequence (0 is LSB) */
unsigned int degrade_to_20_bits(unsigned int word);               /* This functions degrades the 32 bit word incase it has 1'ns as MSB to a 20 bit word for correct output */

/* Functions for struct.c */

/* Add label to labelList */
labelPtr add_label(labelPtr *lptr, char *name, unsigned int address, boolean external, ...);
boolean is_existing_label(labelPtr lbl, char *name);          /* This function checks if the label already exists */
labelPtr get_label(labelPtr lbl, char *name);                 /* get label from list by name*/
int delete_label(labelPtr *lptr, char *name);                 /* This function delets label in the label list in case we entered a wrong label */
void offset_address(labelPtr lptr, int num, boolean is_data); /* This functions sets the correct address for the label (memory starts at 100) */
int make_entry(labelPtr l, char *name);                       /* This function sets the bool var of an existing entry to true */
unsigned int get_label_address(labelPtr l, char *name);       /* This function gets the label address */
boolean is_external_label(labelPtr l, char *name);            /* This function checks if the label is external */
extPtr get_ext_label(extPtr xlbl, char *name);                /* This function gets the label for the extLabel table */
void free_label_table(labelPtr *lptr);                        /* This function frees the label list */

/* Functions for externalStruct.c */
extPtr add_ext(extPtr *lptr, char *name, unsigned int reference); /* This function adds a node to the end of the list */
void free_ext(extPtr *lptr);                                      /* This function frees the allocated memory for the list */

#endif