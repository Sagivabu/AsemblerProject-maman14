#include "main.h"
#include "common.h"
#include "external_vars.h"

/* Functions regarding the labels table */
labelPtr add_label(labelPtr *lptr, char *name, unsigned int address, boolean external, ...)
{
    va_list p;

    labelPtr t = *lptr;
    labelPtr temp; /* Auxiliary variable to store the info of the label and add to the list */

    if (is_existing_label(*lptr, name))
    {
        error = LABEL_ALREADY_EXISTS;
        return NULL;
    }
    temp = (labelPtr)malloc(sizeof(Labels));
    if (!temp) /*if we couldn't allocate memory to temp then print an error massage and exit the program*/
    {
        printf("\nerror, cannot allocate memory\n");
        exit(ERROR);
    }

    /* Storing the info of the label in temp */
    strcpy(temp->name, name);
    temp->entry = FALSE;
    temp->address = address;
    temp->external = external;

    if (!external) /* An external label can't be in an action statement */
    {
        va_start(p, external);
        temp->inActionStatement = va_arg(p, boolean);
    }
    else
    {
        extern_exists = TRUE;
    }

    /* If the list is empty then we set the head of the list to be temp */
    if (!(*lptr))
    {
        *lptr = temp;
        temp->next = NULL;
        return temp;
    }

    /* Setting a pointer to go over the list until he points on the last label and then stting temp to be the new last label */
    while (t->next != NULL)
        t = t->next;
    temp->next = NULL;
    t->next = temp;

    va_end(p);
    return temp;
}

/* This function checks if the label already exists */
boolean is_existing_label(labelPtr lbl, char *name)
{

    return (get_label(lbl, name) != NULL);
}

/* This function gets the label */
labelPtr get_label(labelPtr lbl, char *name)
{
    while (lbl)
    {
        if (strcmp(lbl->name, name) == 0)
            return lbl;
        lbl = lbl->next;
    }
    return NULL;
}

/* This function delete label in the label list in case we entered a wrong label */
int delete_label(labelPtr *lptr, char *name)
{

    labelPtr temp = *lptr;
    labelPtr prevtemp;
    while (temp)
    {
        if (strcmp(temp->name, name) == 0)
        {
            if (strcmp(temp->name, (*lptr)->name) == 0) /* if label is head*/
            {
                *lptr = (*lptr)->next;
                free(temp);
            }
            else
            {
                prevtemp->next = temp->next;
                free(temp);
            }
            return 1;
        }
        prevtemp = temp;
        temp = temp->next;
    }
    return 0;
}

/* This functions sets the correct address for the label (memory starts at 100) */
void offset_address(labelPtr lptr, int num, boolean is_data)
{
    while (lptr)
    {
        if (!(lptr->external) && (is_data ^ (lptr->inActionStatement)))
        {
            lptr->address += num;
        }
        lptr = lptr->next;
    }
}

/* This function sets the boolean variable of an existing entry to true */
int make_entry(labelPtr l, char *name)
{
    labelPtr label = get_label(l, name); /* get the label */

    if (label != NULL)
    {
        if (label->external)
        {
            error = ENTRY_CANT_BE_EXTERN; /*add to error list*/
            return FALSE;
        }
        label->entry = TRUE;
        entry_exists = TRUE; /* Global variable that holds that there was at least one entry in the program */
        return TRUE;
    }
    else
        error = ENTRY_LABEL_DOES_NOT_EXIST; /*add to error list*/
    return FALSE;
}

/* This function gets the label address */
unsigned int get_label_address(labelPtr l, char *name)
{
    labelPtr label = get_label(l, name);
    if (label != NULL)
    {
        return (label->address);
    }
    else
        return FALSE;
}

/* This function checks if the label is external */
boolean is_external_label(labelPtr l, char *name)
{
    labelPtr label = get_label(l, name);
    if (label != NULL)
    {
        return (label->external);
    }
    else
        return FALSE;
}

/* This function gets the label for the extLabel table */
extPtr get_ext_label(extPtr xlbl, char *name)
{

    while (xlbl)
    {
        if (strcmp(xlbl->name, name) == 0)
            return xlbl;
        xlbl = xlbl->next;
    }
    return NULL;
}

/* This function frees the label list */
void free_label_table(labelPtr *lptr)
{

    labelPtr temp;
    while (*lptr)
    {
        temp = *lptr;
        *lptr = (*lptr)->next;
        free(temp);
    }
}