#include "macroDeploy.h"

/* returns new file after macro deployment*/
FILE *macroDeployment(FILE *fp, char *newFileName)
{
    FILE *nfp;
    char lineOfFile1[MAX_INPUT], *tempLine, *bkpLine = NULL, *token, *label;
    boolean flag_macro_on = FALSE, flag_defect_macro_on = FALSE;
    macroNode *head = NULL, *currentMacro; /* list of macros */
    int i = 0, line_number = 0;

    if ((nfp = fopen(newFileName, "w+")) == NULL) /* Open the new file with writing permision */
    {
        fprintf(stderr, "\nAn error occured while creating / opening the file: %s\n", newFileName);
    }
    else
    {
        rewind(fp); /* in case some1 progress in the file */
        /* Step 1 - Copy all macros to nodes */
        while (fgets(lineOfFile1, MAX_INPUT, fp) != NULL)
        {
            line_number++;
            error = EMPTY_ERROR; /* Reset the error variable at the begining of a new line*/
            if (bkpLine != NULL) /* Prevent memory leaks because of strdup()*/
            {
                free(bkpLine);
                bkpLine = NULL;
            }
            tempLine = strdup(lineOfFile1);
            bkpLine = tempLine;           /* bkp for using free() later*/
            if (ignore_line(lineOfFile1)) /* skip blank lines */
                continue;
            tempLine = skip_spaces(tempLine); /* tempLine = lineOfFile without spaces*/
            token = strtok(tempLine, " ");
            if (flag_macro_on || flag_defect_macro_on) /* save the lines of a macro */
            {
                if (!strncmp(token, "endm", 4)) /* macro end */
                {
                    if (!isLastWord(token, lineOfFile1)) /* Extra text after 'endm' */
                    {
                        error = EXTRA_TEXT_AFTER_ENDM;
                        write_error(line_number);
                    }
                    flag_macro_on = FALSE;
                    flag_defect_macro_on = FALSE;
                    i = 0;
                    continue;
                }
                if (i >= MACRO_MAX_LINES) /* if exceeds the number of allowed max lines in macro definition */
                {
                    error = EXCEED_MACRO_MAX_LINES;
                    write_error(line_number);
                    continue;
                }
                if (flag_macro_on) /* only when valid macro is defined*/
                {
                    if (is_label(token, TRUE)) /* ERROR if there is a Label in macro */
                    {
                        error = LABEL_IN_MACRO;
                        write_error(line_number);
                        return NULL;
                    }
                    strncpy(head->line[i], lineOfFile1, MAX_INPUT - 1); /* ensure we dont override the next line incase input is longer then MAX_INPUT */
                    head->line[i++][MAX_INPUT - 1] = '\0';
                }
                continue;
            }
            if (!strcmp(token, "macro")) /* macro start */
            {
                char *bkp;
                token = strtok(NULL, " "); /* token = macro_name */
                bkp = strdup(token);       /* backup the name */
                if (bkp[strlen(bkp) - 1] == '\n')
                    bkp[strlen(bkp) - 1] = 0;

                if (isLastWord(token, lineOfFile1)) /* No string left after macro name*/
                {
                    if (valid_macro_name(bkp)) /* valid macro name */
                    {
                        addNode(&head, bkp);
                        flag_macro_on = TRUE;
                    }
                    else /* invalid macro name */
                    {
                        write_error(line_number);
                        flag_defect_macro_on = TRUE; /* defected macro - just skip his lines*/
                    }
                }
                else /* there is garbage string after macro_name*/
                {
                    error = EXTRA_TEXT_AFTER_MACRO_NAME;
                    write_error(line_number);
                    flag_defect_macro_on = TRUE;
                }
                free(bkp);
            }
        }
        if (flag_macro_on) /* if macro definition remained open */
        {
            error = UNCLOSED_MACRO_DEFINITION;
            write_error(line_number);
            exit(1);
        }

        /* Step 2 - copy source file to new file and deploy macros */
        rewind(fp);
        line_number = 0;

        while (fgets(lineOfFile1, MAX_INPUT, fp) != NULL)
        {
            line_number++;
            error = EMPTY_ERROR; /* Reset the error variable at the begining of a new line*/
            if (bkpLine != NULL) /* Prevent memory leaks because of strdup() */
            {
                free(bkpLine);
                bkpLine = NULL;
            }
            tempLine = strdup(lineOfFile1); /* copy line temporary */
            bkpLine = tempLine;
            if (ignore_line(tempLine)) /* skip blank lines */
                continue;
            token = strtok(tempLine, " ");
            if (flag_macro_on) /* skip on copying macro definitions to the new file */
            {
                if (!strncmp(token, "endm", 4)) /* end of macro definition */
                    flag_macro_on = FALSE;
                continue;
            }
            else if (is_label(token, TRUE)) /* label before calling a macro */
            {
                label = strdup(token);     /* duplicate the LABEL to temp parameter */
                token = strtok(NULL, " "); /* skip label and grab the next word */
                if ((currentMacro = isMacro(token, head)) != NULL)
                {
                    if (isLastWord(token, lineOfFile1)) /* macro name is the last word in line */
                    {
                        print_label_then_macro(nfp, label, currentMacro, bkpLine); /* merge label and macro to the same line and print */
                    }
                    else
                    {
                        error = EXTRA_TEXT_AFTER_MACRO_NAME;
                        write_error(line_number);
                        fputs(lineOfFile1, nfp);
                    }
                }
                else
                    fputs(lineOfFile1, nfp);

                free(label);
            }
            else if ((currentMacro = isMacro(token, head)) != NULL) /* line that calls macro */
            {
                if (isLastWord(token, lineOfFile1))
                {
                    pasteMacro(nfp, currentMacro, 0);
                }
                else
                {
                    error = EXTRA_TEXT_AFTER_MACRO_NAME;
                    write_error(line_number);
                    fputs(lineOfFile1, nfp);
                }
            }
            else if (!strcmp(token, "macro")) /* Macro definition begun */
            {
                flag_macro_on = TRUE;
                continue;
            }
            else /* copy the line as is */
            {
                fputs(lineOfFile1, nfp);
            }
        }
    }
    freeList(head);
    fclose(fp);
    return nfp;
}
/*
macroNode callingMacro(char *line, char *token)
{
}*/

