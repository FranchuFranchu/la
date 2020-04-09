from abc import ABC
import pprint
import logging

from lark import Lark, Tree, Token

from .parse_tabs import tabs_to_codeblocks
from .la_builtins import LaBuiltins, LaInteger, LaBoolean, LaString, LaFunction, LaArgument
import la.errors as errors


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

operators = {
    "+": lambda a, b: a.__la_add__(b),
    "*": lambda a, b: a.__la_mul__(b),
    "-": lambda a, b: a.__la_sub__(b),
    "/": lambda a, b: a.__la_div__(b),
    "**": lambda a, b: a.__la_pow__(b),
    " cat ": lambda a, b: a.__la_cat__(b),
    " rep ": lambda a, b: a.__la_rep__(b),
    ".": lambda a, b: a.__la_getattr__(b),
    ":": lambda a, b: a.__la_getiattr__(b),
    "==": lambda a, b: a.__la_eq__(b),
}


def checkCorrectPrecedence(tree, currentPrecedence = None, currentPos = -2**31):


    if isinstance(tree, Token):
        return True

    operatorPos = currentPos


    if len(tree.children) >= 3 and tree.data == "operation":
        operatorPrecedence = precedence[tree.children[1].children[0]]
        operatorPos = tree.children[1].children[0].pos_in_stream
        if currentPrecedence != None:
            if currentPrecedence < operatorPrecedence:
                return False
            if currentPrecedence == operatorPrecedence:
                return operatorPos <= currentPos
        return checkCorrectPrecedence(tree.children[0], operatorPrecedence, operatorPos) and checkCorrectPrecedence(tree.children[2], operatorPrecedence)
    elif tree.data == "evaluatable":
        return checkCorrectPrecedence(tree.children[0], currentPrecedence, operatorPos)
    elif tree.data == "_ambig":
        # Must make sure at least one is possible
        k = resolveAmbig(tree)

        return False if k == False else True

    else:
        return True


def resolveAmbig(tree):
    valid_precedence = []
    for i in tree.children:
        if i.data == "operation":
            if checkCorrectPrecedence(i):
                valid_precedence.append(i)
        else:
            valid_precedence.append(i)
    
    if valid_precedence:
        return valid_precedence[0]
    else:
        return False    

def executeFunction(function: LaFunction, args):
    args = [('', i) for i in args]
    return function.__la_call__(args)

def evaluate(tree: Tree, env: dict):
    if isinstance(tree, Token):
        if tree.type == "VARIABLE_NAME":
            return env[str(tree)] 
        elif tree.type == "SINGLE_QUOTE_DATA":
            return LaString(tree)
        elif tree.type == "DOUBLE_QUOTE_DATA":
            return LaString(tree)
        elif tree.type == "DECIMAL_NUMBER":
            return LaInteger(int(tree))
        raise errors.UnimplementedTokenError(tree.type)

    if len(tree.children) == 1:
        return evaluate(tree.children[0], env)
    elif tree.data == "function_call":
        assert len(tree.children) == 2
        args = [evaluate(i,env) for i in tree.children[1].children]
        return executeFunction(evaluate(tree.children[0], env), args)
    elif tree.data == "_ambig":
        # Must resolve ambiguity
        true = resolveAmbig(tree)
        return evaluate(true, env)
    elif tree.data == "operation":
        return operators[tree.children[1].children[0]](
            evaluate(tree.children[0], env),
            evaluate(tree.children[2], env),
            )

def assignVariable(output, value, env):
    if isinstance(output, Token):
        env[str(output)] = evaluate(value, env)

def run_prefixed_codeblock(tree: Tree, env: dict, func):
    codeblock_func = func
    def _def_func(env):
        def _internal_func(args: dict):
            env2 = {**env, **args}
            codeblock_func(env2)

        args = []
        for i in tree.children[2].children:
            args.append(LaArgument(name=i))

        env[tree.children[1]] = LaFunction(_internal_func, args)
    def _if_func(env):
        func(env) if evaluate(tree.children[1], env) else None
    FUNCS = {
        "if": _if_func,
        "function": _def_func,
    }
    FUNCS[tree.children[0]](env)

def run_codeblock(tree: Tree, env: dict):
    assert tree.data == "codeblock"
    for i in tree.children:
        if i.data == "_ambig":
            i = resolveAmbig(i)
        assert i.data == "executable"
        assert isinstance(i, Tree)
        if i.meta.empty:
            pass
        elif len(i.children) == 1 and i.children[0].data == "evaluatable":
            evaluate(i.children[0], env)
        elif len(i.children) == 2 and i.children[1].data == "evaluatable":
            assignVariable(i.children[0], i.children[1], env)
        elif len(i.children) == 1 and i.children[0].data == "codeblock":
           run_codeblock(i.children[0], env)
        elif len(i.children) == 2 and i.children[1].data == "codeblock" and i.children[0].data == "codeblock_prefix":
            ttree = i.children[0].children[0]
            code_block = i.children[1]
            func = lambda env: run_codeblock(code_block, env)
            run_prefixed_codeblock(ttree, env, func)

        else:
            raise errors.InvalidExecutableStatement(text[i.meta.start_pos:i.meta.end_pos])

run_codeblock(tree.children[0], dict(LaBuiltins.__dict__))
