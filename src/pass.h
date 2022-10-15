#ifndef pass_h

#define pass_h

#include "main.h"
#include "struct.h"

/*This file includes all functions from the  file passes*/

/*********** Pass one fuctions ***********/
void passOne(FILE *fp);
void line_pass_one(char *line);
int handle_guidance(int guidance_type, char *line); /* This function handles guidances (.data, .string, .entry, .extern) and sends them to the correct functions for analizing */
int handle_data_guidance(char *line);               /* This function handels .data guidance and encodes it to data */
int handle_string_guidance(char *line);             /* This function handles a .string guidance and encoding it to data */
int handle_extern_guidance(char *line);             /* This function handles an .extern guidance */
int handle_command(int type, char *line);           /* This function handels commands and encodes to words */
void write_num_to_data(int num);                    /* This function inserts .data to data memory */
int detect_method(char *operand);                   /* This function tries to find the addressing method of a given operand and returns -1 if it was not found */

/* This function checks for the validity of given methods according to the opcode */
boolean command_accept_num_operands(int command_type, boolean first, boolean second);
/* This function checks for the validity of given addressing methods according to the opcode */
boolean command_accept_methods(int command_type, int first_method, int second_method);
/* This function encodes the first word of the command */
unsigned int build_first_word(int command_type, int is_first, int is_second, int first_method, int second_method, int first_register, int second_register);
/* This function encodes the second word of the command */
unsigned int build_second_word(int command_type, int is_first, int is_second, int first_method, int second_method, int first_register, int second_register);
/* This function calculates number of additional words for a command */
int calculate_command_num_additional_words(int is_first, int is_second, int first_method, int second_method);

int find_new_type(int type);          /* Finds the real number of the opcode to insert in word */
int command_funct(int commands);      /* Finds the function number to insert in word */
unsigned int bit_by_index(int index); /* Turn on the bit in the given index (5 means 2^5=32) */
int num_words(int method);            /* This function returns how many additional words an addressing method requires */

/*********** Pass one fuctions ***********/
void passTwo(FILE *fp, char *filename);            /* Starting function of pass two */
void line_pass_two(char *line);                    /* The function that reads each line and checks it */
int handle_command_pass_two(int type, char *line); /* This function handles commands for the second pass - encoding additional words */
/* Checks if src and dest exists by opcode */
void does_operand_exists(int ope, boolean *is_source, boolean *is_destination);
/* This function encodes the additional words of the operands to instructions memory */
int encode_additional_words(char *src, char *dest, boolean is_src, boolean is_dest, int src_method, int dest_method);
/* This function encodes an additional word to instructions memory, given the addressing method */
void encode_additional_word(boolean is_dest, int method, char *operand);
void encode_num_immediate(char *operand);  /* Encodes an imidiate method word */
void encode_label(char *label);            /* This function encodes a given label (by name) to memory */
void encode_label_index(char *label);      /* Encodes an index method words */
int create_output_files(char *name);       /* Creating the output files */
FILE *open_file(char *filename, int type); /* Opens the file with the correct permissions */
void create_object_file(FILE *fp);         /* Creating ob file */
void create_entry_file(FILE *fp);          /* Creating entry file */
void create_external_file(FILE *fp);       /* Creating extern file */

void print_address_and_word(FILE *fp, unsigned int address, unsigned int word); /* The required print function for this project - "0100 A4-B0-C0-D0-E2*/

#endif