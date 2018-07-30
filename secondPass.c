#include "secondPass.h"

/* Variables declaration */
long ICounter;/*IC - for the second pass*/

static FILE *sourceFileHandle; /* input file handle */
static FILE *objFile, *entFile, *extFile; /* output files handles */

static char objFileName[MAX_FILE_NAME_LENGTH + 1] = { 0 }; /* object file name */
static char entFileName[MAX_FILE_NAME_LENGTH + 1] = { 0 }; /* entry file name */
static char extFileName[MAX_FILE_NAME_LENGTH + 1] = { 0 }; /* extern file name */

static char *assemblyFileName;/* assemblyF file name */
static bool extFlag; /* extern flag */
static bool entFlag; /* entry flag */

extern long IC, DC; /* IC and DC after the first pass */

extern symbolPtr symbolListHead, symbolListTail; /* extern from firstPass.c */
extern dataPtr dataListHead, dataListTail; /* extern from firstPass.c */

/*
Description:
The main function of the second pass.
The second pass purpose is to:
1. Parses from the input file, and converts any line and operand to "weird 32 base".
2. Generates the output files - object file, entry file, and extern file.
3. Notify of runtime errors and warnings.
*/
bool secondPass(char* fileName){
	
	bool is_label, is_data_command, endFile, is_entry, noErrorFlag;

	char line[MAX_LINE_LENGTH + 1] = { 0 }; /* line in the assembler file */
	char labelName[MAX_LINE_LENGTH + 1] = { 0 }; 

	/*initialize variables*/
	ICounter = IC_INITIALIZED;
	extFlag = FALSE; /* TRUE if extern file exists */
	entFlag = FALSE; /* TRUE if entry file exists */
	lineCounter = 0;

	resetFilesName(objFileName, entFileName, extFileName);
	is_label = is_data_command = endFile = is_entry = FALSE;
	noErrorFlag = TRUE;
	assemblyFileName = fileName;


	/* check input file */
	sourceFileHandle = fopen(fileName, "r");
	if (sourceFileHandle == NULL){
		printFileError(OPEN_FILE_ERROR, fileName);
		return FALSE;
	} 

	/* create the object output file */
	createFile(fileName, objFile, objFileName, OBJECT_FILE_END);

	/*checks if created successfully*/
	if (!isFileExists(objFileName)){
		printFileError(OPEN_FILE_ERROR, objFileName);
		return FALSE;
	}

	/* print IC and DC in first line of object file */
	printICandDC();
	
	/* parse the input assembler file */
	while ((endFile = readLine(sourceFileHandle, line))){
		
		/*case - empty line OR comment OR extern */
		if (isEmptySentence(line) || isComment(line) || isExtern(line)){
			continue;
		}

		is_entry = isEntry(line);/*contains an entry definition*/
		is_label = isLabel(line, labelName); /*contains a label definition*/

		
		/* check for data declaration */
		if (!is_entry && is_label){
			is_data_command = isDataCommand(line); /*contains a data declaration*/
		}

		/* print entry to entry output file */
		if (is_entry){
			printToEntryFile(line); /* print the label name and its address to the entry file */
			continue;
		}
		
		/* data command print last */
		if (is_label && is_data_command){
			continue;
		}

		/*label with command*/
		else{
			if (!encodingCommand(line)) /*encodding the line and each operand*/
				noErrorFlag = FALSE;
		}

		resetSingleLine(line);	/*reset the line*/

	}

	printDataWeird(); /*print the data in weird base after finished to print all instructions*/

	freeSymbolsList(&symbolListHead);/* free memory of the symbols list */
	freeDataList(&dataListHead);/* free memory of the symbols list */

	return noErrorFlag;/* second pass succeeded */
}
/*
prints instruction counter and data counter to the first line of object file 
*/
void printICandDC(){
	
	objFile = fopen(objFileName, "wb");

	if (!objFile){
		printFileError(OPEN_FILE_ERROR, objFileName);
		return;
	}

	fprintf(objFile, "%ld\t%ld\n", (IC-IC_INITIALIZED-DC), DC); 
	fclose(objFile);
}


