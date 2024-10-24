#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "assembler.h"

//asm input_file.txt output.bin
//spu output.bin


int file_opener ()
{
    assembler_t sossembler = {};
    assembler_t* ptr_ass = &sossembler;

    ptr_ass->orig_file = fopen ( "original.asm", "r" );
    ptr_ass->compiled_file = fopen ( "compilated.txt", "w" );

    label_t labels_array [ LABELS_ARRAY_SIZE ] = {};
    fiksik_t fixup_label [ LABELS_ARRAY_SIZE ] = {};
    ptr_ass->fixup_label =  fixup_label;
    ptr_ass->labels_array = labels_array;

    ptr_ass->ptr_cmd = ( int* ) calloc ( MAX_CMD_SIZE, sizeof ( int ) );

    for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
    {
        ptr_ass->labels_array [ i ].ip = POISON_VALUE;
        strncpy ( ptr_ass->labels_array [ i ].name, "POISON", NAME_LENGTH );
        ptr_ass->fixup_label [ i ].ip_of_L_cmd = POISON_VALUE;
        strncpy ( ptr_ass->fixup_label [ i ].name, "POISON", NAME_LENGTH );
    }

    code_translator ( ptr_ass );

    fclose ( ptr_ass->orig_file );
    fclose ( ptr_ass->compiled_file );
    return FUNC_DONE;
}

int code_translator ( assembler_t* ptr_ass )
{
    int counter = -1;
    char cmd[MAX_CMD_LENGTH] = {};
    while  ( fscanf ( ptr_ass->orig_file, "%s", cmd ) != EOF )
    {
        counter++;
        if ( strcmp ( cmd, "PUSH" ) == 0) {
            int operand = 0;
            ptr_ass->ptr_cmd [ counter ] = PUSH;
            fscanf ( ptr_ass->orig_file, "%d", &operand );
            counter++;
            ptr_ass->ptr_cmd [ counter ] = operand;
            continue;
        }
        else if ( strcmp ( cmd, "SUM" ) == 0){
            ptr_ass->ptr_cmd [ counter ] = SUM;
            continue;
        }
        else if ( strcmp ( cmd, "SUB" ) == 0){
            ptr_ass->ptr_cmd [ counter ] = SUB;
            continue;
        }
        else if ( strcmp ( cmd, "MUL" ) == 0){
            ptr_ass->ptr_cmd [ counter ] = MUL;
            continue;
        }
        else if ( strcmp ( cmd, "DIV" ) == 0){
            ptr_ass->ptr_cmd [ counter ] = DIV;
            continue;
        }
        else if ( strcmp ( cmd, "OUT" ) == 0){
            ptr_ass->ptr_cmd [ counter ] = OUT;
            continue;
        }
        else if ( strcmp ( cmd, "HLT" ) == 0){
            ptr_ass->ptr_cmd [ counter ] = HLT;
            continue;
        }
        else if ( strcmp ( cmd, "DUMP" ) == 0){
            ptr_ass->ptr_cmd [ counter ] = DUMP;
            continue;
        }
        else if ( strcmp ( cmd, "PUSHR" ) == 0){
            char reg[5] = {};
            fscanf ( ptr_ass->orig_file, "%s", reg );
            ptr_ass->ptr_cmd [ counter ] = PUSHR;
            counter++;
            ptr_ass->ptr_cmd [ counter ] = reg - 'A';
            continue;
        }
        else if ( strcmp ( cmd, "POP" ) == 0){
            char reg[5] = {};
            fscanf ( ptr_ass->orig_file, "%s", reg );
            ptr_ass->ptr_cmd [ counter ] = POP;
            counter++;
            ptr_ass->ptr_cmd [ counter ] = reg - 'A';
            continue;
        }
        else if ( strcmp ( cmd, "JMP" ) == 0){
            ptr_ass->ptr_cmd [ counter ] = JMP;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            counter++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        ptr_ass->ptr_cmd [ counter ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    else
                    {
                        where [ strlen ( where ) -1 ] = '\0';
                        ptr_ass->ptr_cmd [ counter ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, counter );
                        break;
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd [ counter ] = atoi ( where );
                break;
            }
            continue;
        }
        else if ( strcmp ( cmd, "JA" ) == 0){
            ptr_ass->ptr_cmd [ counter ] = JA;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            counter++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        ptr_ass->ptr_cmd [ counter ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    else
                    {
                        where [ strlen ( where ) -1 ] = '\0';
                        ptr_ass->ptr_cmd [ counter ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, counter );
                        break;
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd [ counter ] = atoi ( where );
                break;
            }
            continue;
        }
        else if ( strcmp ( cmd, "JMP" ) == 0){
            ptr_ass->ptr_cmd [ counter ] = JMP;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            counter++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        ptr_ass->ptr_cmd [ counter ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    else
                    {
                        where [ strlen ( where ) -1 ] = '\0';
                        ptr_ass->ptr_cmd [ counter ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, counter );
                        break;
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd [ counter ] = atoi ( where );
                break;
            }
            continue;
        }
        else if ( strcmp ( cmd, "JAE" ) == 0){
            ptr_ass->ptr_cmd [ counter ] = JAE;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            counter++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        ptr_ass->ptr_cmd [ counter ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    else
                    {
                        where [ strlen ( where ) -1 ] = '\0';
                        ptr_ass->ptr_cmd [ counter ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, counter );
                        break;
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd [ counter ] = atoi ( where );
                break;
            }
            continue;
        }
        else if ( strcmp ( cmd, "JBE" ) == 0){
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            counter++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        fprintf ( ptr_ass->compiled_file, "%d %d ", JBE, ptr_ass->labels_array[ i ].ip );
                        break;
                    }
                    else
                    {
                        fprintf ( stderr, "No label with name '%s'\n", where );
                        break;
                    }
                }
            }
            else
            {
            fprintf ( stderr, "where: %d\n", atoi ( where ) );
            fprintf ( ptr_ass->compiled_file, "%d %d ", JBE, atoi ( where ) );
            }
            continue;
        }
        else if ( strcmp ( cmd, "JE" ) == 0){
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            counter++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        fprintf ( ptr_ass->compiled_file, "%d %d ", JE, ptr_ass->labels_array[ i ].ip );
                        break;
                    }
                    else
                    {
                        fprintf ( stderr, "No label with name '%s'\n", where );
                        break;
                    }
                }
            }
            else
            {
            fprintf ( stderr, "where: %d\n", atoi ( where ) );
            fprintf ( ptr_ass->compiled_file, "%d %d ", JE, atoi ( where ) );
            }
            continue;
        }
       else if ( strcmp ( cmd, "JNE" ) == 0){
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            counter++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        fprintf ( ptr_ass->compiled_file, "%d %d ", JNE, ptr_ass->labels_array[ i ].ip );
                        break;
                    }
                    else
                    {
                        fprintf ( stderr, "No label with name '%s'\n", where );
                        break;
                    }
                }
            }
            else
            {
            fprintf ( stderr, "where: %d\n", atoi ( where ) );
            fprintf ( ptr_ass->compiled_file, "%d %d ", JNE, atoi ( where ) );
            }
            continue;
        }
        else if ( cmd [ strlen( cmd ) - 1 ] == ':')
        {
            create_new_label ( ptr_ass, cmd, counter );

            lobotomy ( ptr_ass );

            continue;
        }

        else fprintf ( stderr, "ERROR SYNTAX: cmd = %s\n", cmd );
        break;
    }
    for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
    {
        if ( strcmp ( ptr_ass->fixup_label [ i ].name, "POISON" ) != 0 )
        {

        }
    }
    // fwrite!!
    return FUNC_DONE; // TODO: emoe
}

