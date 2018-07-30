#include "GlobalsFunctions.h"

bool isRegister(char *str){
	int i;
	for (i = 0; i < NUM_OF_REGISTERS; i++){
		if (strcmp(str, Registers[i]) == 0)
			return TRUE;
	}
	return FALSE;
}

bool isOpcode(char *str){
	int i;
	for (i=0;i<NUM_OF_OPCODES;i++){
		if (!(strcmp(str, opcodes[i].opcodeName)))
			return TRUE;
	}
	return FALSE;
}

bool isLabel(char* src, char* dest){
	bool res, flag;
	char *temp, *start;
	int len;
	res = TRUE;
	flag = FALSE;
	temp = trimString(src);
	start = temp;
	len = 0;

	/* check if an extern declaration */
	if (strstr(temp, EXTERN_DECLARATION)){ 
		temp = strstr(temp, EXTERN_DECLARATION);
		temp = (char *)(temp + EXTERN_LENGTH);
		temp = trimLeftString(temp);
		if (checkLabel(src, EXTERN)){
			if (dest == '\0' || dest !=NULL){
				strcpy(dest, temp);
			}
			return TRUE;			
		}
		return FALSE;
	}  
	/* check if an entry declaration */
	if (strstr(temp, ENTRY_DECLARATION)){ 
		temp = strstr(temp, ENTRY_DECLARATION);
		temp = (char *)(temp + ENTRY_LENGTH);
		temp = trimLeftString(temp);
		if (checkLabel(src, ENTRY)){
			if (dest == '\0' || dest !=NULL){
				strcpy(dest, temp);
			}	
			return TRUE;			
		}
		return FALSE;
	}
	
	/* label cannot start with a dot */
	if (temp[0] == '.'){
		return FALSE;
	}

	/* check if a label starts with an alphabetic letter */
	if (!(isalpha(*temp))){
		printError(LABEL_NOT_START_WITH_ALPHA);
		res = FALSE;
	}
	
	/* validate label */
	while (*temp && !isspace(*temp) && strncmp((char*)(temp), ":", 1) != 0){
		if (!flag && *temp && !isalpha(*temp) && !isdigit(*temp)){
			printError(LABEL_CONTAINS_NON_ALPHA_OR_DIGIT);
			res = FALSE;
			flag = TRUE;
		}
		temp++;
		len++;
	} 
	if (strncmp((char*)(temp++), ":", 1) != 0){
		return FALSE;
	}
	if (len>MAX_LABEL_LENGTH){
		printError(LABEL_LENGTH_ERROR);
		return FALSE;
	}

	if (dest !=NULL || dest =='\0'){
		strncpy(dest, start, len);
	}	

	return res;
}

bool isLabelData(symbolPtr *head, char* labelname){
	symbolPtr temp = *head;
	while (temp){
		if (!(strcmp(labelname, (temp)->name))){
			if((temp)->isData){
				return TRUE;
			}
			return FALSE;
		}
		temp = (temp)->next;
	}

	return FALSE;
}
bool isLabelExtern(symbolPtr *head, char* labelname){
	symbolPtr temp = *head;
	while (temp){
		if (!(strcmp(labelname, (temp)->name))){
			if((temp)->isExternal){
				return TRUE;
			}
			return FALSE;
		}
		temp = (temp)->next;
	}
	return FALSE;
}
bool isLabelEntry(symbolPtr *head, char* labelname){
	symbolPtr temp = *head;
	while (temp){
		if (!(strcmp(labelname, (temp)->name))){
			if((temp)->isEntry){
				return TRUE;
			}
			return FALSE;
		}
		temp = (temp)->next;
	}
	return FALSE;
}
bool isLabelDefined(symbolPtr *symbols_h, char* labelname){
	return nodeExists(symbols_h,labelname);
}

bool isEntry(char* line){
	char *temp = trimString(line);
	if (strstr(temp, ENTRY_DECLARATION))
		return TRUE;
	return FALSE;
}


bool isExtern(char* line){
	char *temp = trimString(line);
	if (strstr(temp, EXTERN_DECLARATION))
		return TRUE;
	return FALSE;
}