/*
Description:
Prints the data of the assembly file in weird 32 base, after the instructions were printted.
*/
void printDataWeird(){
	int i = 0;
	dataPtr temp = dataListHead; /* temp datalist head pointer */
	
	char weirdWord[MAX_2_WEIRD_LENGTH] = { 0 };
	
	objFile = fopen(objFileName, "ab");
	if (!objFile){
		printFileError(OPEN_FILE_ERROR, objFileName);
		return;
	}
	/* iterate datalist and print */
	while (temp){
		/* initialize char array */
		for (i = 0; i<MAX_2_WEIRD_LENGTH; i++){
			weirdWord[i] = 0;
		}

		decimalToWierd((temp->asciiCode),weirdWord);
		fprintf(objFile, "%ld\t%s\n", ICounter, weirdWord);
		temp = temp->next;
		ICounter++;
	}
	fclose(objFile);
}
/*
Description:
prints entry label declared 
line - a single line from the assembly file
*/
void printToEntryFile(char* line){

	char* labelBuffer;
	char *temp;
	symbolPtr searchLabel = symbolListHead;/* temp symbol list head pointer */

	temp = trimString(line);
	temp = strstr(temp, ENTRY_DECLARATION);
	temp = (char *)(temp + ENTRY_LENGTH); /* move the pointer after the ".entry" command*/
		
	/*ignore whitespaces*/
	while (isspace(*temp))
		temp++;

	labelBuffer = (char*)malloc(strlen(temp) + 2);

	if (!labelBuffer){
		printError(ALLOCATE_MEMORY_ERROR);
		return;
	}
	/* copy label to label buffer */
	strncpy(labelBuffer, temp, strlen(temp) + 1); 
	labelBuffer[strlen(temp) + 1] = '\0';
	
	/* check if label is declared but not defined */
	if (LabelDeclaredButNotDefined(temp) == TRUE){
		printWarning(LABEL_DECLARED_BUT_NOT_DEFINED, labelBuffer);
		free(labelBuffer);
		return;
	}

	else{

		/*entry file validation - create entry file if not created already */
		if (!entFlag){
			entFlag = TRUE;
			createFile(assemblyFileName, entFile, entFileName, ENTRY_FILE_END);/*create the entry output file*/
		}
	
		if (!isFileExists(entFileName)){
			printFileError(OPEN_FILE_ERROR, entFileName);
			free(labelBuffer);
			return;
		}

		entFile = fopen(entFileName, "ab");
		if (!entFile){
			printFileError(OPEN_FILE_ERROR, entFileName);
			free(labelBuffer);
			return;
		}

		while (searchLabel){
			/*find node with labelname */
			if (strcmp((searchLabel->name), temp) == 0){
				fprintf(entFile, "%s\t%u\n", temp, (searchLabel->address));
				break;
			}
			searchLabel = searchLabel->next;
		}
	}
	free(labelBuffer);
	fclose(entFile);
}
/*
Description:
Prints To extern File
operand - operand string
*/
void printToExternFile(char *operand){
	
	
	/* create external file if not created already */
	if (!extFlag){
		createFile(assemblyFileName, extFile, extFileName, EXTERN_FILE_END);/* create the object output file */
		extFlag = TRUE;
	}

	extFile = fopen(extFileName, "ab"); 
	if (!extFile){
		printFileError(OPEN_FILE_ERROR, extFileName);
	}

	fprintf(extFile, "%s\t%ld\n", operand, ICounter);
	fclose(extFile);
}
/*
Description:
Checks if an entry label was defined during the assembly file.
temp - label name
*/
bool LabelDeclaredButNotDefined(char *temp){
	symbolPtr searchLabel = symbolListHead;

	/* search label in symbol list */
	while (searchLabel){

		if (strcmp((searchLabel->name), temp) == 0){
			return FALSE;
		}
		searchLabel = searchLabel->next;
	}
	return TRUE;
}

