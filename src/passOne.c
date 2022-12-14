#include "main.h"
#include "external_vars.h"
#include "pass.h"
#include "common.h"

/* This file is the first pass of the programn */

/* Starting of pass one */
void passOne(FILE *fp)
{
    char line[MAX_INPUT]; /* This char array contains the current line we are handling */
    int line_number = 1;  /* Line number starts from 1 */

    /* Initializing data and instructions counter */
    ic = 0;
    dc = 0;

    while (fgets(line, MAX_INPUT, fp) != NULL) /* Read lines until end of file */
    {
        error = EMPTY_ERROR; /* Reset the error variable at the begining of a new line*/

        if (!ignore_line(line)) /* Ignore line if it's blank*/
            line_pass_one(line);
        if (if_error())
        {
            error_exist = TRUE;       /* There was at least one error through all the program */
            write_error(line_number); /* Print the error */
        }
        line_number++;
    }
    /* Calculates the correct address after the symbol table is completed */
    offset_address(symbols_table, DEFAULT_IC, FALSE);     /* Instruction symbols will have addresses that start from 100 (MEMORY_START) */
    offset_address(symbols_table, ic + DEFAULT_IC, TRUE); /* Data symbols will have addresses that start fron MEMORY_START + IC */
}

void line_pass_one(char *line)
{
    /* Initializing variables for the type of the guidance/command */
    int guidance_type = UNKNOWN_TYPE;
    int command_type = UNKNOWN_COMMAND;
    boolean label = FALSE;        /* This variable will hold TRUE if a label exists in this line */
    labelPtr label_node = NULL;   /* This variable holds optional label in case we need to create it */
    char current_sign[MAX_INPUT]; /* This string will hold the current sign if we analyze it */

    /* Beginning to parse a line */
    line = skip_spaces(line); /* Skip all spaces */
    if (end_of_line(line))
        return; /* Starts next row in case the row is empty */

    if ((isalpha(*line) == 0) && *line != '.') /* First non-blank character must be a letter or a dot */
    {
        error = INVALID_SYNTAX;
        return;
    }

    copy_sign(current_sign, line);     /* Taking the label from line and copy it to current_sign*/
    if (is_label(current_sign, COLON)) /* Test if the first word is a label */
    {
        label = TRUE;
        label_node = add_label(&symbols_table, current_sign, 100, FALSE, FALSE); /* Adding a new label to the Symbol table */
        if (label_node == NULL)                                                  /* In case we didnt succeed to add the label */
            return;
        line = next_sign(line); /* Getting the next sign */
        if (end_of_line(line))
        {
            error = INVALID_LABEL_LINE; /* Only label exists in line */
            return;
        }
        copy_sign(current_sign, line); /* After we get the next word or symbol we continue with the proccess */
    }

    if (if_error())
        return;
    /* In case the first pass for label search returns an error */
    if ((guidance_type = find_guidance(current_sign)) != NO_MATCH) /* In case the sign after the label is a guidance */
    {
        if (label == TRUE)
        {
            if (guidance_type == ENTRY || guidance_type == EXTERN)
            { /* If the guidance is extern or entry, delete it from label table */
                delete_label(&symbols_table, label_node->name);
                label = FALSE;
            }
            else
            {
                /* Setting fields accordingly in label */
                strcpy(label_node->symbol_type, "data");
            }
            label_node->address = dc; /* Address of data label is dc */
        }
        line = next_sign(line);
        handle_guidance(guidance_type, line); /* The function to handle all types of guidances */
    }

    else if ((command_type = find_command(current_sign)) != NO_MATCH) /* In case the sign is a command */
    {
        if (label == TRUE)
        {
            /* Setting fields accordingly in label */
            label_node->inActionStatement = TRUE;
            strcpy(label_node->symbol_type, "code");
            label_node->address = ic; /* Address of data label is ic */
        }
        line = next_sign(line);             /* line start now with the first operand after command name*/
        handle_command(command_type, line); /* The function to handle all types of commands */
    }
    else
        error = MISSING_SYNTAX; /* In case a line does not have a command or a guidance */
}