bool isValidLabel(char* token){
	int i = 0, length;
	char* trimmed;
	trimmed = trimString(token);
	length = strlen(trimmed);

	if (length==0 || length>MAX_LABEL_LENGTH){
		printError(INVALID_LABEL_LENGTH);
		return FALSE;
	}
	if (trimmed[i] < 0 || trimmed[i] > 255){
		return FALSE;
	}
	if (!isalpha(trimmed[i++])){
		return FALSE;
	}
	for (; i < length; i++){
		if (trimmed[i] < 0 || trimmed[i] > 255 || trimmed[i] == '.')
			return FALSE;
		if (!(isalpha(trimmed[i]) || isdigit(trimmed[i])))
			return FALSE;
	}
	return TRUE;
}



bool checkLabel(char *line , char *type){
	int i;
	char* temp;
	bool res;
	res=TRUE;

	if (strcmp(type, EXTERN) == 0){
		temp = strstr(line,EXTERN_DECLARATION); /* pointer to extern label */
		temp += EXTERN_LENGTH; /* pass label */
		temp = trimString(temp);
	}
	else if (strcmp(type, ENTRY) == 0){
		temp = strstr(line, ENTRY_DECLARATION); /* pointer to entry label */
		temp += ENTRY_LENGTH; /* pass label */
		temp = trimString(temp);
	}	

	temp = trimString(temp);
	if (strlen(temp) == 0){
		printError(NO_LABEL_DECLARED_ERROR);
		return FALSE;
	}
	if (strlen(temp) > MAX_LABEL_LENGTH){
		printError(LABEL_LENGTH_ERROR);
		return FALSE;
	}
	if (!isalpha(temp[0])){
		printError(LABEL_NOT_START_WITH_ALPHA);
		res = FALSE;
	}
	for (i=1;i<strlen(temp);i++){
		if (!(isalpha(temp[i]) || isdigit(temp[i]))){
			if (isspace(temp[i])){
				printError(MORE_THEN_ONE_LABEL);
			}
			else{
				printError(LABEL_CONTAINS_NON_ALPHA_OR_DIGIT);
			}
			return FALSE;
		}
	}
	return res;
}


/* input: what comes after operand jmp,bne,jsr only 
   it can be only immidiate number, register or label

   input examples: L1(#-1,r4)
				   LOOP(K,W)
   output: if a valid jumping with parameters addressing declaration
*/
bool jumpingWithParameters(char* line){
	
	char *token;
	char *label,*par1,*par2,*tooMuchWordsFlag;
	
	/* L1(#-1,r4) */
	token = trimString(line); 
	
	if (!hasParanthesis(token)){
		return FALSE;		
	}
	
	label = strtok(line,"("); 	
	par1 = strtok(NULL,",");
	par2 = strtok(NULL,")");
	tooMuchWordsFlag = strtok(NULL, ",");

	if (tooMuchWordsFlag != NULL){
		printError(TOO_MUCH_OPERANDS);
		return FALSE;
	}

	if (label == NULL || par1 == NULL || par2 == NULL){
		return FALSE;
	}
	
	if (isKeyword(label) || isKeyword(par1) || isKeyword(par2)){
		printError(OPERAND_NAME_EQUALS_KEYWORD_NAME);
		return FALSE;
	}

	if (!isValidLabel(label)){
		printError("Jumping with parameters invalid label defined");
		return FALSE;
	}

	/* check for spaces inside paranthesis */
	if (stringHasSpace(par1) || stringHasSpace(par2)){
		printError(SPACE_INSIDE_PARAMETER_ERROR);				
		return FALSE;
	}

	if (isRegister(par1) || isNumOperand(par1) || isValidLabel(par1)){
		if (isRegister(par2) || isNumOperand(par2) || isValidLabel(par2)){
			return TRUE;
		}
		else{
			printError("Destination parameter should be label,register or number operand (ex. #5)");			
		}	
	}
	else{
			printError("Source parameter should be label,register or number operand (ex. #5)");	
	}	
	
	/* first parameter goes to r6 */
	/* second parameter goes to r7 */
	
	return FALSE;
}
bool isNumOperand(char* token){
	int  i = 0, num;
	char* trimmed;
	trimmed = trimString(token);

	if (trimmed[i++] != '#')
		return FALSE;

	num = atoi(trimmed + i);

	if (num == 0){
		if (trimmed[i] == '+' || trimmed[i] == '-')
			i++;
		if (strcmp(trimmed + 1, "0") != 0)
			return FALSE;
	}

	trimmed++;

	if (num > MAX_NUMBER || num < MIN_NUMBER)
	{
		if(num > MAX_NUMBER)
			printExtendedError(OVERFLOW_COMMAND_POSITIVE_NUMBER, trimmed);
		else 
			printExtendedError(OVERFLOW_COMMAND_NEGATIVE_NUMBER, trimmed);
		return FALSE;
	}
	else
		return TRUE;
}