/*
Description:
Generates an output file.
fileName - the input file name
dest - output file hendle
destName - output file name
end - output file extension
*/
void createFile(char* fileName, FILE* dest, char* destName, char* end){
	int filenameLength, fileExtensionLength, destExtensionLength;
	char *temp;	

	destExtensionLength = strlen(end);
	filenameLength = strlen(fileName);
	
	if (filenameLength >= MAX_FILE_NAME_LENGTH)
		return;

	temp = strrchr(fileName,'.'); /* find last accurance of dot */
	fileExtensionLength = strlen(temp);

	if (filenameLength - fileExtensionLength + destExtensionLength >= MAX_FILE_NAME_LENGTH)
		return;
	
	strncpy(destName, fileName, filenameLength - fileExtensionLength); /* copy filename without extension */
	strncpy(destName + filenameLength - fileExtensionLength , end, destExtensionLength); /* copy destination extention */

	destName = trimString(destName);
	
	dest = fopen(destName, "wb");
	if (dest == NULL){
		printFileError(ERROR_CREATE_FILE, destName);
		return;
	}
	fclose(dest);

}

/*
Description:
Encoding a command
objFileName - object file name
line - a single line from the assembly file
*/
bool encodingCommand(char* line){

	int i, opcodeGroup, length;
	char *temp, *lineBuffer;
	bool noerrors;
	char *sourceOperand; /* bits 2-3 */
	char *destOperand; /* bits 4-5 */	
	char *labelOperand;
	/*opcode word size*/
	char opcodeName[OPCODE_SIZE+1] = { 0 }; /* bits 6-9 */
	
	/*the label*/ 
	char label[MAX_LABEL_LENGTH] = { 0 };

	temp = sourceOperand = destOperand = NULL;
	noerrors = FALSE;

	if (!isFileExists(objFileName)){
		printFileError(OPEN_FILE_ERROR, objFileName);
		return FALSE;
	}

	temp = trimString(line);
	length = strlen(temp);

	/* allocate buffer for output */
	lineBuffer = (char*)malloc(length + 2);

	if (!lineBuffer){
		printError(ALLOCATE_MEMORY_ERROR);
		return FALSE;
	}

	strncpy(lineBuffer, temp, length + 1);
	lineBuffer[length + 1] = '\0';
	opcodeGroup = 0;
	
	/*case - commmand starts with label*/
	if (isLabel(temp, label)){
		temp = (char*)(temp + strlen(label) + 1);
	}

	/*ignore whitespaces*/
	while (isspace(*temp)){
		temp++;
	}
	strcat(temp, "\n");
	for (i = 0; (*temp) != '\0' && (!isspace(*temp)); i++){
		opcodeName[i] = temp[0];
		temp++;
	}

	/*to which group the opcode belongs*/
	opcodeGroup = getOpcodeGroup(opcodeName);

	if (opcodeGroup != THIRD_GROUP){
		while (isspace(*temp)){
			temp++;
		}
	}
	/* handle jumping with parameters addressing */
	if (hasParanthesis(temp)){
	
		/* handle operands */
		labelOperand = strtok(temp,"("); 	
		sourceOperand = strtok(NULL,",");
		sourceOperand = trimString(sourceOperand);
		destOperand = strtok(NULL,")");
		destOperand = trimString(destOperand);
		
		/* encode instruction */
		noerrors &= makeBinaryCodeJumpingWithParameters(opcodeGroup,opcodeName,labelOperand,sourceOperand,destOperand);
	
		/* encode parameters */

		if (isRegister(sourceOperand) && isRegister(destOperand)){ /* encode registers to single word */
			printRegisterWeird(sourceOperand, destOperand, TRUE, TRUE);
		}
		else{
			/* encode source parameter */
		    if (isRegister(sourceOperand)){
				printRegisterWeird(sourceOperand, destOperand, TRUE, FALSE);
			}			
			else if (isValidLabel(sourceOperand)){
				noerrors &= printLabelWeird(sourceOperand);
			}
			
			else if (isNumOperand(sourceOperand)){
				printNumberWeird(objFileName, sourceOperand);
			}
			else{
				noerrors = FALSE;
			}
			/* encode destination parameter */

			if (isRegister(destOperand)){
				printRegisterWeird(sourceOperand, destOperand, FALSE, TRUE);
			}

			else if (isValidLabel(destOperand)){
				noerrors &= printLabelWeird(destOperand);
			}
			
			else if (isNumOperand(destOperand)){
				printNumberWeird(objFileName, destOperand);
			}
			else{
				noerrors = FALSE;			
			}
		
								
		}


	}
	else{

		switch (opcodeGroup){
			/* two operands*/

			case FIRST_GROUP:{
				sourceOperand = strtok(temp, ",");/*the operands are separated by a comma*/
				destOperand = strtok(NULL, "");
				destOperand = trimString(destOperand);
				break;
			}
			/*single operand*/
			case SECOND_GROUP:{
				destOperand = strtok(temp, "");
				destOperand = trimString(destOperand);
				break;
			}
			/*opcode only*/
			case THIRD_GROUP:	
				break;
	
			default:
				break;

		}
			noerrors &= !makeBinaryCode(opcodeGroup, opcodeName, sourceOperand, destOperand);
	}


	if (noerrors){
		free(lineBuffer);
		return FALSE;
	}
	free(lineBuffer);
	return TRUE;
}

