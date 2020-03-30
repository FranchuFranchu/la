
class LaError(Exception):
    pass

class CompileTimeError(LaError):
    pass

class UnimplementedTokenError(CompileTimeError):
    pass