/* This function handles guidances (.data, .string, .entry, .extern) and sends them to the correct functions for analizing */
int handle_guidance(int guidance_type, char *line)
{

    if (line == NULL || end_of_line(line)) /* At least one parameter must follow a guidance */
    {
        error = NO_PARAMETER_AVAILABLE;
        return ERROR;
    }

    switch (guidance_type)
    {
    case DATA:
        /* Handle .data  and insert values separated by comma to the memory */
        return handle_data_guidance(line);

    case STRING:
        /* Handle .string guidance and insert all characters (including a '\0') to memory */
        return handle_string_guidance(line);

    case ENTRY:
        /* Only check for syntax of entry (should not contain more than one parameter) */
        if (!end_of_line(next_sign(line))) /* If there's a next sign (after the first one) */
        {
            error = GUIDANCE_INVALID_NUM_PARAMS;
            return ERROR;
        }
        break;

    case EXTERN:
        /* Handle .extern guidance */
        return handle_extern_guidance(line);
    }
    return EMPTY_ERROR;
}

/* This function handels .data guidance and encodes it to data */
int handle_data_guidance(char *line)
{
    char sign[NUM]; /* Holds sign */

    /* Flags to verify that there is a seperation between the diffrent numbers using a comma */
    boolean number = FALSE;
    boolean comma = FALSE;

    while (!end_of_line(line)) /* checks if end of line */
    {
        line = next_list_sign(sign, line); /* Getting current sign */

        if (strlen(sign) > 0)
        {
            if (number == FALSE) /* If there wasn't a number before */
            {
                if (is_number(sign) == FALSE) /* Then the sign must be a number */
                {
                    error = DATA_EXPECTED_NUM;
                    return ERROR;
                }
                else
                {
                    number = TRUE;                 /* A valid number was inputted */
                    comma = FALSE;                 /* Resetting comma (now it is needed) */
                    write_num_to_data(atoi(sign)); /* encoding number to data */
                }
            }
            else if (*sign != ',') /* If there was a number, now a comma is needed */
            {
                error = DATA_EXPECTED_COMMA_AFTER_NUM;
                return ERROR;
            }
            else /* If there was a comma, there should be only once in a row (means comma should be false) */
            {
                if (comma == TRUE)
                {
                    error = DATA_COMMAS_IN_A_ROW;
                    return ERROR;
                }
                else
                {
                    comma = TRUE;
                    number = FALSE;
                }
            }
        }
    }
    if (comma == TRUE) /* if line ended with a comma */
    {
        error = DATA_UNEXPECTED_COMMA;
        return ERROR;
    }
    return EMPTY_ERROR;
}

/* This function handles a .string guidance and encoding it to data */
int handle_string_guidance(char *line)
{
    char sign[MAX_INPUT];

    line = next_sign_string(sign, line);       /* hold the next sign in string*/
    if (!end_of_line(sign) && is_string(sign)) /* If sign exists and it's a valid string */
    {
        line = skip_spaces(line);
        if (end_of_line(line)) /* If there's no additional sign */
        {
            /* "Cutting" quotation marks and encoding it to data */
            sign[strlen(sign) - 1] = '\0';  /* replace last (") */
            write_string_to_data(sign + 1); /* skip first (") */
        }

        else /* There's another sign */
        {
            error = STRING_TOO_MANY_OPERANDS;
            return ERROR;
        }
    }

    else /* Invalid string */
    {
        error = STRING_OPERAND_NOT_VALID;
        return ERROR;
    }

    return EMPTY_ERROR;
}

