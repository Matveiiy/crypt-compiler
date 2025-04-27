//
// Created by motya on 01.04.2025.
//

#ifndef CRYPT_VM_H
#define CRYPT_VM_H

#include <cstdint>
#include <fstream>
#include <unordered_map>

namespace interpreter {
    using byte = uint8_t;

// supports: int, double, {char, objects, array, null}?
// something like this: https://www.lua.org/source/4.0/lopcodes.h.html
//  or like this: https://github.com/munificent/craftinginterpreters/blob/master/c/chunk.h
//  each instruction is 1 byte
    enum OpCode {

        //== Code block endings

        OP_STOP,
        OP_END,     // return from void function
        OP_RETURN,  // returns from function

        //== Pushes to stack

        OP_CONSTANT,   // C -> pushes constant on the stack
        OP_GET_LOCAL,  // pushes local var value TODO:
        OP_SET_LOCAL,  // set local var value from stack TODO:

        //== In stack arithmetics

        OP_ADDI,
        OP_SUBI,
        OP_DIVI,
        OP_MULI,
        OP_NEGATE,
        /*
                OP_ADDD,
                OP_SUBD,
                OP_DIVD,
                OP_MULD,
                not needed right now
        */

        //== Jumps

        // does pc += J if predicate on popped from stack is true
        OP_JMPNE,  // J -> not equal
        OP_JMPEQ,  // J -> equal
        OP_JMPLT,  // J -> less
        OP_JMPLE,  // J -> less or equals
        OP_JMPGT,  // J -> greater
        OP_JMPGE,  // J -> greater or equal

        OP_JMP,   // J -> does pc += J
        OP_JMPT,  // J -> does pc += J if popped != 0
        // OP_JMPF,//jump if false(or null)??? false == null????

        // TODO: clojures, classes, objects, arrays; (for, while - more specific)

    };
    static constexpr uint32_t NULL_VAL = 1;//001
    static constexpr uint32_t LAST3_PTR = 7;//111
    static constexpr uint32_t NORMAL_PTR_ = ~(LAST3_PTR);//111...000 - offset by 8

#define IS_INT(x) (((x).as_int & 1) == 0)
#define IS_CLASS_OBJ(x) (!IS_INT(x))
#define UNBOX_INT(x) ((x).as_int >> 1)
#define UNBOX_OBJ_OFFSET(x) ((x).as_uint & NORMAL_PTR_)
//dereference* object to do it!
#define GET_CLASSPTR_FROM_VAL(x, heap) static_cast<ObjInstance*>(*(UNBOX_OBJ_OFFSET(x) + (heap)))

    struct VMData;
    struct Value;
typedef void(*nativeFunc)(VMData&);

    struct ObjClass {
        nativeFunc operators[10];//better be multiple of 2(for 32 bit systems)
        std::unordered_map<int, int> layout;
        std::string name;
        int8_t nfields = 0;
    };
    struct ObjInstance {
        ObjClass* classptr;
    };
    struct ObjFunction {
        ObjClass* classptr;
        int params = 0;
    };
    struct Value {
        union {
            int32_t as_int;
            uint32_t as_uint;
        };

        inline static Value boxedInt(int32_t val) { return Value((uint32_t)(val << 1)); }
        inline static Value boxedOffset(uint32_t offset) { return Value(offset | 1); }
        explicit Value() : as_uint(NULL_VAL) {}
    private:
        explicit Value(uint32_t offset) : as_uint(offset)  {}
    };


// Codes and arguments sizes
    static constexpr int OPCODE_SIZE = 6;
    static constexpr int SIZE_C = 9;
    static constexpr int SIZE_B = 9;
    static constexpr int SIZE_A = 8;
    static constexpr int SIZE_AX = 26;
    static constexpr int SIZE_J = 18;

// Internal interpreter sizes
    static constexpr int MAX_CONSTANTS = 1 << 8;
    static constexpr int PROGRAM_STACK_SIZE = 1024;
    static constexpr int HEAP_SIZE = 1 << 30;
    struct VMData {
        Value constant_pool[MAX_CONSTANTS];
        int constant_count = 0;
        Value* sp = &stack[0];
        uint32_t ip = 0, bp = 0;
        uint32_t cur = 0;
        uint32_t code[1024];  // actually will contain list of function - each has a pointer to a block of code

        void* heap[1000];//HEAP_SIZE TODO: <- remove this
        Value stack[PROGRAM_STACK_SIZE];
        Value locals[1024];
    };

    bool less(Value val1, Value val2);

    bool less_equal(Value val1, Value val2);

    bool equal_val(Value val1, Value val2);

    bool nequal_val(Value val1, Value val2);

    bool greater(Value val1, Value val2);

    bool greater_equal(Value val1, Value val2);

    void run();

    void example();

    void jmp_example();

    VMData &vm_instance();

// inits from file(or any other stream); TODO (example - jvm)
    void init_vm(std::istream &in);
};  // namespace interpreter

#endif  // CRYPT_VM_H