bool isEmptySentence(char* line){
	char *temp;
	if (line == NULL){
		return TRUE;	
	}
	temp = trimString(line);

	if (strlen(temp)==0){
		return TRUE;	
	}
	return FALSE;
}

bool isComment(char* line){
	char *temp;
	if (line == NULL){
		return TRUE;	
	}
	temp = trimLeftString(line);
	if (temp[0] == ';')
		return TRUE;
	return FALSE;
}


char *trimLeftString(char *str)
{
	if (str == NULL)
		return NULL;
	while (isspace(*str) && *str >= 0 && *str <= 255)
		str++;
	return str;
}

char *trimRightString(char *str){
	int i, length;
	char *back;
	if (str == NULL)
		return NULL;
	length = strlen(str);
	if (length == 0)
		return str;
	back = str + length;
	while (*back=='\0' || isspace(*back)){
		back--;
		i++;
	}

	*(back+1) = '\0';
	return str;
}

char* trimString(char* str){
	char* res = trimRightString(trimLeftString(str));
	return res;
}

bool isFileExists(char* fileName){
	FILE* fh;
	bool res;
	fh = fopen(fileName, "r");
	if (fh == NULL)
		res = FALSE;
	else
		res = TRUE;
	fclose(fh);
	return res;
}
/* return symbol */
int whichSymbol(char *line, bool is_label){
	return 0;
}

/* get .string or .data symbol from line */
bool getSymbol(char* line, char* dest, bool is_label){

	bool afterLabel,detectedSymbol;
	int i, j, length;
	int correctSymbol;/* 0 - .string 1 - .data */
	


	afterLabel=detectedSymbol = FALSE;
	correctSymbol=-1;
	afterLabel = !is_label;
	length = strlen(line); 
	
	/* iterate line */
	for (i=0;i<length; i++){
		if (afterLabel){
			if (!isspace(line[i])){
				for (j=0;j<NUM_OF_DATA_TYPES;j++){
					if (strncmp(line+i,DataCommand[j],strlen(DataCommand[j])) == 0){
						if (detectedSymbol){
							return FALSE;
						}
						else{
							detectedSymbol=TRUE;
							correctSymbol=j;
							i+=strlen(DataCommand[j])-1;
							continue;
						}					
					}
				}
			}	
		}
		else{
			if (line[i] == ':'){
				if (i!=0)
					afterLabel = TRUE;
				else
					return FALSE;
			}
			else if (!isalpha(line[i])){
				if (i==0 && !isdigit(line[i]))
					return FALSE;
			}
			else{
				continue;			
			}
		}	
	}





	if (correctSymbol != -1){
		if (dest != NULL)
			strncpy(dest, DataCommand[correctSymbol], strlen(DataCommand[correctSymbol]));
		return TRUE;
	}
	return FALSE;
}

bool addNumberToDataList(dataPtr *head, dataPtr *tail, int dc, int num)
{
	if (num >= 0){
		if (num > MAX_DATA_NUMBER){
			printError(OVERFLOW_DATA_POSITIVE_NUMBER);
			return FALSE;
		}
		addToDataList(head, tail, dc, positiveNumber, num);
	}
	else{
		if (num < MIN_DATA_NUMBER){
			printError(OVERFLOW_DATA_NEGATIVE_NUMBER);
			return FALSE;
		}
		addToDataList(head, tail, dc, negativeNumber, num);
	}
	return TRUE;
}