/* This function handles an .extern guidance */
int handle_extern_guidance(char *line)
{

    char sign[MAX_LABEL]; /* This will hold the required label */

    copy_sign(sign, line); /* Getting the next sign */

    if (end_of_line(sign)) /* If the sign is empty, then there's no label */
    {
        error = EXTERN_NO_LABEL;
        return ERROR;
    }

    if (!is_label(sign, FALSE)) /* The sign should be a label (without a colon) */
    {
        error = EXTERN_INVALID_LABEL;
        return ERROR;
    }

    line = next_sign(line); /* Grab the next sign */
    if (!end_of_line(line)) /* Should not be next sign */
    {
        error = EXTERN_TOO_MANY_OPERANDS;
        return ERROR;
    }

    /* Trying to add the label to the symbols table */
    if (add_label(&symbols_table, sign, 1, TRUE) == NULL)
        return ERROR;

    return if_error(); /* Error code might be 1 if there was an error in is_label() */
}

/* This function handels commands and encodes to words */
int handle_command(int type, char *line)
{

    boolean is_first = FALSE;
    boolean is_second = FALSE;                                                    /* These booleans will tell which of the operands were received (not by source/dest, but by order) */
    int first_method = 0, second_method = 0;                                      /* These will hold the addressing methods of the operands */
    char first_operand[OPERAND_NAME_LENGTH], second_operand[OPERAND_NAME_LENGTH]; /* These strings will hold the operands */
    int first_register = 0, second_register = 0;                                  /* These will hold the register numbers */

    /* Trying to parse 2 operands */
    line = next_list_sign(first_operand, line);
    if (!end_of_line(first_operand)) /* If first operand is not empty */
    {
        is_first = TRUE; /* First operand exists! */
        line = next_list_sign(second_operand, line);
        if (!end_of_line(second_operand)) /* If second operand (should hold temporarily a comma) is not empty */
        {
            if (second_operand[0] != ',') /* A comma must separate two operands of a command */
            {
                error = COMMAND_UNEXPECTED_CHAR;
                return ERROR;
            }
            else
            {
                line = next_list_sign(second_operand, line); /* now grabs the second opperand after the comma between them */
                if (end_of_line(second_operand))             /* If second operand is not empty */
                {
                    error = COMMAND_UNEXPECTED_CHAR;
                    return ERROR;
                }
                is_second = TRUE; /* Second operand exists! */
            }
        }
    }
    line = skip_spaces(line);
    if (!end_of_line(line)) /* If the line continues after two operands */
    {
        error = COMMAND_TOO_MANY_OPERANDS;
        return ERROR;
    }

    if ((is_first == TRUE))
    {
        first_method = detect_method(first_operand); /* Detect addressing method of first operand */
        if (first_method == METHOD_REGISTER)
            first_register = find_reg_number(first_operand); /* Finds the register number */
        else if (first_method == METHOD_INDEX)
            first_register = find_reg_number_in_index(first_operand); /* Finds the register number if method is index */
    }
    if ((is_second == TRUE))
    {
        second_method = detect_method(second_operand);         /* Detect addressing method of second operand */
        if (second_method == METHOD_REGISTER)                  /* register as dest operand can only be in METHOD_REGISTER */
            second_register = find_reg_number(second_operand); /* Finds the register number */
        else if (second_method == METHOD_INDEX)
            second_register = find_reg_number_in_index(second_operand); /* Finds the register number if method is index */
    }

    if (!if_error()) /* If there was no error while trying to parse addressing methods */
    {
        if (command_accept_num_operands(type, is_first, is_second)) /* If number of operands is valid for this specific command */
        {
            if (command_accept_methods(type, first_method, second_method)) /* If addressing methods are valid for this specific command */
            {
                /* Encode first word of the command to memory and increase ic by the number of additional words */
                encode_to_instructions(build_first_word(type, is_first, is_second, first_method, second_method, first_register, second_register));
                if (type != STOP && type != RTS) /* commands with at least 1 operands will have 2 words */
                    encode_to_instructions(build_second_word(type, is_first, is_second, first_method, second_method, first_register, second_register));
                ic += calculate_command_num_additional_words(is_first, is_second, first_method, second_method);
            }
            else
            {
                error = COMMAND_INVALID_OPERANDS_METHODS;
                return ERROR;
            }
        }
        else
        {
            error = COMMAND_INVALID_NUMBER_OF_OPERANDS;
            return ERROR;
        }
    }
    return EMPTY_ERROR;
}

