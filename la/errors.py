
class LaError(Exception):
    pass

class CompileTimeError(LaError):
    pass

class UnimplementedTokenError(CompileTimeError):
    pass

class InvalidExecutableStatement(CompileTimeError):
    pass

class InvalidVariableName(CompileTimeError):
    pass