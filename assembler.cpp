#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>

#include "assembler.h"

//asm input_file.txt output.bin
//spu output.bin


int file_opener ()
{
    assembler_t sossembler = {};
    assembler_t* ptr_ass = &sossembler;
    cmd_t comandor = {};
    ptr_ass->ptr_cmd = &comandor;

    ass_ctor ( ptr_ass );

    code_translator ( ptr_ass );
    //fprintf ( stderr, " size : %d\n", ptr_ass->ptr_cmd->size_code );

    /*for ( int i = 0; i < ptr_ass->ptr_cmd->size_code; i++ )
    {
        printf ( "%d: %d\n", i+1 , ptr_ass->ptr_cmd->ptr_code [ i ]);
    }*/

    for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
    {
        if ( strcmp ( ptr_ass->fixup_label [ i ].name, "POISON" ) != 0 )
        {
//fprintf ( stderr, "fixup_name: %s\n", ptr_ass->fixup_label[ i ].name );
            for ( int j = 0; j < LABELS_ARRAY_SIZE; j++ )
            {
//fprintf ( stderr, "%s and %s\n", ptr_ass->fixup_label [ i ].name, ptr_ass->labels_array [ j ].name );
                if ( strcmp ( ptr_ass->fixup_label [ i ].name, ptr_ass->labels_array [ j ].name ) == 0 )
                {
/*fprintf ( stderr, "fixup_name: '%s' to '%s', %d to %d\n", ptr_ass->fixup_label[ i ].name, ptr_ass->labels_array [ j ].name,\
                                                          ptr_ass->ptr_cmd->ptr_code [ ptr_ass->fixup_label[ i ].ip_of_L_cmd ], ptr_ass->labels_array [ j ].ip);*/
                    //fprintf ( stderr, "ip of L comand: %d\n", ptr_ass->fixup_label[ i ].ip_of_L_cmd );
                    ptr_ass->ptr_cmd->ptr_code [ ptr_ass->fixup_label[ i ].ip_of_L_cmd ] = ptr_ass->labels_array [ j ].ip;
                    break;
                }
            }
        }

    }

    /*for ( int i = 0; i < ptr_ass->ptr_cmd->size_code; i++ )
    {
        printf ( "%d: %d\n", i+1, ptr_ass->ptr_cmd->ptr_code [ i ]);
    }*/

    make_endfile ( ptr_ass );

    fclose ( ptr_ass->orig_file );
    fclose ( ptr_ass->compiled_file );
    return FUNC_DONE;
}

