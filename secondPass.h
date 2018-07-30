#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "globalVariables.h"
#include "GlobalsFunctions.h"

/* second pass */
bool secondPass(char*);

/* creates object,extern,entry files */
void createFile(char*, FILE*, char*, char*);

/* prints entry declaration to entry file */
void printToEntryFile(char*);

/* prints extern declarations to extern file */
void printToExternFile(char *);

/* return 1 if opcode from first group, 2 if from second group, 3 if from third opcode group */
int getOpcodeGroup(char*);

/* get addressing type 0-immediate,1 - direct memory, 2- jumping with parameters, 3 - direct register */
int getAddressingType(int*);

/* encode command to weird 2 base */
bool encodingCommand(char*);

/* encode jumping with parameters command */
bool makeBinaryCodeJumpingWithParameters(int,char*,char*,char*,char*);

bool makeBinaryCode(int,char*,char*,char*);

void makeSingleOperandBinary(char *, int *);

/* prints  IC and DC to the first line of object file */
void printICandDC();

void printBinaryWord(int *);

void setOpcodeBinaryCode(char *, int *);

void copyBinaryOpcode(int, int *);

bool makeOperandsWeird(int, int *, int *, char *, char *);

/* print label to weird 2 base and write to object file */
bool printLabelWeird(char *operand);

void printRegisterWeird(char *, char *, bool, bool);

void copyBinaryRegister(int *, int, char *);

void printNumberWeird(char*, char *);

bool printDestOperand(char *, char *, int);

bool printWeirdDataOperand(char *);

void printDataWeird();

bool LabelDeclaredButNotDefined(char *);

void resetFilesName(char *, char *, char *);

void resetSingleLine(char *);

#endif