macroNode *isMacro(char *token, macroNode *currentNode) /* TRUE if received token is a call for a macro */
{
    char *tempToken = strdup(token);
    if (tempToken[strlen(tempToken) - 1] == ' ' || tempToken[strlen(tempToken) - 1] == '\t' || tempToken[strlen(tempToken) - 1] == '\n') /* remove white chars after macro name */
        tempToken[strlen(tempToken) - 1] = 0;

    while (currentNode != NULL) /* searching in macro's list if there is a node with (name == token) */
    {
        if (!strcmp(currentNode->name, tempToken))
        {
            free(tempToken);
            return currentNode;
        }
        currentNode = currentNode->next;
    }
    free(tempToken);
    return NULL;
}

boolean valid_macro_name(char *sign) /* This function checks if a sign is a macro name or not */
{
    int i, sign_len = strlen(sign);
    if (sign == NULL || sign_len < 2) /* The min length for a macro's name is 2 */
    {
        error = MACRO_NAME_TOO_SHORT;
        return FALSE;
    }

    if (sign_len > MAX_LABEL) /* Maximum length same as label */
    {
        error = MACRO_NAME_TOO_LONG;
        return FALSE;
    }

    if (isdigit(sign[0])) /* First character must be a letter */
    {
        error = MACRO_INVALID_FIRST_CHAR;
        return FALSE;
    }

    /* check if there are uppercase letters or symbols (: . , ' - _ ) */
    for (i = 0; i < sign_len; i++)
    {
        if (isupper(sign[i]))
        {
            error = MACRO_UPPERCASE_LETTER;
            return FALSE;
        }
        if (!isalpha(sign[i]) && !isdigit(sign[i])) /* symbols not allowed */
        {
            error = INVALID_MACRO_NAME;
            return FALSE;
        }
    }

    if (find_command(sign) != NO_MATCH) /* Macro can't have the same name as a command */
    {
        error = MACRO_CANT_BE_COMMAND;
        return FALSE;
    }

    if (is_register(sign)) /* Macro can't have the same name as a register */
    {
        error = MACRO_CANT_BE_REGISTER;
        return FALSE;
    }

    return TRUE; /* Valid macro name! */
}