AddressingMode getOperandAddressing(char* token){

	if (isNumOperand(token)) /* Immediate addressing 0 */
		return IMMEDIATE;
					
	if (isValidLabel(token)) /* Direct addressing 1 */
		return DIRECT_MEMORY;
		
	if (jumpingWithParameters(token)) 	/* Jumpring with parameters addressing 2 */
		return JUMPING_WITH_PARAMETERS;
	
	if (isRegister(token)) /* Direct register addressing 3 */
		return DIRECT_REGISTER;
						
	return -1;
}

bool hasParanthesis(char *line){
	char *temp;
	char *p1,*p2;

	temp = trimString(line);
	p1 = strstr(temp, "(");
    if (p1) {
        p2 = strstr(p1, ")");
        if (p2){
			return TRUE;
		}
    }
    return FALSE;;
}

bool validAddressingOperand(char *op, AddressingMode srcOpAddress, AddressingMode dstOpAddress){
	int i;	
	char *token;
	opcodeStructure *opcode;
	token = trimString(op);
	
	for (i=0;i<NUM_OF_OPCODES;i++){
	/* recognize opcode in command */
		if (strncmp(token, opcodes[i].opcodeName, MAX_OPCODE_LENGTH) == 0){

			opcode = &(opcodes[i]);
			
			switch (opcode->group){ /* lea mov cmp add sub */
				case FIRST_GROUP:{
					if (strcmp(opcode->opcodeName, "lea") == 0 && (srcOpAddress==DIRECT_MEMORY) && (dstOpAddress==DIRECT_REGISTER || dstOpAddress==DIRECT_MEMORY)){
						return TRUE;
					}
					/* cmp */
					if (strcmp(opcode->opcodeName, "cmp") == 0 && (srcOpAddress!=JUMPING_WITH_PARAMETERS) && (dstOpAddress!=JUMPING_WITH_PARAMETERS)){
						return TRUE;				
					}	
					/* mov add sub */				
					if ((srcOpAddress!=JUMPING_WITH_PARAMETERS) && (dstOpAddress==DIRECT_MEMORY || dstOpAddress==DIRECT_REGISTER) ){
						return TRUE;					
					}
					return FALSE;
				}
				break;
				case SECOND_GROUP:{ /* not clr inc dec red | jmp bne jsr | prn */
					
					if (strcmp(opcode->opcodeName, "jmp") == 0 && (dstOpAddress!=IMMEDIATE))
						return TRUE;
			
					if (strcmp(opcode->opcodeName, "bne") == 0 && (dstOpAddress!=IMMEDIATE))
						return TRUE;

					if (strcmp(opcode->opcodeName, "jsr") == 0 && (dstOpAddress=IMMEDIATE))
						return TRUE;

					if (strcmp(opcode->opcodeName, "prn") == 0 && (dstOpAddress!=JUMPING_WITH_PARAMETERS)){
						return TRUE;
					
					/* not clr inc dec red */				
					if ((dstOpAddress==DIRECT_MEMORY || dstOpAddress==DIRECT_REGISTER))
						return TRUE;					
					
					return FALSE;
					
				}
				break;
				case THIRD_GROUP:{
					return TRUE;
				}
				break;
				default:
					return FALSE;
				
			}
		}
	}
	}
	return FALSE;	
}
bool stringHasSpace(char *str){
	char *temp;
	temp = str;
	while (*temp){
		if (isspace(*temp)){
			printError(SPACE_INSIDE_PARAMETER_ERROR);
			return TRUE;
		}
		temp++;		
	}
	return FALSE;
}

