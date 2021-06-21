#include "asm.h"

#define PRINT_ERR(message) printf ("%s on line %d in function %s\n", message, __LINE__, __FUNCTION__)

#define STR_EQ(str1, str2, str1Len) !strncmp (str1, str2, str1Len)

const size_t MAX_LABEL_NUMS = 100;

void CallAsm (const char* fileName) {

    FILE* input = fopen (fileName, "r");
    if (input) {

        char*   buffer = ReadFile (input);
        assert (buffer);

        ReadAsmCode (buffer);
        
        free (buffer);

    } else {

        PRINT_ERR ("Can't open file");
        return;

    }

    fclose (input);
    
}

char* ReadFile (FILE* input) {

    assert (input);

    size_t  fileSize = GetFileSize (input);
    
    char*   buffer   = (char*)calloc (fileSize, sizeof (char));
    assert (buffer); 

    size_t check = fread  (buffer, sizeof (char), fileSize, input);

    return buffer;

}

size_t GetFileSize (FILE* input) {

    assert (input);

    fseek  (input, 0, SEEK_END);
    size_t fileSize = ftell (input);
    fseek  (input, 0, SEEK_SET);

    return fileSize;

}

void ImplemJmp (char** buffer, char* machineCode, size_t* byte, Label* table, size_t numLabels) {

    SkipTrash (buffer);

    size_t labelLength  = 0;
    char*  labelP       = *buffer;

    while (**buffer != '\n') {

        (*buffer)++;
        labelLength++;

    }

    for (size_t label = 0; label < numLabels; label++) {
        
        printf ("label name %s\n", table [label].name);
        printf ("label length %zu\n", labelLength);
        printf ("We're trying to check %s\n", labelP);

        if (STR_EQ(labelP, table [label].name, labelLength) && *(table [label].name + labelLength) == '\n') {
            
            char* converter = (char*)(&(table [label].byte));
            for (size_t bytec = 1; bytec <= sizeof (size_t); bytec++)
                machineCode [(*byte)++] = converter [sizeof (size_t) - bytec];
            return;

        }

    }

}

