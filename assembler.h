#ifndef ASSEMBLER_HEADER
#define ASSEMBLER_HEADER

enum FUNC_INITS
{
    PUSH    = 1,
    SUM     = 2,
    SUB     = 3,
    MUL     = 4,
    DIV     = 5,
    OUT     = 6,
    HLT     = -1,
    DUMP    = 69,
    PUSHR   = 7,
    POP     = 8,
    JMP     = 10,
    JA      = 11,
    JAE     = 12,
    JB      = 13,
    JBE     = 14,
    JE      = 15,
    JNE     = 16
};
const int READING = 1;
const int VOZRAST_SOGLASIYA_V_KITAYE = 14;
const int START_STK_CAPACITY = VOZRAST_SOGLASIYA_V_KITAYE; // TODO: SUKA
const int FUNC_DONE = 0;
const int LABELS_ARRAY_SIZE = 10;
const int NAME_LENGTH = 10;
const int MAX_CMD_LENGTH = 10;
const int POISON_VALUE = -1;
const int MAX_CMD_SIZE = 500;

struct label_t
{
    char        name[ NAME_LENGTH ];
    int         ip;
};

struct fiksik_t
{
    char        name [ NAME_LENGTH ];
    int         ip_of_L_cmd;
};

struct assembler_t
{
    FILE*       orig_file;
    FILE*       compiled_file;
    label_t*    labels_array;
    fiksik_t*   fixup_label;
    int*        ptr_cmd;
};

int file_opener         ();
int code_translator     ( assembler_t* ptr_ass );
int create_new_label    ( assembler_t* ptr_ass, char label[NAME_LENGTH], int counter );
int create_new_fiksik    ( assembler_t* ptr_ass, char label[NAME_LENGTH], int counter );
int lobotomy            ( assembler_t* ptr_ass );

#endif
