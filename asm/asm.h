#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

enum CommandsNum {

    #define DEF_CMD(name, asmFunc, cpuFunc) name, 
    #include "../commands"
    #undef  DEF_CMD

};

enum Regs {

    #define DEF_REG(name) name,
    #include "../regs"
    #undef  DEF_REG

};

struct Label {

    char*  name;
    size_t byte;

};

void    CallAsm             (const char* fileName);

char*   ReadFile            (FILE*  input);
size_t  GetFileSize         (FILE*  input);

void    ReadAsmCode         (char*  buffer);
void    PrintMachineCode    (char*  machineCode, size_t byteSize);

void    SkipTrash           (char** buffer);

void    ReadParams          (char** buffer, char* machineCode, size_t* byte);
void    ImplemJmp           (char** buffer, char* machineCode, size_t* byte, Label* table, size_t numLabels);

double  ReadNumber          (char** buffer);
