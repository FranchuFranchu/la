from typing import List, Tuple, Callable
from enum import Enum
import re
import math


class LVMOpcode(Enum):
    EXIT_TO_SYSTEM = 0
    MOV = 1
    DEBUG_DUMP_OPERATOR_0 = 2
    JUMP = 3
    JUMP_CONDITION = 4
    BINARY_OPERATION_NOCHANGE = 5
    BINARY_OPERATION = 6
    IN = 7
    OUT = 8
    PUSH = 9
    POP = 10
    CALL = 11
    RETURN = 12

class LVMArgumentTypes(Enum):
    REGISTER_A = 1
    REGISTER_B = 2
    REGISTER_C = 3
    REGISTER_D = 4
    REGISTER_E = 5
    REGISTER_F = 6
    REGISTER_G = 7
    REGISTER_H = 8
    REGISTER_I = 9
    REGISTER_FLAGS = 0xA

    MEMORY_RELATIVE = 0xb
    MEMORY_ABSOLUTE = 0xc
    SPECIAL_IMMEDIATE = 0xd

    MEMORY_OFFSET_FROM_REGISTER = 0xe
    MEMORY_FROM_REGISTER = 0xf

    COMPILER_LABEL = 0x100
    COMPILER_ARGUMENT_LITERAL_BYTE = 0x101
    COMPILER_VARIABLE = 0x102
    COMPILER_IN_STACK = 0x103

LVMByteArgumentCount = {
    LVMArgumentTypes.MEMORY_RELATIVE: 4,
    LVMArgumentTypes.MEMORY_ABSOLUTE: 4,
    LVMArgumentTypes.SPECIAL_IMMEDIATE: 4,
}


class LVMStatement():
    pass

class LaCompilerBatchBytecode(LVMStatement):
    bytecode: List[LVMStatement]

    def __init__(self):
        self.bytecode = []

    # returns bytes object
    def compile(self):
        return LVMCompiler(self.bytecode).compile()

class LaCompilerBatchMacro(LaCompilerBatchBytecode):
    """Class responsible for handling data"""
    has_args:      List[Tuple[LVMArgumentTypes, object]]
    requires_args: List[Tuple[LVMArgumentTypes, object]]

    def __init__(self, bytecode: list, requires_args):
        self.has_args = []
        self.bytecode = bytecode
        self.requires_args = requires_args


    def copy(self, has_args):
        n =  type(self)(bytecode=self.bytecode,requires_args=self.requires_args)
        n.has_args = has_args
        return n

    def compile(self, labels_to_fill):
        print(self.has_args, self.requires_args)
        assert len(self.has_args) == len(self.requires_args)

        final_bytecode = []

        for has, requires in zip(self.has_args, self.requires_args):
            if has == requires:
                continue
            final_bytecode.append(LVMCompilableStatement(LVMOpcode.PUSH, [requires]))
            final_bytecode.append(LVMCompilableStatement(LVMOpcode.MOV, [has, requires]))

        final_bytecode.extend(self.bytecode)

        for has, requires in list(zip(self.has_args, self.requires_args))[::-1]:
            if has == requires:
                continue
            final_bytecode.append(LVMCompilableStatement(LVMOpcode.POP, [requires]))

        return LVMCompiler(final_bytecode).compile_code()


class LVMLabel(LVMStatement):
    name: str
    def __init__(self, name):
        self.name = name

class LVMCompilableStatement(LVMStatement):
    """
    docstring for LVMCompilableStatement
    """
    
    args: List[Tuple[LVMArgumentTypes, object]]
    opcode: LVMOpcode


    def __init__(self, opcode, args = []):
        super(LVMCompilableStatement, self).__init__()
        self.args = args
        self.opcode = opcode

    def compile(self, pending_labels):
        opcode = self.opcode.value.to_bytes(1, byteorder="little")

        argument_types = bytearray(math.ceil(len(self.args) / 2))

        args_place = bytes()
        i = 0
        for argtype, argval in self.args:
            argsize = LVMByteArgumentCount.get(argtype.value) or 0

            if argtype == LVMArgumentTypes.COMPILER_LABEL:
                pending_labels[argval] = len(opcode) + len(argument_types) + len(args_place)
                argtype = LVMArgumentTypes.SPECIAL_IMMEDIATE
                argval = 0

            argument_types[i//2] |= (argtype).value << ((i % 2) * 4)


            if argtype == LVMArgumentTypes.COMPILER_ARGUMENT_LITERAL_BYTE:
                args_place += argval.to_bytes(1, byteorder="little")

            elif argtype == LVMArgumentTypes.SPECIAL_IMMEDIATE:
                args_place += argval.to_bytes(4, byteorder="little")

            elif argval is None:
                pass
            else:
                raise ValueError("Unknown argument type/value pair")
            i += 1
        return opcode + argument_types+ args_place




class LVMCompiler(object):
    """docstring for LVMCompiler"""
    statements: List[LVMStatement]
    def __init__(self, statements = []):
        super(LVMCompiler, self).__init__()
        self.statements = statements

    def compile_code(self):
        pending_labels = {}
        done_labels = {}
        bytecode = bytes()
        for i in self.statements:
            this_pending_labels = {}
            if isinstance(i, LVMLabel):
                done_labels[i.name] = len(bytecode)
                continue

            this_bytecode = i.compile(this_pending_labels)
            for k, v in this_pending_labels.items():
                if k in done_labels:
                    this_bytecode = bytearray(this_bytecode)
                    this_bytecode[v:v+4] = done_labels[k].to_bytes(this_bytecode)
                    this_bytecode = bytes(this_bytecode)
                else:
                    pending_labels[k] = v + len(bytecode)

            bytecode += this_bytecode

        return bytecode



    def save_to(self, file):
        file.write((4).to_bytes(4,byteorder="little"))
        file.write(self.compile_code())

    @classmethod
    def from_string(cls, s):
        self = cls()
        for line in s.splitlines():
            if line.strip() == "":
                continue
            match = re.match(r"([0-9a-zA-Z\.]+):", line)
            if match:
                label_name = match.group(0)
                self.statements.append(LVMLabel(label_name))
                continue
            tokens = line.split(' ')
            opcode = getattr(LVMOpcode, tokens[0])
            def _eval_num(token):
                if re.match(r"0x([0-9a-zA-Z]+)", token):
                    return int(token, 16)
                elif re.match(r"([0-9]+)", token):
                    return int(token, 10)

                raise ValueError("Invalid token: " + token)

            args = []
            for token in tokens[1:]:
                match = re.match(r"\[(.+)\]", token)
                if match:
                    i = _eval_num(match.group(1))
                    args.append((LVMArgumentTypes.SPECIAL_IMMEDIATE, i))
                    continue
                match = re.match(r"[A-I]", token)
                if match:
                    args.append((getattr(LVMArgumentTypes, "REGISTER_%s" % token.upper()), None))
                    continue

                match = re.match(r"([0-9]+)|(0x([0-9][a-zA-Z]+))", token)
                if match:
                    i = _eval_num(match.group(0))
                    args.append((LVMArgumentTypes.SPECIAL_IMMEDIATE, i))
                    continue

                match = re.match(r"([0-9a-zA-Z\.]+)", token)
                if match:
                    args.append((LVMArgumentTypes.COMPILER_LABEL, match))
                    continue

            self.statements.append(LVMCompilableStatement(
                opcode,
                args,
            ))
        return self
