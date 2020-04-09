# Converts python-style code into JS-style code
def tabs_to_codeblocks(d):
    list_index = 0
    code = list(d)
    at_newline = True
    current_indentation = 0
    this_line_indentation = 0
    while list_index < len(code):
        if at_newline:
            if code[list_index] in (" ", "\t"):
                this_line_indentation += 1
            else:
                at_newline = False
                difference = this_line_indentation - current_indentation
                if difference > 0:
                    for i in range(difference):
                        code.insert(list_index,"{")
                        list_index += 1
                elif difference < 0:
                    for i in range(-difference):
                        code.insert(list_index,"}")
                        list_index += 1
                    code.insert(list_index, ";")
                current_indentation = this_line_indentation

        if not at_newline:
            if code[list_index] == "\n":
                at_newline = True
                this_line_indentation = 0
                code.insert(list_index,";")
                list_index += 1
        list_index += 1

    # Close indentation again
    for i in range(current_indentation):
        code.insert(list_index,"}")
        list_index += 1 

    return "".join(code)