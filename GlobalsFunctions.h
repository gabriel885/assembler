#ifndef GLOBAL_FUNCTIONS_H
#define GLOBAL_FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "dataList.h"
#include "symbolsList.h"
#include "globalVariables.h"
#include "errors.h"


/*Description: is a valid register*/
bool isRegister(char*);

/*Description: is a valid opcode*/
bool isOpcode(char*);

/*Description: has open paranthesis in line */
bool hasParanthesis(char*);


/*Description: is a valid label */
bool checkLabel(char *, char*);


/*
Description: gets an operand and returns TRUE if it's a number operand. otherwise, returns FALSE.
*/
bool isNumOperand(char*);

/*
Description: gets a token and returns TRUE if it's a valid label. otherwise, returns FALSE.
*/
bool isValidLabel(char*);

/*
Description: gets an operand and returns TRUE if it's a jumping with parameters valid addressing, otherwise returns FALSE 
valid input ex: L1(#-1,r6)
Note: valid operands are jmp,bne,jsr 
*/
bool jumpingWithParameters(char*);

/* check if string has space inside it */
bool stringHasSpace(char *);

/* checks if valid addressing method for source/destination operands */
bool validAddressingOperand(char*, AddressingMode, AddressingMode);


/*
Description: gets an operand and returns it's addressing mode. if the operand is invalid, returns -1.
*/
AddressingMode getOperandAddressing(char*);

/*
Description: Gets a line to check if it has a label definition,
if true - returns TRUE and insert the label to the destination string, otherwise, returns FALSE.
*/
bool isLabel(char*, char*);

/*
Description: gets an line and returns TRUE if it's an entry line declaration. otherwise, returns FALSE.
*/
bool isEntry(char*);

/*
Description: gets an line and returns TRUE if it's an extern line declaration. otherwise, returns FALSE.
*/
bool isExtern(char*);

/*
Description: check if label is a data command 
*/
bool isLabelData(symbolPtr*, char*);

/*
Description: check if label is an extern 
*/
bool isLabelExtern(symbolPtr*, char*);

/*
Description: check if label is an entry
*/
bool isLabelEntry(symbolPtr*, char*);

/*
Description: gets a valid label and checks if this label already defined
*/
bool isLabelDefined(symbolPtr*, char*);

/*
Description: is an empty sentence
*/
bool isEmptySentence(char*);

/*
Description: is a comment sentence - begins with ';'
*/
bool isComment(char*);

/*
Description: gets a token and returns TRUE if its a keyword. otherwise, returns FALSE.
*/
bool isKeyword(char*);

/*
Description: gets a file path and returns TRUE if the file exists and can be opened for reading. otherwise, returns FALSE.
*/
bool isFileExists(char*);

/*
Description: gets the head and tail of the data list, the current data counter, and a number, and adds it to the data list.
*/
bool addNumberToDataList(dataPtr*, dataPtr*, int, int);

/*
Description: gets a line of data and a destination string and copies the symbol (.string or .data)
inside the data line to the destination string.
returns TRUE if .string or .data symbol is exist (only one of them), if not, returns FALSE.
*/
bool getSymbol(char*, char*, bool);

/*
Description: gets a parsed command (i.e "mov r1, r3") without any spaces or unnecessary characters.
returns the number that IC should be incremented with.
returns 0 when failed.
*/
int getCommandSize(char*);

/*
Description: convert a binary code to decimal number
and returns the number
*/
int binaryToDecimal(int *);

/*
Description: trim whitespace from a string
*/
char *trimString(char*);

/*
Description: trim whitespace on right end of a string
*/
char *trimRightString(char*);

/*
trim whitespaces on left end of a string
*/
char *trimLeftString(char*);


/*
Description: checks if a line is a data command (declaration of .string, .data ).
returns TRUE if the line is a data command, otherwise, returns FALSE.
*/
bool isDataCommand(char*);
/*
Description: gets a number operand and returns it as an integer.
input: #-1
outut: -1
*/
int getNumber(char*);

/*
Description: is a valid string
*/
bool isString(char *);

/*
Description: convert decimal code to binary code
*/
void decimalToBinary(int, int*, int);

/*
input : binary word array, result array and array size 
output: wierd 2 base array 
*/
void binaryToWierd(int*, char*, int);

/*
Description: convert 10 word length represents in binary code to wierd 2 base
*/
void decimalToWierd(int, char*);

/*
Description: is a sentence start with data command
*/
bool StartWithDataCommand(char *);

/*
Description: gets a FILE* handle and a destination string, and reads the next line from the file to the string.
*/
bool readLine(FILE*, char*);

/*
Description: is a sentence start with command
*/
bool StartWithCommand(char *);

/*
Description: reset the variables used in the first pass
*/
void resetLine(char *, char* , char *);

/*
Description: is a valid number
*/
bool isNumber(char *);


#endif