/*
Description:
Returns the opcode group number. 1 - first group, 2 - second group, 3 - third group 
*/
int getOpcodeGroup(char *opcodeName){
	
	int i;
	for (i = 0; i < FIRST_GROUP_SIZE; i++){
		if (strcmp(OpcodesFirstGroup[i], opcodeName) == 0)
			return FIRST_GROUP;
	}

	for (i = 0; i < SECOND_GROUP_SIZE; i++){
		if (strcmp(OpcodesSecondGroup[i], opcodeName) == 0)
			return SECOND_GROUP;
	}
	return THIRD_GROUP;
}
/* Encode jumping with parameters command */
bool makeBinaryCodeJumpingWithParameters(int opcodeGroup, char* opcode, char *destOperand, char* par1,char* par2){
	
	int i,j;
	int binaryWord[WORD_SIZE] = { 0 };
	int sourceParameter[PARAMETER_SIZE] = {0};
	int destParameter[PARAMETER_SIZE] = {0};
	int binaryOpcode[OPCODE_SIZE] = { 0 };
	int binaryDestOperand[OPERAND_SIZE] = { 0 };
	
	i=j=0;
	par1 = trimString(par1);
	par2 = trimString(par2);

	if (!isValidLabel(destOperand)){
		printRuntimeError(JUMPING_WITH_PARAMETERS_SHOULD_START_WITH_LABEL,destOperand);
		return FALSE;	
	}
	switch (opcodeGroup){
		case SECOND_GROUP:{
			setOpcodeBinaryCode(opcode, binaryOpcode); 
			makeSingleOperandBinary(destOperand, binaryDestOperand); /* encode destination label */

			if (isRegister(par1)){ /* is register */
				sourceParameter[0] = 1;
				sourceParameter[1] = 1;
			}
			else if (isValidLabel(par1)){ /* is label */
				sourceParameter[0] = 0;
				sourceParameter[1] = 1;
			}
			else{ /* immidiate */
				sourceParameter[0] = 0;
				sourceParameter[1] = 0;
			}
			
			if (isRegister(par2)){ /* is register */
				destParameter[0] = 1;
				destParameter[1] = 1;
			}

			else if (isValidLabel(par2)){ /* is label */
				destParameter[0] = 0;
				destParameter[1] = 1;
			}

			else{ /* immidiate */
				destParameter[0] = 0;
				destParameter[1] = 0;
			}

			break;
		}
		default:
			printRuntimeError(JUMPING_WITH_PARAMETERS_ERROR,destOperand);
			return FALSE;
		break;
	}
	
	for (i=0;i<PARAMETER_SIZE; i++, j++)
		binaryWord[j] = sourceParameter[i];
	
	for (i=0;i<PARAMETER_SIZE;i++,j++)
		binaryWord[j] = destParameter[i];

	for (i=0; i < OPCODE_SIZE; i++, j++)
		binaryWord[j] = binaryOpcode[i];
	
	j+=2;

	for (i=0;i<OPERAND_SIZE; i++, j++)
		binaryWord[j] = binaryDestOperand[i];

	printBinaryWord(binaryWord);/*print the word in 2 weird base*/
	
	if (printLabelWeird(destOperand)){
		return TRUE;
	}
	return FALSE;

}
/*
Description:
The executor of line encoding.
opcodeGroup - opcode group number
opcode - opcoda name
sourceOperand -  source Operand
destOperand - destination Operand
*/
bool makeBinaryCode(int opcodeGroup, char* opcode, char* sourceOperand, char *destOperand){
	
	int i,j;

	int binaryWord[WORD_SIZE] = { 0 };

	int binaryOpcode[OPCODE_SIZE] = { 0 };
	int binarySourceOperand[OPERAND_SIZE] = { 0 };
	int binaryDestOperand[OPERAND_SIZE] = { 0 };

	i=0;
	j=4;

	if (!isFileExists(objFileName)){
		printFileError(OPEN_FILE_ERROR, objFileName);
		return FALSE;
	}

	switch (opcodeGroup){
		case THIRD_GROUP:{
			setOpcodeBinaryCode(opcode, binaryOpcode);
			break;
		}
		case SECOND_GROUP:{
			setOpcodeBinaryCode(opcode, binaryOpcode);
			makeSingleOperandBinary(destOperand, binaryDestOperand); /* addressing destination operand */
			break;
		}
		case FIRST_GROUP:{
			setOpcodeBinaryCode(opcode, binaryOpcode);
			makeSingleOperandBinary(sourceOperand, binarySourceOperand); /* addressing source operand */
			makeSingleOperandBinary(destOperand, binaryDestOperand); /* addressing destination operand */
			break;
		}
		default: break;
	}
	/* concatenate binary era+opcodes+parameters to binary word */
	for (i = 0; i < OPCODE_SIZE; i++, j++)
		binaryWord[j] = binaryOpcode[i];
	
	for (i = 0; i < OPERAND_SIZE; i++, j++)
		binaryWord[j] = binarySourceOperand[i];

	for (i = 0; i < OPERAND_SIZE; i++, j++)
		binaryWord[j] = binaryDestOperand[i];
	
	printBinaryWord(binaryWord);/*print the word in 2 weird base*/

	if (makeOperandsWeird(opcodeGroup, binarySourceOperand, binaryDestOperand, sourceOperand, destOperand))
		return TRUE; 
	
	return TRUE;
}

