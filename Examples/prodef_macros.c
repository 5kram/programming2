// Pro-Defined MacroCommands
// ANSI C is successive standard for the C programming language
// Published by the American National Standards Institute (ANSI)
#include  <stdio.h>

int main (int argc, char *argv[]) {

    // Source Code Filename as a String Literal
    printf("File: %s\n", __FILE__);
    // Date of Compilation as a String Literal
    // Format: "MMM DD YYYY"
    printf("Date: %s\n", __DATE__);
    // Time of Compilation as a String Literal
    // Format: "HH:MM:SS"
    printf("Time: %s\n", __TIME__);
    // Function Name as a String Literal
    printf("Function: %s\n", __func__);
    // Line Number in the source file as a Decimal Constant
    printf("Line: %d\n", __LINE__);
    // Equal to the Decimal Value 1
    // If the compiler compiles with ANSI Standard
    printf("ANSI: %d\n", __STDC__);

    return 0;
}