size_t FillLabels (char* buffer, Label* table) {

    size_t labelNum = 0;

    char    machineCode [1000]  = "";
    size_t  byte                = 0;

    char*   forJmpPBuffer       = buffer;
    Label   labelTable  [1000]  = {};
    size_t  numLabels           = 0;

    while (true) {

        SkipTrash (&buffer);

        if (*buffer == 0)
            break;

        if (*buffer == ':') {

            table [labelNum].byte = byte + 1;
            table [labelNum].name = buffer + 1;

            while (*buffer != '\n')
                buffer++;

            labelNum++;

            continue;

        }   
            #define DEF_CMD(name, asmFunc, cpuFunc) else if (STR_EQ (#name, buffer, strlen (#name))) {          \
                                                                                                                \
                                                                machineCode [byte++] = name;                    \
                                                                buffer      += strlen (#name);                  \
                                                                if (STR_EQ (#name, "j", 1)) {                   \
                                                                                                                \
                                                                    while (*buffer != '\n' && *buffer != '\0') {\
                                                                                                                \
                                                                        buffer++;                               \
                                                                                                                \
                                                                    }                                           \
                                                                    byte += 8;                                  \
                                                                    continue;                                   \
                                                                                                                \
                                                                }                                               \
                                                                asmFunc                                         \
                                                                continue;                                       \
                                                                                                                \
                                                            }                                                               
            #include "../commands"
            #undef  DEF_CMD

    }

    return labelNum;

}

void ReadAsmCode (char* buffer) {

    assert (buffer);

    char*   pBuffer = buffer;

    char    machineCode [1000]  = "";
    size_t  byte                = 0;

    char*   forJmpPBuffer       = buffer;
    Label   labelTable  [1000]  = {};
    size_t  numLabels           = FillLabels (forJmpPBuffer, labelTable);

    while (true) {
        
        printf ("here %c\n", *buffer);

        SkipTrash (&buffer);

        if (*buffer == 0)
            break;

        if (*buffer == ':') {

            while (*buffer != '\n')
                buffer++;

            continue;

        }   
            #define DEF_CMD(name, asmFunc, cpuFunc) else if (STR_EQ (#name, buffer, strlen (#name))) {          \
                                                                                                                \
                                                                machineCode [byte++] = name;                    \
                                                                buffer      += strlen (#name);                  \
                                                                asmFunc                                         \
                                                                continue;                                       \
                                                                                                                \
                                                            }                                                               
            #include "../commands"
            #undef  DEF_CMD

    }

    buffer = pBuffer;

    PrintMachineCode (machineCode, byte);

}

void PrintMachineCode (char* machineCode, size_t byteSize) {

    assert (machineCode);

    for (size_t i = 0; i < byteSize; i++) {

        printf ("%d", machineCode [i]);

    }

    FILE* outputFile = fopen ("output.fr", "wb");

    fwrite (machineCode, sizeof (char), byteSize, outputFile);

    fclose (outputFile);

}

void SkipTrash (char** buffer) {

    assert (buffer);

    while (isspace (**buffer))
        (*buffer)++;

    if (**buffer == '#') {

        while (**buffer != '\n')
            (*buffer)++;

    }

}

void PutModeIntoMachineCode (char* machineCode, size_t* byte, bool mem, bool reg, bool num) {

    machineCode [(*byte)++] = mem; 
    machineCode [(*byte)++] = reg;
    machineCode [(*byte)++] = num;

}

void WriteRegIntoMachineCode (char regName, char* machineCode, size_t* byte) {

    machineCode [(*byte)++] = regName;

}

int ReadRegFromBuffer (char** buffer) {

    if (false) {
    }
    #define DEF_REG(name) else if (STR_EQ(#name, *buffer, strlen (#name))) {                            \
                                                                                                        \
                                        (*buffer) += strlen (#name);                                    \
                                        return name;                                                    \
                                                                                                        \
                                    }   
    #include "../regs"
    #undef DEF_REG

}

void ConvertNumIntoMachineCode (double num, char* machineCode, size_t * byte) {

    char*  numConvertToChar = (char*)(&num);

    for (size_t byteNum = 0; byteNum < sizeof (double); byteNum++) {

        machineCode [(*byte)++] = numConvertToChar [byteNum];

    }

}

void WriteNumIntoMachineCode (char** buffer, char* machineCode, size_t* byte) {

    double num = ReadNumber (buffer);    
    ConvertNumIntoMachineCode (num, machineCode, byte);

}

void ReadParams (char** buffer, char* machineCode, size_t* byte) {
    
    SkipTrash (buffer);

    if (**buffer == '[') {          //operative mem

        (*buffer)++;

        if (**buffer == 'r') {      //register

            int regName = ReadRegFromBuffer (buffer);
            
            SkipTrash (buffer);

            if (**buffer == '+') {
                
                (*buffer)++;

                PutModeIntoMachineCode  (machineCode, byte, 1, 1, 1);
                WriteRegIntoMachineCode (regName, machineCode, byte);

                SkipTrash (buffer);

                WriteNumIntoMachineCode (buffer, machineCode, byte);

            } else {

                PutModeIntoMachineCode  (machineCode, byte, 1, 1, 0);
                WriteRegIntoMachineCode (regName, machineCode, byte);

            }

            SkipTrash (buffer);

            if (**buffer == ']') {

                (*buffer)++;

            } else {

                PRINT_ERR("Wrong command");
                return;

            }

        } else {

            PutModeIntoMachineCode  (machineCode, byte, 1, 0, 1);
            WriteNumIntoMachineCode (buffer, machineCode, byte);

            SkipTrash (buffer);

            if (**buffer == ']') {

                (*buffer)++;

            } else {

                PRINT_ERR("Wrong command");
                return;

            }

        }

    } else if (**buffer == 'r') {                           //register

        PutModeIntoMachineCode  (machineCode, byte, 0, 1, 0);
        int regName = ReadRegFromBuffer  (buffer);
        WriteRegIntoMachineCode (regName, machineCode, byte);

    } else if (isdigit (**buffer)) {                        //just a number

        PutModeIntoMachineCode  (machineCode, byte, 0, 0, 1);
        WriteNumIntoMachineCode (buffer, machineCode, byte);

    } else
        PutModeIntoMachineCode  (machineCode, byte, 0, 0, 0);

}

double ReadNumber (char** buffer) {

    double result = 0;

    while (isdigit (**buffer)) {

        result *= 10;
        result += (**buffer) - '0';

        (*buffer)++;

    }

    return result;

}