int create_new_label ( assembler_t* ptr_ass, char label[NAME_LENGTH], int counter )
{
    for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
    {
        if ( ptr_ass->labels_array[ i ].ip == POISON_VALUE )
        {
            strncpy ( ptr_ass->labels_array[ i ].name, label, NAME_LENGTH );
            ptr_ass->labels_array[ i ].ip = counter;
            break;
        }
    }

    return FUNC_DONE;
}

int create_new_fiksik ( assembler_t* ptr_ass, char label[NAME_LENGTH], int counter )
{
    for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
    {
        if ( ptr_ass->fixup_label[ i ].ip_of_L_cmd == POISON_VALUE )
        {
            strncpy ( ptr_ass->fixup_label[ i ].name, label, NAME_LENGTH );
            ptr_ass->fixup_label[ i ].ip_of_L_cmd = counter;
            break;
        }
    }

    return FUNC_DONE;
}

int lobotomy ( assembler_t* ptr_ass )
{
    printf ( "              LABELS        \n");
    for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
    {
        printf (  "label: name - %s, ip = %d\n", ptr_ass->labels_array[i].name, ptr_ass->labels_array[i].ip );
    }
    printf ( "-----------------------------\n"
             "             FIKSIKS          \n");
    for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
    {
        printf (  "label: name - %s, ip = %d\n", ptr_ass->fixup_label[i].name, ptr_ass->fixup_label[i].ip_of_L_cmd );
    }

    printf ( "-------------------------------\n");

    return FUNC_DONE;
}
