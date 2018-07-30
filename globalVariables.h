
#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#define INPUT_EXTENSION ".as"

#define MAX_FILENAME_LENGTH 30
#define MAX_LABEL_LENGTH 31
#define MAX_FILE_NAME_LENGTH 255


#define IC_INITIALIZED 100
#define DC_INITIALIZED 0

#define EXTERN_DECLARATION ".extern"
#define ENTRY_DECLARATION ".entry"
#define EXTERN_LENGTH 7
#define ENTRY_LENGTH 6


#define ENTRY_FILE_END ".ent"
#define EXTERN_FILE_END ".ext"
#define OBJECT_FILE_END ".ob"

#define NUM_OF_KEYWORDS	20
#define NUM_OF_REGISTERS 8
#define NUM_OF_OPCODES 16

#define MAX_OPCODE_LENGTH 4 

#define REGISTER_SIZE 14
#define ERACODE_SIZE 2
#define MEMORY_SIZE 256
#define ERA_TYPES_SIZE	3
#define WORD_SIZE 14
#define OPCODE_SIZE 4
#define REGISTER_ADDRESS_SIZE	4
#define OPERAND_SIZE 2
#define ENCODING_SIZE 2
#define PARAMETER_SIZE 2

#define BASE_LENGTH 2 /* / . */

#define MAX_LINE_LENGTH 80

#define FIRST_GROUP 1
#define SECOND_GROUP 2
#define THIRD_GROUP 3

#define FIRST_GROUP_SIZE 5
#define SECOND_GROUP_SIZE 9
#define THIRD_GROUP_SIZE 2

/*Minimum machine instructions*/
#define MIN_ROWS 1

/*maximum machine instructions*/
#define MAX_ROWS 5

#define MAX_2_WEIRD_LENGTH 15

#define MULT 16

#define MAX14BITS 16384

#define MAX10BITS 1024

#define uint unsigned int

#define EIGHT_BITS 8

#define TEN_BITS 10

#define EXTERN "extern"
#define ENTRY "entry"

#define MIN_NUMBER -128
#define MAX_NUMBER 127

#define MIN_DATA_NUMBER -512 
#define MAX_DATA_NUMBER 511

#define NUM_OF_DATA_TYPES 2

#define TWELVE_BITS 12
#define STRING_COMMAND_LENGTH 7
#define DATA_COMMAND_LENGTH 5

#define NUM_OF_ADDRESSING_WITH_PARAMETERS 3

#define SPACE_INSIDE_PARAMETER_ERROR "Parameter should not have space inside them"

typedef enum { FALSE, TRUE } bool;


typedef enum { IMMEDIATE, DIRECT_MEMORY, JUMPING_WITH_PARAMETERS, DIRECT_REGISTER } AddressingMode;


typedef struct opcode_structure{
	int DecimalCode;
	char* opcodeName;
	int binaryCode[WORD_SIZE];
	int group;
} opcodeStructure;


typedef struct register_structure{
	const char* registerName;
	int address[REGISTER_ADDRESS_SIZE];
} RegisterStructure;


typedef struct word{
	int opcode[OPCODE_SIZE];
	int sourceOperand[OPERAND_SIZE];
	int ERAcode[ERACODE_SIZE];
} word;


typedef enum {
	Absolute,
	External,
	Relocatable
} encodingType;

typedef struct ERAStructure{
	uint num;
	encodingType ERAName;
	char* binaryVal;
}ERAStructure;



extern ERAStructure ERATypes[ERA_TYPES_SIZE];

extern char* WIERD_2_BASE[BASE_LENGTH];

extern char* Keywords[NUM_OF_KEYWORDS];

extern char* Registers[NUM_OF_REGISTERS];

extern char* addressing_with_parameters_operands[NUM_OF_ADDRESSING_WITH_PARAMETERS];

extern opcodeStructure opcodes[NUM_OF_OPCODES];

extern RegisterStructure registersBinary[NUM_OF_REGISTERS];

extern int lineCounter;

extern char *OpcodesFirstGroup[FIRST_GROUP_SIZE];

extern char *OpcodesSecondGroup[SECOND_GROUP_SIZE];

extern char *OpcodesThirdGroup[THIRD_GROUP_SIZE];

extern char *DataCommand[NUM_OF_DATA_TYPES];

extern int DataCommandLength[NUM_OF_DATA_TYPES];

extern char *AssemblyfileName;

#endif
