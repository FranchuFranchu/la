import la.LVMCompiler as compiler

a = compiler.LVMCompiler.from_string("""
PRINT:
DEBUG_DUMP_OPERATOR_0 42
JUMP PRINT
""")

print(a.statements[1].args)



with open("la_generated_bytecode.lac", "wb") as f:
    a.save_to(f)