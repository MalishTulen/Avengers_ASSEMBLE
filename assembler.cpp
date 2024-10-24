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

    ass_ctor ( ptr_ass );

fprintf ( stderr, "DEBUG2\n");

    code_translator ( ptr_ass );
fprintf ( stderr, "DEBUG3\n");
    fprintf ( stderr, " size : %d\n", ptr_ass->ptr_cmd->size_code );

    for ( int i = 0; i < ptr_ass->ptr_cmd->size_code; i++ )
    {
        printf ( "%d: %d\n", i, ptr_ass->ptr_cmd->ptr_code [ i ]);
    }

    fclose ( ptr_ass->orig_file );
    fclose ( ptr_ass->compiled_file );
    return FUNC_DONE;
}

int code_translator ( assembler_t* ptr_ass )
{
    char cmd[MAX_CMD_LENGTH] = {};

fprintf ( stderr, "DEBUG_CODE_TRANSLATOR1\n");

    while  ( fscanf ( ptr_ass->orig_file, "%s", cmd ) != EOF )
    {
        //ptr_ass->ptr_cmd->size_code += 1;
        if ( strcmp ( cmd, "PUSH" ) == 0) {
fprintf ( stderr, "DEBUG_CODE_TRANSLATOR( PUSH1 )\n");
            int operand = 0;
            ptr_ass->ptr_cmd->ptr_code [ 0 ] = PUSH;
fprintf ( stderr, "DEBUG: size_cmd = %d\n", ptr_ass->ptr_cmd->size_code );
fprintf ( stderr, "DEBUG_CODE_TRANSLATOR( %d )\n", * (ptr_ass->ptr_cmd->ptr_code + ptr_ass->ptr_cmd->size_code) );
fprintf ( stderr, "DEBUG_CODE_TRANSLATOR( PUSH2 )\n");
            fscanf ( ptr_ass->orig_file, "%d", &operand );
            ptr_ass->ptr_cmd->size_code++;
fprintf ( stderr, "operand: %d\n", operand );
fprintf ( stderr, "DEBUG_CODE_TRANSLATOR( PUSH3 )\n");
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = operand;
            continue;
        }
        else if ( strcmp ( cmd, "SUM" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = SUM;
            continue;
        }
        else if ( strcmp ( cmd, "SUB" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = SUB;
            continue;
        }
        else if ( strcmp ( cmd, "MUL" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = MUL;
            continue;
        }
        else if ( strcmp ( cmd, "DIV" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = DIV;
            continue;
        }
        else if ( strcmp ( cmd, "OUT" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = OUT;
            continue;
        }
        else if ( strcmp ( cmd, "HLT" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = HLT;
            continue;
        }
        else if ( strcmp ( cmd, "DUMP" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = DUMP;
            continue;
        }
        else if ( strcmp ( cmd, "PUSHR" ) == 0){
            char reg[5] = {};
            fscanf ( ptr_ass->orig_file, "%s", reg );
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = PUSHR;
            ptr_ass->ptr_cmd->size_code++;
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = int ( reg - 'A' );
            continue;
        }
        else if ( strcmp ( cmd, "POP" ) == 0){
            char reg[5] = {};
            fscanf ( ptr_ass->orig_file, "%s", reg );
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POP;
            ptr_ass->ptr_cmd->size_code++;
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = int ( reg - 'A' );
            continue;
        }
        else if ( strcmp ( cmd, "JMP" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JMP;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    else
                    {
                        where [ strlen ( where ) -1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                        break;
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
                break;
            }
            continue;
        }
        else if ( strcmp ( cmd, "JA" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JA;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    else
                    {
                        where [ strlen ( where ) -1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                        break;
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
                break;
            }
            continue;
        }
        else if ( strcmp ( cmd, "JAE" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JAE;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    else
                    {
                        where [ strlen ( where ) -1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                        break;
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
                break;
            }
            continue;
        }
        else if ( strcmp ( cmd, "JB" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JB;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    else
                    {
                        where [ strlen ( where ) -1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                        break;
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
                break;
            }
            continue;
        }
        else if ( strcmp ( cmd, "JBE" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JBE;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    else
                    {
                        where [ strlen ( where ) -1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                        break;
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
                break;
            }
            continue;
        }
        else if ( strcmp ( cmd, "JE" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JE;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    else
                    {
                        where [ strlen ( where ) -1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                        break;
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
                break;
            }
            continue;
        }
        else if ( strcmp ( cmd, "JNE" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JNE;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    //fprintf ( stderr, "label with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->labels_array[ i ].name[ strlen ( ptr_ass->labels_array[ i ].name ) - 1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    else
                    {
                        where [ strlen ( where ) -1 ] = '\0';
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                        break;
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
                break;
            }
            continue;
        }

        else fprintf ( stderr, "ERROR SYNTAX: cmd = %s\n", cmd );
        break;
    }
    ptr_ass->ptr_cmd->size_code++;

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

int ass_ctor ( assembler_t* ptr_ass )
{
fprintf ( stderr, "DEBUG0\n");
    ptr_ass->orig_file = fopen ( "original.asm", "r" );
    ptr_ass->compiled_file = fopen ( "compilated.txt", "w" );
    label_t labels_array [ LABELS_ARRAY_SIZE ] = {};
    fiksik_t fixup_label [ LABELS_ARRAY_SIZE ] = {};
    ptr_ass->fixup_label =  fixup_label;
    ptr_ass->labels_array = labels_array;
    cmd_t comandor = {};
    ptr_ass->ptr_cmd = &comandor;

fprintf ( stderr, "DEBUG0.1\n");

    ptr_ass->ptr_cmd->ptr_code = ( int* ) calloc ( MAX_CMD_SIZE, sizeof ( int ) );

fprintf ( stderr, "DEBUG1\n");

    ptr_ass->ptr_cmd->size_code = 0;

//fprintf ( stderr, " size : %d\n", ptr_ass->ptr_cmd->size_code );

    for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
    {
        ptr_ass->labels_array [ i ].ip = POISON_VALUE;
        strncpy ( ptr_ass->labels_array [ i ].name, "POISON", NAME_LENGTH );

        ptr_ass->fixup_label [ i ].ip_of_L_cmd = POISON_VALUE;
        strncpy ( ptr_ass->fixup_label [ i ].name, "POISON", NAME_LENGTH );
    }

    return FUNC_DONE;
}