int getCommandSize(char* command){

	int i, sizeOfCommand = 0;
	AddressingMode srcOperandAddressing;
	AddressingMode destOperandAddressing;
	opcodeStructure *opcode;
	
	bool is_addressing_with_parameters;
	char *token, *srcOperand, *destOperand, *tooMuchWordsFlag, *delim="\t,"; /* to much operands in command */
	char *label,*par1,*par2;
	
	is_addressing_with_parameters = FALSE;
	command = trimString(command);

	/* handle addressing with parameters command */
	if (hasParanthesis(command)){

		/* printf("-%s-",command); */
		
		token = strtok(command, delim); 
		token = trimString(token);
		for (i=0;i<NUM_OF_ADDRESSING_WITH_PARAMETERS;i++){
			if (strcmp(token,addressing_with_parameters_operands[i]) == 0){

				is_addressing_with_parameters = TRUE;

				label = strtok(NULL,"(");
				par1 = strtok(NULL,",");
				par2 = strtok(NULL,")");
				tooMuchWordsFlag = strtok(NULL, delim);
				
				if (!isValidLabel(label)){
					printError("Jumping with parameters invalid label defined");
					return 0;
				}

				/* check for spaces inside paranthesis */
				if (stringHasSpace(par1) || stringHasSpace(par2)){
					printError(SPACE_INSIDE_PARAMETER_ERROR);				
					return 0;
				}
				
				/*opcode instruction size*/
				sizeOfCommand++; 
				
				/* first parameters - label will take another word*/
				sizeOfCommand++; 

				/* if both second and third parameters are registers they will be encoded into single word */
				if (isRegister(par1) && isRegister(par2)){
					sizeOfCommand++;
				}
				else{
					sizeOfCommand+=2;				
				}

				if (tooMuchWordsFlag != NULL){
					printError(TOO_MUCH_OPERANDS);
					return 0;
				}

				return sizeOfCommand;
				break;				
			}	
		}
		
		if (!is_addressing_with_parameters){
			printError("jumping with parameters must have jmp,jsr,bne operands only");
			return 0;		
		}
	}
	else{
		token = strtok(command, delim); 		
	}		

	token = trimString(token); /* trim spaces for opcode */

	for (i=0;i<NUM_OF_OPCODES;i++){

		/* recognize opcode in command */
		if (strncmp(token, opcodes[i].opcodeName, MAX_OPCODE_LENGTH) == 0){

			opcode = &(opcodes[i]);
			
			switch (opcode->group){

				/* 2 Operands Opcode */
				case FIRST_GROUP:{

					srcOperand = strtok(NULL, delim); 
					destOperand = strtok(NULL, delim);	
					tooMuchWordsFlag = strtok(NULL, delim);
					
					srcOperand = trimString(srcOperand);
					destOperand = trimString(destOperand);
			
					/* printf("\n%s\t%s\t%s\n",command,srcOperand,destOperand); */
					
					if (tooMuchWordsFlag != NULL){
						printError(TOO_MUCH_OPERANDS_IN_COMMAND);
						return 0;
					}
					
					if (srcOperand == NULL){
						printError(SOURCE_OPERAND_ERROR);
						return 0;
					}
					
					if (destOperand == NULL){
						printError(DESTINATION_OPERAND_ERROR);
						return 0;
					}

					if (isKeyword(srcOperand) || isKeyword(destOperand)){
						printError(OPERAND_NAME_EQUALS_KEYWORD_NAME);
						return 0;
					}

					/* check operands addressing */
					if ((srcOperandAddressing = getOperandAddressing(srcOperand)) == -1){
						printError(INVALID_SRC_OPERAND);
						return 0;
					}

					if ((destOperandAddressing = getOperandAddressing(destOperand)) == -1){
						printError(INVALID_DEST_OPERAND);
						return 0;
					}
					/* source operand with lea opcode must be label */
 					if (strcmp(opcode->opcodeName, "lea") == 0 && (srcOperandAddressing == IMMEDIATE || srcOperandAddressing == DIRECT_REGISTER)){
						printError(LEA_COMMAND_ADDRESSING_MODE_ERROR);
						return 0;
					}
					
					
					if (destOperandAddressing == IMMEDIATE){
						if (strcmp(opcode->opcodeName, "cmp") != 0){
							printError(CMP_COMMAND_ADDRESSING_MODE_ERROR);
							return 0;
						}
					}
	
					if (tooMuchWordsFlag!=NULL){
						printError(TOO_MUCH_OPERANDS);					
					}
					
					/* opcode instruction size */
					sizeOfCommand++; 

					/* For example: "sub r1,r4" will require only one word to encode */
					if (isRegister(srcOperand) && isRegister(destOperand)){
						sizeOfCommand++;
					}
					else{
						sizeOfCommand += 2;					
					}
		
					/*printf("\n%s srcOp- %d destOp- %d CommandSize- %d\n",token,srcOperandAddressing,destOperandAddressing, sizeOfCommand);*/

					return sizeOfCommand;
			}
			break;

			/* 1 Operand Opcode*/
			case SECOND_GROUP:{
				destOperand = strtok(NULL, delim);						
				tooMuchWordsFlag = strtok(NULL, delim);
				
   				/* printf("\n%s\t%s",command,destOperand); */

				if (destOperand == NULL){
					printError(TOO_FEW_OPERANDS_IN_COMMAND);
					return 0;
				}

				if (isKeyword(destOperand)){
					printError(OPERAND_NAME_EQUALS_KEYWORD_NAME);
					return 0;
				}

				if ((destOperandAddressing = getOperandAddressing(destOperand)) == -1){
					printError(INVALID_DEST_OPERAND);
					return 0;
				}

				if (destOperandAddressing == IMMEDIATE){
					if (strcmp(opcode->opcodeName, "prn") != 0){
						printError(PRN_COMMAND_ADDRESSING_MODE_ERROR);
						return 0;
					}
				}

				if (tooMuchWordsFlag != NULL){
					printError(TOO_MUCH_OPERANDS);
					return 0;
				}

				/* opcode instruction */
				sizeOfCommand++;
	        
				/* destination encoding word */
				sizeOfCommand++;

				/* printf("\n%s destOp- %d CommandSize- %d\n",token,destOperandAddressing,sizeOfCommand); */
				return sizeOfCommand;
			}
			break;

			/* No Operands Opcode */
			case THIRD_GROUP:{

				/* printf("\n%s\n",command); */
				
				/* command size is 1 */
				tooMuchWordsFlag = strtok(NULL, delim);
				if (tooMuchWordsFlag != NULL){
					printError(TOO_MUCH_OPERANDS);
					return 0;
				}
				sizeOfCommand = 1;
				/* printf("\n%s no operands, CommandSize- %d \n",token, sizeOfCommand); */
				return sizeOfCommand;
			}
			break;

			default:
				return 0;
			}
			return 0;
		}
	}

	printError(INVALID_OPCODE_ERROR);
	return 0;
}

