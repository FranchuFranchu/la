from abc import ABC
import pprint
import logging

from lark import Lark, Tree, Token

from .parse_tabs import tabs_to_codeblocks
from .la_builtins import LaBuiltins, LaInteger, LaBoolean
import la.errors


grammar = open("la/grammar.lark", "r").read()

code = tabs_to_codeblocks(open("file.la").read())

text = "{" + code + "}"

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
    ".": 0,
    ":": -1,

}

operators = {
    "+": lambda a, b: a + b,
    "*": lambda a, b: a * b,
    "-": lambda a, b: a - b,
    "/": lambda a, b: a / b,
}

class LaVariable:
    pass

class LaFunction(LaVariable):
    def __la_call__(self, args):
        pass

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
    function.__la_call__(args)

def evaluate(tree: Tree, env: dict):
    if isinstance(tree, Token):
        if tree.type == "VARIABLE_NAME":
            return env[str(tree)] 
        elif tree.type == "SINGLE_QUOTE_DATA":
            return str(tree)
        elif tree.type == "DOUBLE_QUOTE_DATA":
            return str(tree)
        elif tree.type == "DECIMAL_NUMBER":
            return int(tree)
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



def run_codeblock(tree: Tree, env: dict):
    assert tree.data == "codeblock"
    for i in tree.children:
        if i.data == "_ambig":
            i = resolveAmbig(i)
        assert i.data == "executable"
        if len(i.children) == 1 and i.children[0].data == "evaluatable":
            evaluate(i.children[0], env)

run_codeblock(tree.children[0], LaBuiltins.__dict__)
