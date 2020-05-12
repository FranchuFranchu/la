from typing import List, Tuple
from .LVMCompiler import LaCompilerBatchMacro, LVMCompilableStatement, LVMOpcode, LVMArgumentTypes

class LaVariable:
    value: None
    def __la_eq__(self, other):
        return other.value == self.value
        
class LaArgument:
    mandatory: bool
    name: str
    consume: int
    def __init__(self, mandatory = True, name = "", consume = 1):
        self.mandatory = mandatory
        self.name = name
        self.consume = consume

class LaFunction(LaVariable):
    arguments: List[LaArgument]
    def __init__(self, func, arguments=[]):
        self.func = func
        self.arguments = arguments
    def __la_call__(self, args: List[Tuple[str, LaVariable]]):
        free_arguments = self.arguments.copy()
        final_args = {}
        for key, value in args:
            if key == "":
                final_args[free_arguments[0].name] = value
                del free_arguments[0]
            else:

                for arg in free_arguments:
                    if arg.name == key:
                        final_args[key] = value
                        found = key
                        break
        if len(free_arguments) != 0:
            raise FuckingError
        self.func(final_args)

def LaFunctionDecorator(*args, **kwargs):
    def _internal_decorator(func):
        return LaFunction(func, *args, **kwargs)
    return _internal_decorator









class LaInteger(LaVariable):
    value: int
    def __init__(self, value: int):
        self.value = value
    def __la_add__(self, other):
        if isinstance(other, LaInteger):
            return LaInteger(self.value + other.value)
    def __la_sub__(self, other):
        if isinstance(other, LaInteger):
            return LaInteger(self.value - other.value)
    def __la_mul__(self, other):
        if isinstance(other, LaInteger):
            return LaInteger(self.value * other.value)
    def __la_div__(self, other):
        if isinstance(other, LaInteger):
            return LaInteger(self.value / other.value)
    def __la_pow__(self, other):
        if isinstance(other, LaInteger):
            return LaInteger(self.value ** other.value)
    def __la_str__(self):
        return str(self.value)
    def __str__(self):
        return str(self.value)

class LaBoolean(LaVariable):
    value: bool
    def __init__(self, value: bool):
        self.value = value

class LaString(LaVariable):
    value: str
    def __init__(self, value: str):
        self.value = value
    def __la_str__(self):
        return str(self.value)

class LaBuiltins:
    @LaFunctionDecorator([LaArgument(name="value")])
    def print(args):
        print(args["value"].__la_str__())

    @LaFunctionDecorator()
    def input(args):
        pass

la_builtin_macros = {
    "print": LaCompilerBatchMacro([
        LVMCompilableStatement(LVMOpcode.DEBUG_DUMP_OPERATOR_0, [(LVMArgumentTypes.REGISTER_A, None)])
    ], [(LVMArgumentTypes.REGISTER_A, None)])
}
