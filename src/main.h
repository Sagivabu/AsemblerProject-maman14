#ifndef main_h

#define main_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

/******************************************************
 * This file includes ENUMS for the entire project    *
 ******************************************************/

/******************** Genral Values ***********************/
#define MAX_INPUT 81   /* maximum chars per line*/
#define MAX_LABEL 32   /* maximum chars per label*/
#define DEFAULT_IC 100 /* Start of memory stack */
#define ERROR 1
#define EMPTY_ERROR 0
#define NO_MATCH -1 /* no match for comparison*/
#define NUM 12      /* length of possible number in .data, I took the biggest int number 2147483647 */
#define MAX_WORDS 500
#define UNKNOWN_FUNCT 0
#define OPERAND_NAME_LENGTH 20 /* random length of operand name */
#define ADDRESS_MODULO 16      /* Base address is the closet number to address's number and is divided by MODULO */
#define MACRO_MAX_LINES 6

/******************** Limits ***********************/
#define REG_LEN_MAX 3       /* register length is 2/3 - r1,r2...r14,r15 */
#define REG_LEN_MIN 2       /* register length is 2/3 - r1,r2...r14,r15 */
#define MIN_REGISTER_NUM 0  /* r0 */
#define MAX_REGISTER_NUM 15 /* r15 */

/******************** Bits allocation in word **********************/
#define ZERO_BITS 1
#define ARE_BITS 3
#define OPCODE_BITS 16
#define FUNCT_BITS 4
#define SRC_REG_BITS 4
#define SRC_METHOD_BITS 2
#define DEST_REG_BITS 4
#define DEST_METHOD_BITS 2
#define IMMIDIATE_BITS 16
#define FIRST4BITS 15 /* 0000 0000 0000 1111 */
#define SKIP4BITS 4   /* skip to the next 4 bits */

#define SRC_METHOD_START_POS 6
#define SRC_METHOD_END_POS 7
#define DEST_METHOD_START_POS 0
#define DEST_METHOD_END_POS 1

#define SECOND_WORD (ic + 1) /* usually ic will hold the first word */

/******************** Enums ***********************/

/* Directives types */
enum guidance
{
    DATA,
    STRING,
    ENTRY,
    EXTERN,
    UNKNOWN_TYPE
};

/* Enum of commands ordered by their opcode */
enum commands
{
    MOV,
    CMP,
    ADD,
    SUB,
    LEA,
    CLR,
    NOT,
    INC,
    DEC,
    JMP,
    BNE,
    JSR,
    RED,
    PRN,
    RTS,
    STOP,
    UNKNOWN_COMMAND
};

/* Enum of file types of the required output */
enum filetype
{
    FILE_INPUT,
    FILE_AM,
    FILE_OBJECT,
    FILE_ENTRY,
    FILE_EXTERN
};

/* Commands opcode */
enum OP_CODE
{
    MOV_OP = 0,
    CMP_OP = 1,
    ADDSUB_OP = 2,
    LEA_OP = 4,
    CLRNOTINCDEC_OP = 5,
    JMPBNEJSR_OP = 9,
    RED_OP = 12,
    PRN_OP = 13,
    RTS_OP = 14,
    STOP_OP = 15
};

/* Commands are divided into 4 groups with different function numbers */
enum commands_funct
{
    ADDCLRJMP_GROUP = 10,
    SUBNOTBNE_GROUP = 11,
    INCEJSR_GROUP = 12,
    DEC_GROUP = 13
};

/* A/R/E modes ordered by their numerical value */
enum ARE
{
    EXTERNAL = 1,
    RELOCATABLE = 2,
    ABSOLUTE = 4
};

/* Addressing methods ordered by their code */
enum methods
{
    METHOD_IMMEDIATE,
    METHOD_DIRECT,
    METHOD_INDEX,
    METHOD_REGISTER,
    METHOD_UNKNOWN
};

/* To identify a label */
enum
{
    NO_COLON,
    COLON
};

/* Errors */
enum errors
{
    INVALID_SYNTAX = 1,
    INVALID_LABEL_LINE,
    MISSING_SYNTAX,
    NO_PARAMETER_AVAILABLE,
    GUIDANCE_INVALID_NUM_PARAMS,
    DATA_EXPECTED_NUM,
    DATA_EXPECTED_COMMA_AFTER_NUM,
    DATA_COMMAS_IN_A_ROW,
    DATA_UNEXPECTED_COMMA,
    STRING_TOO_MANY_OPERANDS,
    STRING_OPERAND_NOT_VALID,
    EXTERN_NO_LABEL,
    EXTERN_INVALID_LABEL,
    EXTERN_TOO_MANY_OPERANDS,
    COMMAND_UNEXPECTED_CHAR,
    COMMAND_TOO_MANY_OPERANDS,
    COMMAND_INVALID_OPERANDS_METHODS,
    COMMAND_INVALID_NUMBER_OF_OPERANDS,
    COMMAND_INVALID_METHOD,
    CANNOT_OPEN_FILE,
    COMMAND_LABEL_DOES_NOT_EXIST,
    LABEL_TOO_LONG,
    LABEL_INVALID_FIRST_CHAR,
    LABEL_CANT_BE_COMMAND,
    LABEL_CANT_BE_REGISTER,
    LABEL_ALREADY_EXISTS,
    ENTRY_CANT_BE_EXTERN,
    ENTRY_LABEL_DOES_NOT_EXIST,
    NOT_A_VALID_REG,
    NO_LABEL_FOUND,
    NOT_A_LABEL,
    LABEL_IN_MACRO,
    EXCEED_MACRO_MAX_LINES,
    UNCLOSED_MACRO_DEFINITION,
    EXTRA_TEXT_AFTER_MACRO_NAME,
    EXTRA_TEXT_AFTER_ENDM,
    INVALID_MACRO_NAME,
    MACRO_NAME_TOO_SHORT,
    MACRO_NAME_TOO_LONG,
    MACRO_UPPERCASE_LETTER,
    MACRO_INVALID_FIRST_CHAR,
    MACRO_CANT_BE_COMMAND,
    MACRO_CANT_BE_REGISTER,
    INDEX_METHOD_BLANK_OPERAND,
    INVALID_SYNTAX_FOR_INDEX_METHOD,
    INVALID_LABEL_NAME,
    INVALID_REGISTER_IN_INDEX_METHOD
};

#endif