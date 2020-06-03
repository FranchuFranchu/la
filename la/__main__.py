from abc import ABC
import pprint
import logging
from typing import List, Tuple, Dict, Callable

from lark import Lark, Tree, Token

from la.parse_tabs import tabs_to_codeblocks
from la.elf_utils import ElfFile, ElfSection
from la.la_builtins import LaInteger, LaBoolean, LaString, LaFunction, LaArgument, la_builtin_macros
import la.errors as errors
from la.LVMCompiler import LVMCompiler, LVMArgumentTypes, LVMCompilableStatement, LVMOpcode

class LaCompilerEnviroment(object):
    """docstring for LaCompilerEnviroment"""
    vars_where: Dict[str, Tuple[LVMArgumentTypes, object]]
    def __init__(self):
        super(LaCompilerEnviroment, self).__init__()
        self.compiler = LVMCompiler()
        self.vars_where = dict()

    def alloc_var(self, varname):

        valid_place = None
        for k in list(LVMArgumentTypes):
            if k.name.startswith("REGISTER_") and k not in [i[0] for i in self.vars_where.values()]:
                print(k)
                valid_place = (k, None)
                break

        if valid_place is None:
            raise NotImplementedError

        self.vars_where[varname] = valid_place
        return self.vars_where[varname]

    def free_var(self, varname):
        del self.vars_where[varname]
        return self.vars_where[varname]
precedence = {
    " cat ": 2,
    "+": 2,
    "-": 2,
    " rep ": 1,
    "*": 1,
    "/": 1,
    "**": 0,
    ".": -1,
    ":": -2,

}


def executeFunction(env, function_name, args):
    print(args)
    assert args.data == "function_args"
    has_args = []
    for i in args.children[0].children:
        has_args.append(env.vars_where[str(i)])

    f = la_builtin_macros[function_name].copy(has_args)
    env.compiler.statements.append(f)

def evaluate(tree: Tree, env: LaCompilerEnviroment):

    if len(tree.children) == 1:
        return evaluate(tree.children[0], env)
    elif tree.data == "function_call":
        assert len(tree.children) == 2
        assert tree.children[0].children[0].type == "VARIABLE_NAME"
        executeFunction(env, tree.children[0].children[0], tree.children[1])

def assignVariable(vname: Token, tree: Tree, env: LaCompilerEnviroment):

    if isinstance(tree, Token):
        if tree.type == "DECIMAL_NUMBER":
            env.alloc_var(vname)
            env.compiler.statements.append(LVMCompilableStatement(
                LVMOpcode.MOV,
                [(LVMArgumentTypes.SPECIAL_IMMEDIATE, int(tree)), env.vars_where[vname]],
            ))
            return

        elif tree.type == "DOUBLE_QUOTE_DATA":
            labelName = env.compiler.add_data(tree.encode('utf-8'))
            print(env.compiler.extra_data_at_the_end)
            var_location = (LVMArgumentTypes.COMPILER_LABEL, labelName)
            env.vars_where[vname] = var_location
            return

        raise errors.UnimplementedTokenError(tree.type)

    if isinstance(tree, Tree):
        if len(tree.children) == 1:
            assignVariable(vname, tree.children[0], env)

def compile_codeblock(tree: Tree, env: LaCompilerEnviroment):
    assert tree.data == "codeblock"
    for i in tree.children:
        assert i.data == "executable"
        assert isinstance(i, Tree)
        if i.meta.empty:
            continue
        elif len(i.children) == 2 and i.children[1].data == "evaluatable":
            assignVariable(i.children[0], i.children[1], env)

        elif len(i.children) == 1 and i.children[0].data == "evaluatable":
            evaluate(i.children[0], env)

        else:
            raise errors.InvalidExecutableStatement(text[i.meta.start_pos:i.meta.end_pos])


def main():


    grammar = open("la/grammar.lark", "r").read()

    code = tabs_to_codeblocks(open("file.la").read())

    text = "{" + code + "}"

    print(text)

    parser = Lark(grammar, 
        ambiguity="explicit",
        propagate_positions = True,
        )  # Scannerless Earley is the default

    tree = parser.parse(text)

    env = LaCompilerEnviroment()
    compile_codeblock(tree.children[0], env)


    with open("la_generated_bytecode.lac", "wb") as f:
        
        ElfFile(sections = [
            ElfSection(name=".all", content=env.compiler.compile_code(), virtual_address=0)
        ]).write_to(f)
        

if __name__ == '__main__':
    main()