/* This function inserts .data to data memory */
void write_num_to_data(int num)
{
    unsigned int word = 0; /* An empty word */
    unsigned int temp = 0;
    int mask = (int)pow(2, IMMIDIATE_BITS) - 1; /*Creating a mask of 16 bits 0000 1111 1111 1111 1111*/

    word <<= ZERO_BITS;
    word <<= ARE_BITS;
    word |= ABSOLUTE;        /* Sets ARE bits*/
    word <<= IMMIDIATE_BITS; /* place ARE bits in their place */

    temp = (unsigned int)num; /* Two's complement */
    mask &= temp;             /* 0000 + the rest 16bits of temp */
    word |= mask;             /* merge ARE bits + temp to one word */

    data[dc++] = word;
}

/* This function tries to find the addressing method of a given operand and returns -1 if it was not found */
int detect_method(char *operand)
{
    if (end_of_line(operand)) /* If end of line*/
        return NO_MATCH;

    /*----- Immediate addressing method check -----*/
    if (*operand == '#')
    { /* First character is '#' */
        operand++;
        if (is_number(operand))
            return METHOD_IMMEDIATE;
    }

    /*----- Direct addressing method check ----- */
    else if (is_label(operand, FALSE)) /* Checking if it's a label when there shouldn't be a colon (:) at the end */
        return METHOD_DIRECT;

    /*----- Index addressing method check ----- */
    else if (is_label_index_method(operand, NULL, NULL))
        return METHOD_INDEX;

    /*----- Register addressing method check -----*/
    else if (is_register(operand))
        return METHOD_REGISTER;

    error = COMMAND_INVALID_METHOD;
    return NO_MATCH;
}

/* This function checks for the validity of given methods according to the opcode */
boolean command_accept_num_operands(int command_type, boolean first, boolean second)
{
    switch (command_type)
    {
    /* These opcodes must receive 2 operands */
    case MOV:
    case CMP:
    case ADD:
    case SUB:
    case LEA:
        return first && second;

    /* These opcodes must only receive 1 operand */
    case CLR:
    case NOT:
    case INC:
    case DEC:
    case JMP:
    case BNE:
    case JSR:
    case RED:
    case PRN:
        return first && !second;

    /* These opcodes can't have any operand */
    case RTS:
    case STOP:
        return !first && !second;
    }
    return FALSE;
}

/* This function checks for the validity of given addressing methods according to the opcode */
boolean command_accept_methods(int command_type, int first_method, int second_method)
{
    switch (command_type)
    {
        /* These commands only accept the next methods:
         * Source: 0, 1, 2, 3
         * Destination: 1, 2, 3
         */
    case MOV:
    case ADD:
    case SUB:
        return (first_method == METHOD_IMMEDIATE || first_method == METHOD_DIRECT || first_method == METHOD_INDEX || first_method == METHOD_REGISTER) &&
               (second_method == METHOD_DIRECT || second_method == METHOD_INDEX || second_method == METHOD_REGISTER);

        /* CMP command only accept the next methods:
         * Source: 0, 1, 2, 3
         * Destination: 0, 1, 2, 3
         */
    case CMP:
        return (first_method == METHOD_IMMEDIATE || first_method == METHOD_DIRECT || first_method == METHOD_INDEX || first_method == METHOD_REGISTER) &&
               (second_method == METHOD_IMMEDIATE || second_method == METHOD_DIRECT || second_method == METHOD_INDEX || second_method == METHOD_REGISTER);

        /* LEA command only accept the next methods:
         * Source: 1,2
         * Destination: 1, 2, 3
         */
    case LEA:
        return (first_method == METHOD_DIRECT || first_method == METHOD_INDEX) && (second_method == METHOD_DIRECT || second_method == METHOD_INDEX || second_method == METHOD_REGISTER);

        /* These commands only accept the next methods:
         * Source: NONE
         * Destination: 1, 2, 3
         */
    case CLR:
    case NOT:
    case INC:
    case DEC:
    case RED:
        return (first_method == METHOD_DIRECT || first_method == METHOD_INDEX || first_method == METHOD_REGISTER);

        /* These commands only accept the next methods:
         * Source: NONE
         * Destination: 1, 2
         */
    case JMP:
    case BNE:
    case JSR:
        return (first_method == METHOD_DIRECT || first_method == METHOD_INDEX);

        /* PRN command only accept the next methods:
         * Source: NONE
         * Destination: 0, 1, 2, 3
         */
    case PRN:
        return (first_method == METHOD_IMMEDIATE || first_method == METHOD_DIRECT || first_method == METHOD_INDEX || first_method == METHOD_REGISTER);

        /* These commands are always ok because they accept all methods/none of them and
         * number of operands is being verified in another function
         */
    case RTS:
    case STOP:
        return TRUE;
    }
    return FALSE;
}

