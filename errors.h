/*
Authors:
Guy Komari
Gil Mansharov
*/

#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "globalVariables.h"

/* File errors */
#define FILE_NAME_MEM_ALLOCATION_FAILURE "Could not allocate memory for the file name you provided"
#define OPEN_FILE_ERROR "could not open the file "
#define ERROR_CREATE_FILE	"Could not create file"
#define MISSING_INPUT_FILE "missing input files "
#define FILENAME_TOO_LONG "filename is too long"

#define INVALID_LABEL_LENGTH "label's length is invalid"

#define INVALID_OPCODE_ERROR "The opcode is invalid"

#define ERRORS_IN_FIRST_PASS "errors found on the first pass, output files will not be created for the input file "

#define ERRORS_IN_SECOND_PASS "errrors found on the second pass, for the input file "

#define MISSING_SPACE_AFTER_ENTRY "A space is missing after declaration of an entry"

#define MISSING_SPACE_AFTER_EXTERN "A space is missing after declaration of an extern"



#define LABEL_BEGINS_WITH_NUMBER  "Label should not begin with a number"

#define MORE_THEN_ONE_LABEL "only one label can be declared"

#define NUMBER_IS_OUT_OF_RANG "the number is out of range, the language range is -256 to 256"

#define LABEL_NOT_START_WITH_ALPHA "The label should start with an alphabetical letter"

#define LABEL_CONTAINS_NON_ALPHA_OR_DIGIT "The label should contain only alphabetical letters and digits"

#define MISSSING_COLON_ERROR "A colon is missing after a definition of a label name"

#define OPEN_INPUT_FILE_ERROR "the input file does not exist"

#define LABEL_DECLARED_BUT_NOT_DEFINED	"An undefined label has been declared"

#define ERROR_CONVERT_LABEL_ADDRESS_TO_WEIRD	"Cannot convert label address to weird base"

#define ALLOCATE_MEMORY_ERROR "cannot allocate memory "

#define STRUCT_DEFINITION_ERROR "Unrecognized Type in struct definition"

#define INVALID_DATA_TYPE "Invalid data type "

#define INVALID_NUMBER_IN_DATA_LABEL "Invalid number in .data label"

#define MISSING_SPACE_AFTER_COLON	"Missing space after colon"

#define LABEL_IS_ALREADY_DEFINED	"Label is already defined"

#define JUMPING_WITH_PARAMETERS_ERROR "An error occurred in jumping with parameters command encoding"
 
#define JUMPING_WITH_PARAMETERS_SHOULD_START_WITH_LABEL "Jumping with parameters should start with a label"

#define VARIABLE_NAME_EQUALS_KEYWORD_NAME	"A variable name has the same name as a keyword"

#define OPERAND_NAME_EQUALS_KEYWORD_NAME	"An operand name cannot be the same as a keyword name"

#define TOO_FEW_OPERANDS_IN_COMMAND	"Too few operands in the command"

#define TOO_MUCH_OPERANDS_IN_COMMAND "Too much operands in command"

#define SOURCE_OPERAND_ERROR "Source operand error" 

#define DESTINATION_OPERAND_ERROR "Destination operand error"

#define INVALID_SRC_OPERAND	"Source operand is invalid"

#define INVALID_DEST_OPERAND	"Destination operand is invalid"

#define LEA_COMMAND_ADDRESSING_MODE_ERROR	"\"lea\" opcode cannot have an immediate or immediate register addressing mode"

#define CMP_COMMAND_ADDRESSING_MODE_ERROR	"Only \"cmp\" opcode can use immediate addressing mode in the destination operand"

#define PRN_COMMAND_ADDRESSING_MODE_ERROR	"Only \"prn\" opcode can use immediate addressing mode in the destination operand"


#define TOO_MUCH_OPERANDS "Too many operands in command" 

#define SYMBOLS_LIST_MEM_ALLOCATION_FAILURE	"Memory allocation failed in symbols list addition"

#define MISSING_SPACE_AFTER_STRING_DECLARATION	"Missing space after .string declaration"

#define MISSING_SPACE_AFTER_DATA_DECLARATION	"Missing space after .data declaration"

#define MISSING_SPACE_AFTER_STRUCT_DECLARATION	"Missing space after .struct declaration"

#define ERROR_UNKNOWN_DECLARATION "Error unknown declaration - "

#define NO_LABEL_DECLARED_ERROR "no label declared after entry or extern declaration"

#define LABEL_LENGTH_ERROR "label length is not valid"

#define RUNTIME_ERROR_DECLARING_REGISTER_OR_LABEL "unknown label or register where declared"

#define OVERFLOW_DATA_POSITIVE_NUMBER "positive value too large to fit in 10 bits"

#define OVERFLOW_DATA_NEGATIVE_NUMBER "negative value too large to fit in 10 bits"

#define OVERFLOW_COMMAND_POSITIVE_NUMBER "positive immediate too large to fit in 8 bits"

#define OVERFLOW_COMMAND_NEGATIVE_NUMBER "negative immediate too large to fit in 8 bits"

#define SORCE_OPERAND_NOT_NUMBER_IN_STRUCT "Source opernd should be a number"

#define DESTINATION_OPERAND_NOT_STRING_IN_STRUCT "Destination opernd should be a String"

#define MISSING_ARGUMENTS_IN_DIRECTIVE "Missing arguments in directive"

#define TOO_MUCH_WORDS_IN_STRUCT	"Too much words in the struct"

#define NOT_NUMBER_IN_DATA "Invalid number in .data declaration" 

#define LABEL_NAME_EQUALS_KEYWORD_NAME "A label has the same name as a keyword"

#define LABEL_NAME_EQUALS_REGISTER_NAME "A label has the same name as a register"

#define NOT_STRING_IN_STRING_DECLARATION "Invalid String in .string declaration" 

#define INVALID_CHARACTERS "Invalid characters in .string declaration"

#define INVALID_LABEL_OR_OPCODE_DECLARATION "Invalid label or opcode declaration" 

#define INVALID_LABEL "Invalid label entered"
/*
Description: Prints an error that is detected during the first pass (compilation error)
error - error description
*/
void printError(char*);

/*
Description: Prints an error while opening or creating a file
error - error description
fileName - file name
*/
void printFileError(char*, char*);

void printErrorWithArgument(char *, char*);

/*
Description: Prints a warning that is detected during the second pass
error - error description
ref - a reference that caused the error
*/
void printWarning(char*, char *);

/*
Description: Prints an error that is detected during the second pass (runtime error)
error - error description
ref - a reference that caused the error
*/
void printRuntimeError(char* , char* );

/*
Description: Prints an error that is detected during the first pass (compilation error)
error - error description
ref - a reference that caused the error
*/
void printExtendedError(char* , char* );

#endif
