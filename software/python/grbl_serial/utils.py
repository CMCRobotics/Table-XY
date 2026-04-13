"""
General definitions and Utilities
"""
string_eq = "="*80

def error_function(error_text: str):
    """
    Prints the Error message and exits
    """
    print("\n\n")
    print(string_eq)
    print(" Exit with Error\n")
    print(error_text)
    print(string_eq)
    print("\n")

    exit()

def clear_terminal():
    # Clear the Terminal
    print(chr(27) + "[2J")