/* converts decimal to binary and binary to weird */
void decimalToWierd(int num, char* res){

	
	int binary_word[WORD_SIZE] = {0};
	
	/*case - num < 0*/
	if (num<0){
		num = MAX14BITS - (num*(-1)); /* 2's complement */
	}

	decimalToBinary(num,binary_word,WORD_SIZE);
	
	binaryToWierd(binary_word,res,WORD_SIZE);
	
}
/* convert int binary array to weird 2 char array */
void binaryToWierd(int *binary, char *res, int arrSize){
	int i;
	for (i=0;i<arrSize;i++){
		if (binary[i] == 0){
			res[i] = '.';
		}
		else{
			res[i] = '/';
		}	
	}
}

void decimalToBinary(int num, int *binaryNum, int arrSize){
	int i;
	/*case - num < 0*/
	if (num<0){
		num = MAX14BITS - num * (-1); /* 2's complement */
	}

	for (i = arrSize - 1; i >= 0; i--){
		binaryNum[i] = num % 2;
		num = num / 2;
	}

}

int binaryToDecimal(int *binaryNum){
	int i;
	int num = 0;
	int mult = 1;
	for (i = WORD_SIZE - 1; i >= 0; i--){
		num = num + binaryNum[i] * mult;
		mult = mult * 2;
	}
	return num;
}


bool isDataCommand(char* token){
	char * temp;
	temp = trimString(token);
	/* ignore label */
	while (!isspace(*temp)){
		temp++;
	}
	/* ignore spaces after label */
	while (isspace(*temp)){
		temp++;
	}
	
	if (!strncmp(temp, DataCommand[0], DataCommandLength[0]) || !strncmp(temp, DataCommand[1], DataCommandLength[1]))
		return TRUE;
	return FALSE;
}

