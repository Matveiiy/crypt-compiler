//
// Created by motya on 12.04.2025.
//
#include "vm.h"
#include "utils.h"

using namespace interpreter;

struct BytecodeHolder {
    std::unordered_map<std::string, int> labels;
    std::vector<std::pair<std::string, int>> pending_labels;
    std::vector<uint32_t> code;

    BytecodeHolder() {}

    BytecodeHolder &emit(OpCode op) {
        code.push_back(op);
        return *this;
    }

    BytecodeHolder &emitC(OpCode op, uint32_t j) {
        code.push_back(op + (j << OPCODE_SIZE));
        return *this;
    }

    BytecodeHolder &label(std::string label) {
        if (labels.contains(label))
            throw std::runtime_error("label already exists");
        labels[label] = code.size();
        return *this;
    }


    BytecodeHolder &jmp(OpCode jmpType, std::string label) {
        return pending_labels.emplace_back(label, code.size()), emit(jmpType);
    }

    BytecodeHolder &emitJ(OpCode op) {
        code.push_back(op);
        return *this;
    }

    BytecodeHolder &update() {
        while (!pending_labels.empty()) {
            auto &cur = pending_labels.back();
            pending_labels.pop_back();
            if (!labels.contains(cur.first)) throw std::runtime_error("label not found " + cur.first);
            auto &pos = labels[cur.first];
            code[cur.second] += (pos - cur.second - 1) << OPCODE_SIZE;
        }
        return *this;
    }
};

template<class T>
void run_checked(std::vector<Value> constants, BytecodeHolder &h, T check) {
    VMData &vm = vm_instance();
    vm.heap[0] = new ObjClass{.name = "NULL"};
    h.emit(OP_STOP);
    memcpy(vm.constant_pool, constants.data(), constants.size() * sizeof(Value));
    std::copy(h.code.begin(), h.code.end(), vm.code);
    vm.sp = vm.stack;
    vm.ip = 0;
    run();
    check(vm);
}

void run1(std::vector<Value> constants, BytecodeHolder &h, std::vector<Value> stack) {
    return run_checked(std::move(constants), h, [&](VMData &vm) {
        ASSERT_TRUE(vm.sp - vm.stack == stack.size());
        for (int i = 0; i < stack.size(); ++i) {
            std::cout << vm.stack[i].as_int << std::endl;
            ASSERT_TRUE(equal_val(stack[i], vm.stack[i]));
        }
    });
}

using VmExampleStackTestSuite = Test;


TEST(VmExampleStackTestSuite, ExampleTest) {
    //example test
    run1({
                 Value::boxedInt(3),
                 Value::boxedInt(2),
                 Value::boxedInt(100000),
                 Value::boxedInt(5),
         },
         BytecodeHolder()
                 .emitC(OP_CONSTANT, 0)
                 .emitC(OP_CONSTANT, 1)
                 .emitC(OP_CONSTANT, 3)
                 .jmp(OP_JMPNE, "l1")
                 .emitC(OP_CONSTANT, 2)
                 .label("l1")
                 .emitC(OP_CONSTANT, 1)
                 .emit(OP_ADDI)
                 .update(), {
                 Value::boxedInt(5),
         });
    //4 * 2 * 3 + 4 + 8 - 7 * 7 = 24 + 12 - 49 = -13
    run1({
                 Value::boxedInt(2),
                 Value::boxedInt(3),
                 Value::boxedInt(4),
                 Value::boxedInt(8),
                 Value::boxedInt(7)
         },
         BytecodeHolder()
                 .emitC(OP_CONSTANT, 4)
                 .emitC(OP_CONSTANT, 4)
                 .emit(OP_MULI)
                 .emitC(OP_CONSTANT, 2)
                 .emitC(OP_CONSTANT, 0)
                 .emit(OP_MULI)
                 .emitC(OP_CONSTANT, 1)
                 .emit(OP_MULI)
                 .emitC(OP_CONSTANT, 2)
                 .emit(OP_ADDI)
                 .emitC(OP_CONSTANT, 3)
                 .emit(OP_ADDI)
                 .emit(OP_SUBI)
                 .update(), {
                 Value::boxedInt(-13)
         });

}