/*
Description:
Converts a single operand to binary for the line encodding .
operand - operand string
binary - binary array
*/
void makeSingleOperandBinary(char *operand, int *binary){
	

	/* IMMEDIATE addressing 0*/
	if (isNumOperand(operand)){
		binary[0] = 0;
		binary[1] = 0;
	}
	/* DIRECT_REGISTER addressing 3*/
	else if (isRegister(operand)){
		binary[0] = 1;
		binary[1] = 1;
	}
	/* DIRECT addressing 2*/
	else if(isValidLabel(operand)){
		/* check if label is data command or extern command */
		if (isLabelData(&symbolListHead,operand)){
			binary[0] = 0;
			binary[1] = 1;
		}
		else if (isLabelExtern(&symbolListHead,operand) || isLabelEntry(&symbolListHead,operand)){
			binary[0] = 1;
			binary[1] = 0;	
		}
		else{
			binary[0] = 1;
			binary[1] = 0;	
		}
		/* not clear yet */ 
		
	}
	/* DIRECT ADDRESSING 1 (.data, .string) */
	else {
		binary[0] = 0;
		binary[1] = 1;	
	}
	
	
}

/*
Description:
Converts binary word to weird 2 base and prints its encodding to the object output file.
binaryWord - binary word array
*/
void printBinaryWord(int *binaryWord){

	char weirdWord[MAX_2_WEIRD_LENGTH] = { 0 };

	binaryToWierd(binaryWord, weirdWord, WORD_SIZE);

	objFile = fopen(objFileName, "ab");

	if (!objFile){
		printFileError(OPEN_FILE_ERROR, objFileName);
		return;
	}

	fprintf(objFile, "%ld\t%s\n", ICounter, weirdWord);
	ICounter++;
	fclose(objFile);
}