void pasteMacro(FILE *nfp, macroNode *currentMacro, int fromLine) /* paste the macro's lines in the new file*/
{
    int i; /* usually 0 */
    for (i = fromLine; i < MACRO_MAX_LINES; i++)
    {
        fputs(currentMacro->line[i], nfp);
    }
    currentMacro = NULL;
    return;
}

/*print label and macro at the same line (merge between them)  */
void print_label_then_macro(FILE *nfp, char *label_name, macroNode *currentMacro, char *src_line)
{
    char new_line[MAX_INPUT]; /* merge label and macro first line to the new_line */
    char *temp, *temp_macroFirstLine;

    strcpy(new_line, src_line); /* copy src line to new line */
    temp = new_line;
    temp = skip_spaces(temp); /* get to the first token (the label) */

    while (!end_of_line(temp) && *temp != ' ') /* skip the label */
        temp++;

    if (end_of_line(temp)) /* in case line has ended unexpectly */
        return;

    *(temp++) = '\t';               /* tab between the label and first line of macro */
    new_line[MAX_INPUT - 1] = '\0'; /* null character manually added */

    temp_macroFirstLine = currentMacro->line[0];            /* hold the first line in macro */
    temp_macroFirstLine = skip_spaces(temp_macroFirstLine); /* temp_macro is now point at the first token */

    strncpy(temp, temp_macroFirstLine, MAX_INPUT); /* copy the rest of macro line into the left space in new_line */

    fputs(new_line, nfp); /* print the new_line */

    pasteMacro(nfp, currentMacro, 1); /* paste the rest of the macro, first line not included */
}

void addNode(struct macroNode **head, char *data) /* add new node of macro to the list*/
{
    int i;

    /* 1. allocate node */
    macroNode *new_node = (macroNode *)calloc(1, sizeof(macroNode));

    /* 2. put in the data  */
    new_node->name = (char *)malloc(strlen(data) + 1);
    strcpy(new_node->name, data);
    for (i = 0; i < MACRO_MAX_LINES; i++) /* reset the lines */
        new_node->line[i][0] = 0;

    /* 3. move the head to point to the new node */
    new_node->next = (*head);
    (*head) = new_node;
}

void freeList(struct macroNode *head) /* free list of macro*/
{
    struct macroNode *tmp;
    while (head)
    {
        free(head->name);
        tmp = head->next;
        free(head);
        head = tmp;
    }
}

char *strdup(char *str)
{
    int len = strlen(str);
    char *temp = (char *)malloc(len + 1);
    memcpy(temp, str, len + 1);
    return temp;
}

boolean isLastWord(char *word, char *line) /* Is 'word' the last word in line */
{
    char tempLine[MAX_INPUT], *token, *previousToken = NULL;
    strcpy(tempLine, line);
    token = strtok(tempLine, " ");
    do /* until token == word */
    {
        /* last word means \n as the last letter */
        if (token[strlen(token) - 1] == '\n' && !strcmp(token, word)) /* when \n stick to the last word ("endm\n")*/
            return TRUE;
        if (!strcmp(token, "\n") && !strcmp(previousToken, word)) /* when \n is seprated and apears after last word ("endm \n")*/
            return TRUE;
        previousToken = token;
    } while ((token = strtok(NULL, " ")) != NULL); /* NULL when no more tokens left*/

    return FALSE;
}

/* test function:
int main(int argc, char *argv[])
{
    char *filename = NULL;
    FILE *fp;

    if (argc < 2)
    {
        fprintf(stdout, "No argument found, please enter a file name to process\n");
        exit(1);
    }
    else
    {
        filename = strdup(argv[1]);
        if ((fp = fopen(filename, "r")) == NULL)
        {  Open the file with reading permision
            fprintf(stderr, "\nAn error occured while opening the file: %s\n", filename);
        }
        else
        {
            macroDeployment(fp, filename);
        }
        free(filename);
        return 0;
    }
}*/
