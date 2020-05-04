from abc import ABC
import pprint
import logging
from typing import List, Tuple, Dict, Callable

from lark import Lark, Tree, Token

from la.parse_tabs import tabs_to_codeblocks
from la.la_builtins import LaBuiltins, LaInteger, LaBoolean, LaString, LaFunction, LaArgument
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
        self.vars_where[varname] = (LVMArgumentTypes.REGISTER_A, None)
        return self.vars_where[varname]

        

grammar = open("la/grammar.lark", "r").read()

code = tabs_to_codeblocks(open("file.la").read())

text = "{" + code + "}"

print(text)

parser = Lark(grammar, 
    ambiguity="explicit",
    propagate_positions = True,
    )  # Scannerless Earley is the default

tree = parser.parse(text)

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

class LaCompilerMacroFixedBytecode:
    func: Callable
    requires_args: List[LVMArgumentTypes]
    bytecode: LVMCompiler
    def compile(args):
        return LVMCompiler.compile()




def executeFunction(env, function_name, args):
    if function_name == "print":
        env.compiler.statements.append(LVMCompilableStatement(LVMOpcode.DEBUG_DUMP_OPERATOR_0, [(LVMArgumentTypes.REGISTER_A, None)]))

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

env = LaCompilerEnviroment()
compile_codeblock(tree.children[0], env)


with open("la_generated_bytecode.lac", "wb") as f:
    env.compiler.save_to(f)