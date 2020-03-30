class LaVariable:
    pass

class LaFunction(LaVariable):
    pass

class LaInteger(LaVariable):
    value: int
    def __init__(self, value: int):
        self.value = value

class LaBoolean(LaVariable):
    value: bool
    def __init__(self, value: bool):
        self.value = value

class LaBuiltins:
    class print(LaFunction):
        def __la_call__(args):
            print(args)
