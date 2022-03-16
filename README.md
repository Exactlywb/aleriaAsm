# ASM emulator

## Project description

My own assembler. You can see examples in ```input``` file. Assembler emulator requires an input file and produces a binary file as an output.

## Project implementation

The main idea is code generation using preprocessor. e.g.

asm.h
```cpp
enum CommandsNum {

    #define DEF_CMD(name, asmFunc, cpuFunc) name, 
    #include "../commands"
    #undef  DEF_CMD

};
```
commands
```
...
DEF_CMD (in  , {}, {})
DEF_CMD (out , {}, {})

DEF_CMD (add , {}, {})
DEF_CMD (sub , {}, {})
DEF_CMD (mul , {}, {})
DEF_CMD (sep , {}, {})

DEF_CMD (sqrt, {}, {})
...
```

So after preprocessing code in ```asm.h``` converts into

```cpp
enum CommandsNum {
    ...
    in,
    out,
    add,
    sub,
    mul,
    sep,
    sqrt,
    ...
};
```

It means that we can write implementation in single file and then use it everywhere we want to generate the same code.

## How to build

After git clone:
```
make
./main
```