/* This function encodes the first word of the command */
unsigned int build_first_word(int command_type, int is_first, int is_second, int first_method, int second_method, int first_register, int second_register)
{
    unsigned int word = 0; /* We decalred it as an unsigned int but the output is printed as 20 bits words */
    /* Words structure given by the project:
    word1[20] = 0 A R E OPCODE[16] */

    int newType = 0;
    newType = find_new_type(command_type); /* Finds the correct opcode to enter */

    /* STEP 0: After defination - the first 3 block are unused spares from unsinged int (32)
        word = [0000 0000 0000] 0000 0000 0000 0000 0000 */

    /* STEP 1: Insert 0 to last bit */
    word <<= ZERO_BITS;
    /* word = [0000 0000 0000] 0000 0000 0000 0000 000'0' */

    /* STEP 2: Insert the ARE bits */
    word <<= ARE_BITS; /* Make space for ARE_BITs */
    word |= ABSOLUTE;  /* First & second word are always ABSOLUTE */

    /* word = [0000 0000 0000] 0000 0000 0000 0000 0ARE */

    /* STEP 3: Insert the opcode */
    word <<= OPCODE_BITS;          /* Make space for opcode [16] */
    word |= bit_by_index(newType); /* Turn on the bit in the given index (5 means 2^5=32) */
    /* word = [0000 0000 0000] 0ARE 0000 0000 0000 0000 */

    return word;
}

/* This function encodes the first word of the command */
unsigned int build_second_word(int command_type, int is_first, int is_second, int first_method, int second_method, int first_register, int second_register)
{
    unsigned int word = 0; /* We decalred it as an unsigned int but the output is printed as 20 bits words */
    /* Words structure given by the project:
    word[20] = 0 A R E funct[4] src_reg[4] src_method[2] dest_reg[4] dest_method[2] */

    int funct = 0;
    funct = command_funct(command_type); /* Finds the correct funct val to enter */

    /****** STEP 0: After defination - the first 3 block are unused spares from unsinged int (32)
    word = [0000 0000 0000] 0000 0000 0000 0000 0000 */

    /****** STEP 1: Insert 0 to last bit */
    word <<= ZERO_BITS;
    /* word = [0000 0000 0000] 0000 0000 0000 0000 000'0' */

    /****** STEP 2: Insert the ARE bits */
    word <<= ARE_BITS;
    word |= ABSOLUTE;
    /* word = [0000 0000 0000] 0000 0000 0000 0000 0ARE */

    /****** STEP 3: Insert funct bits */
    word <<= FUNCT_BITS;
    if (funct != UNKNOWN_FUNCT) /* Make sure we don't enter a incorrect number */
        word |= funct;
    /* word = [0000 0000 0000] 0000 0000 0000 0ARE FUNCT_BITS */

    /****** STEP 4: Insert source register bits */
    word <<= SRC_REG_BITS; /* Leave space for register bits */
    if (is_first & is_second)
    {
        if (first_method == METHOD_REGISTER || first_method == METHOD_INDEX)
        {
            word |= first_register;
        }
    }
    /* word = [0000 0000 0000] 0000 0000 0ARE FUNCT_BITS SRC_REG */

    /****** STEP 5: Insert source register method bits */
    word <<= SRC_METHOD_BITS;  /* Leave space for first method bits */
    if (is_first && is_second) /* If there are two operands, insert the first */
    {
        word |= first_method; /* insert first method bits, if not, insert nothing and move bits*/
    }
    /* word = [0000 0000 0000] 0000 000A RE FUNCT_BITS SRC_REG SRC_METHOD  */

    /****** STEP 6: Insert destination register bits */
    word <<= DEST_REG_BITS; /* Leave space for register bits */
    if (is_first && is_second)
    {
        if (second_method == METHOD_REGISTER || second_method == METHOD_INDEX)
            word |= second_register; /*insert destination register number*/
    }                                /* If not, insert the first one (a single operand is a destination operand)*/
    else if (is_first)
    {
        word |= first_register;
    }
    /* word = [0000 0000 0000] 000A RE FUNCT_BITS SRC_REG SRC_METHOD DEST_REG  */

    /****** STEP 7: Insert destination register method bits */
    word <<= DEST_METHOD_BITS; /* Leave space for second method bits */
    if (is_first && is_second) /* If there are two operands, insert the second */
    {
        word |= second_method;
    }
    /* If not, insert the first one (a single operand is a destination operand) */
    else if (is_first)
    {
        word |= first_method;
    }
    /* word = [0000 0000 0000] 0A RE FUNCT_BITS SRC_REG SRC_METHOD DEST_REG DEST_METHOD  */

    return word;
}