int code_translator ( assembler_t* ptr_ass )
{
    char cmd[MAX_CMD_LENGTH] = {};
    ptr_ass->ptr_cmd->size_code++;

    while  ( fscanf ( ptr_ass->orig_file, "%s", cmd ) != EOF )
    {
fprintf ( stderr, "cmd: '%s'\n", cmd );

fprintf ( stderr, "printed: '%d'\n", ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code -1] );
        if ( ( strcmp ( cmd, "PUSH" ) == 0 ) || ( strcmp ( cmd, "POP" ) == 0 ) )
        {
            int command = 0;

            if ( strcmp ( cmd, "PUSH" ) == 0 )
                command |= PUSH;
            else
                command |= POP;

            int imconst = POISON_VALUE;
            int regstr = POISON_VALUE;

            int amnt_of_scanned_numbers = 0;

            char arg_str[ ARG_STR_CAPACITY ] = {};
            char tmp_str[ ARG_STR_CAPACITY ] = {};

            char *scanning_ptr = NULL;


//fprintf ( stderr, "DEBUG_CODE_TRANSLATOR( PUSH1 )\n");
            fscanf ( ptr_ass->orig_file, "%20[^\n]", arg_str );
//fprintf ( stderr, "arg_str: '%s'\n", arg_str );

            scanning_ptr = strchr ( arg_str, '[' );

            if ( scanning_ptr != NULL )
            {
                scanning_ptr++;
                while ( *scanning_ptr == ' ' )
                    scanning_ptr++;
                command |= TYPE_MEMORY;
            }
            else
            {
                scanning_ptr = arg_str + 1;
            }

//TODO make more func

            if (sscanf( scanning_ptr, "%[A-Z]%n", tmp_str, &amnt_of_scanned_numbers ) == 1)
            {
//fprintf ( stderr, "tmp_str: '%s'\n", tmp_str );
                if ( check_if_reg ( tmp_str ) )
                {
                    //lobotomy ( ptr_ass );
//fprintf ( stderr, "REGISTER1\n");
                    command |= TYPE_REGISTER;
//fprintf ( stderr, "amnt of scanned numbers: %d, '%s'\n", amnt_of_scanned_numbers, tmp_str );
                    if ( amnt_of_scanned_numbers == 2 )
                    {
//fprintf ( stderr, "REGISTER2\n");
                        command |= TYPE_REGISTER;
//fprintf ( stderr, "register: %s\n", tmp_str );
                        regstr = tmp_str [ 0 ] - 'A';

                        scanning_ptr = strchr ( arg_str, '+' );

                        if ( scanning_ptr != NULL )
                        {
//fprintf ( stderr, "REGISTER3\n");
                            scanning_ptr++;
                            while ( *scanning_ptr == ' ' )
                                scanning_ptr++;
                            if (sscanf( scanning_ptr, "%d", &imconst ) == 1)
                            {
//fprintf ( stderr, "REGISTER3.1\n");
                                command |= TYPE_CONSTANT;
                            }
                            else
                            {
                                printf ( "WRONG INPUT: '%s'\n", scanning_ptr );
                                return ERROR_INPUT;
                            }
                        }
                    }
                    else
                    {
                        printf ( "Register have more than two characters: '%s'\n", tmp_str );
                        return ERROR_INPUT;
                    }
                }
                else
                {
                    printf ( "REGISTER '%s' not found\n", tmp_str );
                    return ERROR_INPUT;
                }
            }
            else
            {
//fprintf ( stderr, "REGISTER4\n");
                if (sscanf( scanning_ptr, "%d", &imconst ) != 1)
                {
                    printf ( "WRONG INPUT: '%s'\n", scanning_ptr );
                }
                else
                    command |= TYPE_CONSTANT;
            }

fprintf ( stderr, "command =  %d\n", command );

            if ( cmd_check ( command ) == ERROR_INPUT )
                printf ( "LEARN HOW TO USE 'POP'!( ip = %d )\n", ptr_ass->ptr_cmd->size_code );

//fprintf ( stderr, "operation: %d\n", ( command & TYPE_MEMORY ) );

fprintf ( stderr, "register number : '%d'\n", regstr );


            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code++ ] = command;

            if ( regstr != POISON_VALUE )
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code++ ] = regstr;
            }
                //fprintf ( stderr, "regstr = POISON_VALUE\n");
            if ( imconst != POISON_VALUE )
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code++ ] = imconst;
            }
               //fprintf ( stderr, "imconst = POISON_VALUE\n");
            //ptr_ass->ptr_cmd->ptr_code++;