/*
Description:
Converts an operand to binary.
operand - opcode name
binary - binary array
*/
void setOpcodeBinaryCode(char *operand, int *binary){
	int i = 0;
	for (i = 0; i < NUM_OF_OPCODES; i++){
		if (strcmp(((opcodes[i]).opcodeName), operand) == 0){
			copyBinaryOpcode(i, binary);
			return;
		}
	}
}

/*
Description:
Copy the binary opcode.
index - index of the opcode in the opcodes array
binary - binary array
*/
void copyBinaryOpcode(int index, int *binary){
	int i = 0;
	for (i = 0; i < OPCODE_SIZE; i++){
		binary[i] = (opcodes[index].binaryCode)[i];
	}
}

/*
Description:
Prints the source operand and the destination in weird 2 base.
opcodeGroup - opcode group name
binarySourceOperand - source operand group
binaryDestOperand - destination operand group
sourceOperand - source operand string
destOperand - destination operand string
*/
bool makeOperandsWeird(int opcodeGroup, int *binarySourceperand, int *binaryDestOperand, char *sourceOperand, char *destOperand){

	int sourceAddressingType, destAddressingType;

	/* no operand to encode */
	if (opcodeGroup == THIRD_GROUP)
		return TRUE;

	sourceAddressingType = destAddressingType = 0;

	destAddressingType = getAddressingType(binaryDestOperand); /* 0 immediate, 1-direct memory, 2-jumping with parameters, 3- direct register */

	/* encode single operand word */
	if (opcodeGroup == SECOND_GROUP){
		if (!printDestOperand(sourceOperand, destOperand, destAddressingType))
			return FALSE;
		else
			return TRUE;
	}
	
	/*case - FIRST_GROUP*/
	sourceAddressingType = getAddressingType(binarySourceperand);

	switch (sourceAddressingType){
		/*first operand is a number*/		
		case 0:{
			printNumberWeird(objFileName, sourceOperand);
			if (!printDestOperand(sourceOperand, destOperand, destAddressingType))
				return FALSE;
			break;
		}
		/*first operand is data*/
		case 1:{
			if(!printWeirdDataOperand(sourceOperand))
				return FALSE;
			if(!printDestOperand(sourceOperand, destOperand, destAddressingType))
				return FALSE;
			break;
		}

		/*first operand label */
		case 2:{
			if (!printLabelWeird(destOperand))
				return FALSE;			
			break;
		}
	
		/*first oeprand is register*/
		case 3:{
			if (isRegister(destOperand)){
				printRegisterWeird(sourceOperand, destOperand, TRUE, TRUE);
			}
			else{
				printRegisterWeird(sourceOperand, destOperand, TRUE, FALSE);
				/* if second is a label addressing type is 01 */
			
				if (!printDestOperand(sourceOperand, destOperand, destAddressingType))
					return FALSE;
			}
			break;
		}
		default: break;
	}
	return TRUE;
}

