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
    HLT     = 31,
    DUMP    = 7,
    POP     = 8,
    JMP     = 10,
    JA      = 11,
    JAE     = 12,
    JB      = 13,
    JBE     = 14,
    JE      = 15,
    JNE     = 16,
    CALL    = 17,
    RET     = 18,
    DRAW    = 20,
};

enum PUSH_TYPES
{
    TYPE_CONSTANT = 1 << 5,
    TYPE_REGISTER = 1 << 6,
    TYPE_MEMORY   = 1 << 7,
    TYPE_FULL_BIT = TYPE_CONSTANT | TYPE_REGISTER | TYPE_MEMORY,
};

const int READING = 1;
const int VOZRAST_SOGLASIYA_V_KITAYE = 8;
const int START_STK_CAPACITY = VOZRAST_SOGLASIYA_V_KITAYE; // TODO: SUKA
const int FUNC_DONE = 0;
const int LABELS_ARRAY_SIZE = 10;
const int NAME_LENGTH = 10;
const int MAX_CMD_LENGTH = 10;
const int POISON_VALUE = -666;
const int MAX_CMD_SIZE = 1000;
const int REG_ARRAY_CAPACITY = 7;
const int ARG_STR_CAPACITY = 20;
const int ERROR_INPUT = 52;

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

struct cmd_t
{
    int* ptr_code;
    int size_code;
};

struct assembler_t
{
    FILE*       orig_file;
    FILE*       compiled_file;
    label_t*    labels_array;
    fiksik_t*   fixup_label;
    cmd_t*        ptr_cmd;
};

int  file_opener         ();
int  code_translator     ( assembler_t* ptr_ass );
int  create_new_label    ( assembler_t* ptr_ass, char label[NAME_LENGTH], int counter );
int  create_new_fiksik   ( assembler_t* ptr_ass, char label[NAME_LENGTH], int counter );
int  lobotomy            ( assembler_t* ptr_ass );
int  ass_ctor            ( assembler_t* ptr_ass );
int  make_endfile        ( assembler_t* ptr_ass );
bool check_if_reg        ( char* tmp_ptr );
int  cmd_check           ( int command );


#endif