/* This function calculates number of additional words for a command */
int calculate_command_num_additional_words(int is_first, int is_second, int first_method, int second_method)
{
    int count = 0;
    if (is_first)
        count += num_words(first_method);
    if (is_second)
        count += num_words(second_method);

    return count;
}

/* This function returns how many additional words an addressing method requires */
int num_words(int method)
{

    if ((method == METHOD_DIRECT) || (method == METHOD_INDEX)) /* Another 2 words for the label info */
        return 2;
    else if (method == METHOD_IMMEDIATE) /* Aanother word for the immediate number */
        return 1;
    else /* in case the method is REGISTER, we coded it in the second word */
        return 0;
}

/* Finds the real number of the opcode to insert in word */
int find_new_type(int commandType)
{
    switch (commandType)
    {
    case MOV:
        return MOV_OP;
        break;

    case CMP:
        return CMP_OP;
        break;

    case ADD:
    case SUB:
        return ADDSUB_OP;
        break;

    case LEA:
        return LEA_OP;
        break;

    case CLR:
    case NOT:
    case INC:
    case DEC:
        return CLRNOTINCDEC_OP;
        break;

    case JMP:
    case BNE:
    case JSR:
        return JMPBNEJSR_OP;
        break;

    case RED:
        return RED_OP;
        break;

    case PRN:
        return PRN_OP;
        break;

    case RTS:
        return RTS_OP;
        break;

    case STOP:
        return STOP_OP;
        break;
    }
    return UNKNOWN_FUNCT;
}

/* Finds the function number to insert in word */
int command_funct(int commands)
{

    switch (commands)
    {
    /* These commands have a funct val of 1 */
    case ADD:
    case CLR:
    case JMP:
        return ADDCLRJMP_GROUP;
        break;

    /* These commands have a funct val of 2 */
    case SUB:
    case NOT:
    case BNE:
        return SUBNOTBNE_GROUP;
        break;

    /* These commands have a funct val of 3 */
    case INC:
    case JSR:
        return INCEJSR_GROUP;
        break;

    /* These commands have a funct val of 4 */
    case DEC:
        return DEC_GROUP;
        break;

        /* The rest have no funct val, and we insert 0 */
    }
    return UNKNOWN_FUNCT;
}

/* Turn on the bit in the given index (5 means 2^5=32 (= 0010 0000)) */
unsigned int bit_by_index(int index)
{
    if (index >= 0 && index <= 31)
    {
        return (unsigned int)(pow(2, index)); /* for index=5, returns the number: 0010 0000 (=32) */
    }
    return -1;
}