/*
Description:
Prints a register in weird 2 base.
sourceOperand - source operand string
destOperand - dest operand string
sourceRegister - is source operand register
destRegister - is dest operand register
*/
void printRegisterWeird(char *sourceOperand, char *destOperand, bool sourceRegister, bool destRegister){

	int i;
	int binaryWord[WORD_SIZE] = { 0 };
	char weirdWord[MAX_2_WEIRD_LENGTH] = { 0 };

	for (i = 0; i < NUM_OF_REGISTERS; i++){
		if ((sourceRegister) && strcmp(((registersBinary[i]).registerName), sourceOperand) == 0){
			copyBinaryRegister(binaryWord, i, "first");
		}
		if ((destRegister) && strcmp(((registersBinary[i]).registerName), destOperand) == 0){
			copyBinaryRegister(binaryWord, i, "second");
		}
	}

	binaryToWierd(binaryWord, weirdWord, WORD_SIZE);

	objFile = fopen(objFileName, "ab");

	if (!objFile){
		printFileError(OPEN_FILE_ERROR, objFileName);
		return;
	}

	fprintf(objFile, "%ld\t%s\n", ICounter, weirdWord);
	ICounter++;

	fclose(objFile);
}

/*
Description:
Copy the binary register code.
binaryWord - binary array
index - index of the register in the registersBinary structure
operandType - "first" or "second"
*/
void copyBinaryRegister(int *binaryWord, int index, char* operandType){
	int i;
	int j = 0;
	/* source register print first in binary word */
	if ((strcmp(operandType, "first") == 0)){
		for (i=2; i < OPCODE_SIZE + 2; i++, j++){
			binaryWord[i] = (registersBinary[index].address)[j];
		}
	}
	else{
		for (i=8; i < OPCODE_SIZE + 8; i++, j++){
			binaryWord[i] = (registersBinary[index].address)[j];
		}
	}
}


/*
Description:
Prints a number in weird 2 base.
objFileName - object File Name
operand - operand string to print in weird base
*/
void printNumberWeird(char* objFileName, char *firstOperand){

	int number = getNumber(firstOperand);
	int binaryWord[WORD_SIZE] = { 0 };
	char weirdWord[WORD_SIZE] = { 0 };
	
	decimalToBinary(number,binaryWord,TWELVE_BITS);
	binaryWord[12] = 0;
	binaryWord[13] = 0;

	binaryToWierd(binaryWord, weirdWord, WORD_SIZE);

	objFile = fopen(objFileName, "ab");
	if (!objFile){
		printFileError(OPEN_FILE_ERROR, objFileName);
		return;
	}	

	fprintf(objFile, "%ld\t%s\n", ICounter, weirdWord);
	ICounter++;
	fclose(objFile);
}

/*
Description:
Prints the second opernd in weird 2 base.
sourceOperand - first operand string
destOperand - desination operand string
destAddressingType - desination operand addressing type
*/
bool printDestOperand(char *sourceOperand, char *destOperand, int destAddressingType){


	switch (destAddressingType){
		/* immediate */
		case 0:{
			printNumberWeird(objFileName, destOperand);
			break;
		}
		/*second operand is data*/
		case 1:{
			if (!printWeirdDataOperand(destOperand))
				return FALSE;
			break;
		}
		/*second operand is label*/	
		case 2:{
			if (!printLabelWeird(destOperand))
				return FALSE;			
			break;
		}
		/*second operand is a register*/
		case 3:{
			printRegisterWeird(sourceOperand, destOperand, FALSE, TRUE);
			break;
		}
		default: 
			break;
	}
	return TRUE;
}