fprintf ( stderr, "imconst = %d, %d\n--------------------\n", imconst, ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code - 1 ] );
            continue;
        }
        else if ( strcmp ( cmd, "DRAW" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = DRAW;
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }
        else if ( strcmp ( cmd, "SUM" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = SUM;
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }
        else if ( strcmp ( cmd, "SUB" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = SUB;
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }
        else if ( strcmp ( cmd, "MUL" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = MUL;
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }
        else if ( strcmp ( cmd, "DIV" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code++ ] = DIV;
            continue;
        }
        else if ( strcmp ( cmd, "OUT" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = OUT;
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }
        else if ( strcmp ( cmd, "HLT" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = HLT;
            continue;
        }
        else if ( strcmp ( cmd, "DUMP" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = DUMP;
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }

        else if ( strcmp ( cmd, "JMP" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JMP;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
//fprintf ( stderr, "where: '%s'\n", where);
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                where [ strlen( where ) -1  ] = '\0';
                int counter = 0;
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    counter++;
                    //fprintf ( stderr, "*************************************\nlabel with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    if ( counter == LABELS_ARRAY_SIZE )
                    {
                        ////printf ( "ELSE STARTED!\n" );
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
            }
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }
        else if ( strcmp ( cmd, "JA" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JA;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
//fprintf ( stderr, "where: '%s'\n", where);
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                where [ strlen( where ) -1  ] = '\0';
                int counter = 0;
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    counter++;
                    //fprintf ( stderr, "*************************************\nlabel with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    if ( counter == LABELS_ARRAY_SIZE )
                    {
                        //printf ( "ELSE STARTED!\n" );
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
            }
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }
        else if ( strcmp ( cmd, "JAE" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JAE;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
//fprintf ( stderr, "where: '%s'\n", where);
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                where [ strlen( where ) -1  ] = '\0';
                int counter = 0;
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    counter++;
                    //fprintf ( stderr, "*************************************\nlabel with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    if ( counter == LABELS_ARRAY_SIZE )
                    {
                        //printf ( "ELSE STARTED!\n" );
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
            }
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }
        else if ( strcmp ( cmd, "JB" ) == 0){//TODO make as func
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JB;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );//TODO in jmp/call there is no ':'
            ptr_ass->ptr_cmd->size_code++;
//fprintf ( stderr, "where: '%s'\n", where);//TODO strlen as peremennaya
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                where [ strlen( where ) -1  ] = '\0';
                int counter = 0;
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    counter++;
                    //fprintf ( stderr, "*************************************\nlabel with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    if ( counter == LABELS_ARRAY_SIZE )
                    {
                        //printf ( "ELSE STARTED!\n" );
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                    }
                }
            }
            /*int checker_if_adress = 1;
            int checker_if_register = 1;

            for ( int i = 0; i < NAME_LENGTH; i++ )
            {
                if ( isdigit ( where [ i ] ) != 0 )
                {
                    checker_if_adress *= 0;
                    break;
                }
                if ( isalfa ( where [ i ] ) != 0 )
                {
                    checker_if_register *= 0;
                    break;
                }
            }
            if ( checker_if_adress == 1 )
            {
                    int counter = 0;
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    counter++;
                    //fprintf ( stderr, "*************************************\nlabel with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    if ( counter == LABELS_ARRAY_SIZE )
                    {
                        //printf ( "ELSE STARTED!\n" );
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                    }
                }
            }*/
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
            }
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }
        else if ( strcmp ( cmd, "JBE" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JBE;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
//fprintf ( stderr, "where: '%s'\n", where);
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                where [ strlen( where ) -1  ] = '\0';
                int counter = 0;
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    counter++;
                    //fprintf ( stderr, "*************************************\nlabel with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    if ( counter == LABELS_ARRAY_SIZE )
                    {
                        //printf ( "ELSE STARTED!\n" );
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
            }
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }
        else if ( strcmp ( cmd, "JE" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JE;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
//fprintf ( stderr, "where: '%s'\n", where);
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                where [ strlen( where ) -1  ] = '\0';
                int counter = 0;
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    counter++;
                    //fprintf ( stderr, "*************************************\nlabel with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    if ( counter == LABELS_ARRAY_SIZE )
                    {
                        //printf ( "ELSE STARTED!\n" );
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
            }
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }
        else if ( strcmp ( cmd, "JNE" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = JNE;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
//fprintf ( stderr, "where: '%s'\n", where);
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                where [ strlen( where ) -1  ] = '\0';
                int counter = 0;
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    counter++;
                    //fprintf ( stderr, "*************************************\nlabel with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    if ( counter == LABELS_ARRAY_SIZE )
                    {
                        //printf ( "ELSE STARTED!\n" );
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
            }
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }

        else if ( strcmp ( cmd, "CALL" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = CALL;
            char where[ NAME_LENGTH ] = {};
            fscanf ( ptr_ass->orig_file, "%s", where );
            ptr_ass->ptr_cmd->size_code++;
//fprintf ( stderr, "where: '%s'\n", where);
            if ( where [ strlen( where ) - 1 ] == ':')
            {
                where [ strlen( where ) -1  ] = '\0';
                int counter = 0;
                for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
                {
                    counter++;
                    //fprintf ( stderr, "*************************************\nlabel with name '%s' and '%s'\n", where, ptr_ass->labels_array[ i ].name  );
                    //fprintf ( stderr, "strcmp result: %d\n",  strcmp ( where, ptr_ass->labels_array[ i ].name )  );
                    if ( strcmp ( where, ptr_ass->labels_array[ i ].name ) == 0 )
                    {
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = ptr_ass->labels_array[ i ].ip;
                        break;
                    }
                    if ( counter == LABELS_ARRAY_SIZE )
                    {
                        //printf ( "ELSE STARTED!\n" );
                        ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = POISON_VALUE;
                        create_new_fiksik ( ptr_ass, where, ptr_ass->ptr_cmd->size_code );
                    }
                }
            }
            else
            {
                ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code ] = atoi ( where );
            }
            ptr_ass->ptr_cmd->size_code++;
            continue;
        }
        else if ( strcmp ( cmd, "RET" ) == 0){
            ptr_ass->ptr_cmd->ptr_code [ ptr_ass->ptr_cmd->size_code++ ] = RET;
            continue;
        }

        else if ( cmd [ strlen( cmd ) - 1 ] == ':')
        {
            create_new_label ( ptr_ass, cmd, ptr_ass->ptr_cmd->size_code );

            ////lobotomy ( ptr_ass );

            continue;
        }

        else fprintf ( stderr, "ERROR SYNTAX: cmd = %s\nip = %d", cmd, ptr_ass->ptr_cmd->size_code );
        break;
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
            label [ strlen ( label ) - 1 ] = '\0';
            strncpy ( ptr_ass->labels_array[ i ].name, label, NAME_LENGTH );
            ptr_ass->labels_array[ i ].ip = counter;
            break;
        }
    }
    //lobotomy ( ptr_ass );

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
    //lobotomy ( ptr_ass );

    return FUNC_DONE;
}

int lobotomy ( assembler_t* ptr_ass )
{
    printf ( "              LABELS        \n");
    for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
    {
        printf (  "label: name - '%s', ip = %d\n", ptr_ass->labels_array[i].name, ptr_ass->labels_array[i].ip );
    }
    printf ( "-----------------------------\n"
             "             FIKSIKS          \n");
    for ( int i = 0; i < LABELS_ARRAY_SIZE; i++ )
    {
        printf (  "fiksik: name - '%s', ip = %d\n", ptr_ass->fixup_label[i].name, ptr_ass->fixup_label[i].ip_of_L_cmd );
    }

    printf ( "-------------------------------\n");

    return FUNC_DONE;
}

int ass_ctor ( assembler_t* ptr_ass )
{
    ptr_ass->orig_file = fopen ( "original.asm", "r" );
    ptr_ass->compiled_file = fopen ( "compilated.txt", "w" );

    ptr_ass->fixup_label = ( fiksik_t* ) calloc ( LABELS_ARRAY_SIZE, sizeof ( label_t ) ) ; // TODO: replace with calloc
    ptr_ass->labels_array = ( label_t* ) calloc ( LABELS_ARRAY_SIZE, sizeof ( label_t ) ) ; // TODO: replace with calloc

    ptr_ass->ptr_cmd->ptr_code = ( int* ) calloc ( MAX_CMD_SIZE, sizeof ( int ) );

    ptr_ass->ptr_cmd->size_code = -1;

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

int make_endfile ( assembler_t* ptr_ass )
{
    for ( int i = 0; i < ptr_ass->ptr_cmd->size_code + 1; i++ )
    {
        fprintf ( ptr_ass->compiled_file, "%d ", ptr_ass->ptr_cmd->ptr_code[ i ] );
    }

    return FUNC_DONE;
}

bool check_if_reg ( char* tmp_ptr )
{
    return ( strlen( tmp_ptr ) >= 2 && tmp_ptr[ 1 ] == 'X' && tmp_ptr[ 0 ] >= 'A' && tmp_ptr[ 0 ] <= 'A' + REG_ARRAY_CAPACITY);
}

int cmd_check ( int command )
{
//fprintf ( stderr, " 1: %d, 2: %d\n", ( command & POP ), ( command & 232 ) );
    if ( ( ( command & 232 ) == 104 ) || ( ( command & 232 ) == 40 ) )
        return ERROR_INPUT;
    return FUNC_DONE;
}