int getNumber(char* token)
{
	char* temp;
	char * num;
	int end = 0;
	temp = trimString(token);

	/* ignore '#' */
	if (strncmp(temp, "#", 1) == 0){
		temp++;
	}

	num = temp;

	while (*temp >= 0 && *temp < MAX10BITS && !isspace(*temp)){
		end++;
		temp++;
	}
	return atoi(num);
}


bool isKeyword(char* str)
{
	int i = 0;
	char *tmp = trimString(str);
	for (i=0;i<NUM_OF_KEYWORDS;i++){
		if (strcmp(Keywords[i], tmp) == 0)
			return TRUE;
	}
	return FALSE;
}

bool isStructWithDotOperand(char* operand)
{
	int length;
	char* temp;
	char* token;
	int num;
	length = strlen(operand);
	temp = (char*)malloc(length + 1);
	if (!temp)
	{
		printError(ALLOCATE_MEMORY_ERROR);
		return FALSE;
	}
	strncpy(temp, operand, length + 1);
	token = strtok(temp, ".");
	trimLeftString(token);
	if (!isValidLabel(token))
	{
		free(temp);
		return FALSE;
	}
	token = strtok(NULL, ".");
	if (token == NULL)
	{
		free(temp);
		return FALSE;
	}
	token = trimRightString(token);
	num = atoi(token);
	if (num != 1 && num != 2)
	{
		free(temp);
		return FALSE;
	}
	token = strtok(NULL, ".");
	free(temp);
	return (token == NULL) ? TRUE : FALSE;

}

bool readLine(FILE* fp, char* line)
{
	bool flag = FALSE;
	lineCounter++;
	if (line == NULL)
		return FALSE;
	if (feof(fp))
		return FALSE;
	flag = (fgets(line, MAX_LINE_LENGTH, fp) != NULL) ? TRUE : FALSE;
	return flag;
}

void resetLine(char *line, char* labelName, char *data)
{
	int i;
	for (i = 0; i < MAX_LINE_LENGTH + 1; i++)
	{
		line[i] = 0;
		labelName[i] = 0;
		data[i] = 0;
	}
}


bool isNumber(char *token){
	int i=0;
	token = trimString(token);
	if (token[i] == '+' || token[i] == '-')
		i++;
	while(i<strlen(token)){
		if (!isdigit(token[i]))
			return FALSE;
		i++;
	
	}
	return TRUE;
}

/* check if string in format "string" appear */
bool isString(char *str){
	int i = 0;
	str = trimString(str);
	
	if (str != NULL && str[i++] == '"' && str[strlen(str) - 1] == '"'){
		for (; i < strlen(str) - 1; i++){
			if (isspace(str[i]))
				return FALSE;
		}
	}
	else{
		return FALSE;
	}
	return TRUE;
}


bool StartWithCommand(char * line){

	int i = 0, len = 0;
	char *token = trimString(line);
	char *temp = (char *)calloc(MAX_LINE_LENGTH, sizeof(char));
	for (i = 0; i < sizeof(temp); i++){
		if (isspace(token[i]))
			break;
		len++;
	}
	strncpy(temp, token, len);
	for (i = 0; i < NUM_OF_OPCODES; i++){
		if (!(strcmp(temp, opcodes[i].opcodeName))){
			free(temp);
			return TRUE;
		}
	}
	free(temp);
	return FALSE;
}

bool StartWithDataCommand(char * line){

	int i = 0;
	char *token;
	char temp[MAX_LINE_LENGTH + 1] = { 0 };
	token = trimString(line);
	
	for (i=0;i<MAX_LINE_LENGTH+1; i++){

		if ((*token && (isspace(token[0]))) || *token == '\0')
			break;

		temp[i] = token[0];
		token++;
	}
	/* check if .data or .string */
	for (i=0;i<NUM_OF_DATA_TYPES; i++){
		if (strcmp(temp, DataCommand[i]) == 0){
			return TRUE;
		}
	}
	return FALSE;
}