bool printLabelWeird(char *operand){
	
	char weirdWord[MAX_2_WEIRD_LENGTH] = { 0 };
	int binaryWord[WORD_SIZE] = { 0 };
	symbolPtr searchLabel = symbolListHead;

	
	objFile = fopen(objFileName, "ab");
	if (!objFile){
		printFileError(OPEN_FILE_ERROR, objFileName);
		return FALSE;
	}

	/* check if label exists in symbol list and has address != 0 */
	if (isLabelDefined(&searchLabel,operand)){
		/* if address is not set print IC */
		while(searchLabel){
			/*find the node that hands that label and print address of label to object file in weird base */
			if (strcmp((searchLabel->name), operand) == 0){
				decimalToBinary((searchLabel->address),binaryWord,TWELVE_BITS);
				/* is external */
				if ((searchLabel->isExternal) == TRUE){ 
					binaryWord[12] = 0;
					binaryWord[13] = 1;
					printToExternFile(operand);
				}
				/* relocatable */
				else{ 
					binaryWord[12] = 1;
					binaryWord[13] = 0;
				}
					
				binaryToWierd(binaryWord,weirdWord,WORD_SIZE);

				fprintf(objFile, "%ld\t%s\n", ICounter, weirdWord);
				ICounter++;
				fclose(objFile);
				return TRUE;
			}
			searchLabel = searchLabel->next;
		}		
	}
	return FALSE;
}
/*
Description:
Prints a data opernd in weird 2 base.
operand - operand string
*/
bool printWeirdDataOperand(char *operand){

	bool found;	
	int addressNum;
	char *operandBuffer;
	symbolPtr searchLabel = symbolListHead;

	int binaryWord[WORD_SIZE] = { 0 };
	char weirdWord[MAX_2_WEIRD_LENGTH] = { 0 };

	operandBuffer = (char*)malloc(strlen(operand) + 2);

	if (!operandBuffer){
		printError(ALLOCATE_MEMORY_ERROR);
		return FALSE;
	}

	operand = trimString(operand);
	
	strncpy(operandBuffer, operand, strlen(operand) + 1);
	operandBuffer[strlen(operand) + 1] = '\0';

	found = FALSE;

	objFile = fopen(objFileName, "ab");
	if (!objFile){
		printFileError(OPEN_FILE_ERROR, objFileName);
		free(operandBuffer);
		return FALSE;
	}

	while (searchLabel){
		/*find the node that hands that label*/
		if (strcmp((searchLabel->name), operand) == 0){
			found = TRUE;
			/* converts label to binary */
			decimalToBinary(addressNum = (searchLabel->address), binaryWord, TWELVE_BITS);
			
			if ((searchLabel->isExternal) == TRUE){ /* external */
				binaryWord[12] = 0;
				binaryWord[13] = 1;
				printToExternFile(operand);
			}
			else{ /* relocatable */
				binaryWord[12] = 1;
				binaryWord[13] = 0;
			}
			binaryToWierd(binaryWord, weirdWord, WORD_SIZE);
			fprintf(objFile, "%ld\t%s\n", ICounter, weirdWord);
			ICounter++;
			break;
		}
		searchLabel = searchLabel->next;
	}
	
	if (!found){
		printRuntimeError(RUNTIME_ERROR_DECLARING_REGISTER_OR_LABEL, operandBuffer);
		free(operandBuffer);
		return FALSE;
	}
	free(operandBuffer);
	fclose(objFile);
	return TRUE;
}



/*
Description:
Returns an operand addressing type
binaryOperand - binary operand
*/
int getAddressingType(int* binaryOperand){
	int res = 0;
	res = 2 * binaryOperand[0] + binaryOperand[1];
	return res;
}

/*
Description:
reset the files names
objFileName - object File Name
entFileName - entry File Name
extFileName - extern File Name
*/
void resetFilesName(char *objFileName, char *entFileName, char *extFileName){
	int i;
	for (i = 0; i < MAX_FILE_NAME_LENGTH + 1; i++){
		objFileName[i] = 0;
		entFileName[i] = 0;
		extFileName[i] = 0;
	}
}

/*
Description:
reset a single line
line - line to reset
*/
void resetSingleLine(char *line){
	int i;
	for (i = 0; i < MAX_LINE_LENGTH + 1; i++){
		line[i] = 0;
	}
}

