#include "firstPass.h"


symbolPtr symbolListHead;
symbolPtr symbolListTail;
dataPtr dataListHead;
dataPtr dataListTail;
long IC;
long DC;

/*
Description: the main function of the first pass.
The first pass purpose is to:
1. Notify of compilation errors.
2. Parses from the input file the commands and their sizes and updates IC.
3. Parses from the input file the labels, entries, and externals, and updates DC.
*/
bool firstpass(char* filename){
	bool is_label, is_data_command, endFile, is_entry, is_extern, symbolFlag, noErrorsFlag, defined_label, correctLabel;
	FILE* fh;
	
	char line[MAX_LINE_LENGTH + 1] = { 0 };	
	char labelName[MAX_LINE_LENGTH + 1] = { 0 };
	char data[MAX_LINE_LENGTH + 1] = { 0 };
	
	/* initialize pointers */
	symbolListHead = NULL;
	symbolListTail = NULL;
	dataListHead = NULL;
	dataListTail = NULL;
	
	IC = IC_INITIALIZED; 
	DC = DC_INITIALIZED;

	is_label = is_data_command = endFile = is_entry = is_extern = symbolFlag = defined_label = correctLabel = FALSE;
	noErrorsFlag = TRUE;
	lineCounter = 0;
	
	/* check input file */
	fh = fopen(filename, "r");
	if (fh == NULL){
		printFileError(OPEN_FILE_ERROR,filename);
		return FALSE;
	}

	/*parse every line in file*/
	while ((endFile = readLine(fh, line)))
	{
		is_label = is_extern = is_entry = is_data_command = FALSE;

		if (isComment(line) || isEmptySentence(line)) {
			resetLine(line, labelName, data);
			continue;
		}
		
		is_entry = isEntry(line); /* true if .entry exists in line */
		
		is_extern = isExtern(line); /* true if .extern exists in line */		

		is_label = isLabel(line, labelName);  /* true if label exists in line and saves in labelName otherwise false */
		

		if (is_entry){
			noErrorsFlag &= is_label; /* entry declared but without a valid label is an error */
			resetLine(line, labelName, data);
			continue;
		}

		/* check if a label is a saved keyword and ignore if true */
		if (is_label && (isKeyword(labelName)|| (isRegister(labelName)))){
			if(isKeyword(labelName))
				printError(LABEL_NAME_EQUALS_KEYWORD_NAME);
			if(isRegister(labelName))
				printError(LABEL_NAME_EQUALS_REGISTER_NAME);
			resetLine(line, labelName, data);
			continue;
		}

		is_data_command = getSymbol(line, data, is_label);
			
		if (!is_extern && is_data_command ){
			/*case - the data definition are inside a label 
			LABEL: .string "abcd" 
			XYZ: .data 7,-57,17,9			
			*/

			if (is_label){ 
				defined_label = isLabelDefined(&symbolListHead, labelName); /* check if label already defined */
				if (!defined_label){
					/* addToSymbolsList (head, tail, label name, address, isExternal, isCommand, isData, isEntry) */
					noErrorsFlag &= addToSymbolsList(&symbolListHead, &symbolListTail, labelName, IC, FALSE, FALSE, TRUE, FALSE);
					/* check if symbol with .data .string declaration should have value IC or DC */				
				}
				else{
					printError("Label already defined");
					noErrorsFlag &= !defined_label; /*if label already defined then there is an error*/	
				}
			}
 			
			/*case - if not a valid data definition */
			if (!is_label & !StartWithDataCommand(line)){
				printError(INVALID_LABEL_OR_OPCODE_DECLARATION);
				noErrorsFlag = FALSE;
			}
	
			/*parse the data command*/
			else {
				noErrorsFlag &= ParseData(&dataListHead, &dataListTail, line, is_label, labelName);
			}
		}
		/*case - there is an extern declaration*/
		else if (is_extern){
			/*with a label*/

			if(is_label){
				noErrorsFlag &= AddExternLabelToSymbolsList(labelName);}
			else{ /* without a label */
				printError("extern declaration without a label ");
				resetLine(line, labelName, data);
				continue;
			}
		}
		/* case - command declaration */
		else if(!is_data_command){ 

			/* case - command inside a label*/
			if (is_label){ 
				defined_label = isLabelDefined(&symbolListHead, labelName);
				noErrorsFlag &= !defined_label; /*if label already defined then there is an error*/
				noErrorsFlag &= parseCommand(line, is_label, labelName); /* insert to symbol table symbol with IC */
			}
			/*case - if not a valid command definition */
			else if (!is_label & !StartWithCommand(line)){
				printError(INVALID_LABEL_OR_OPCODE_DECLARATION);
				noErrorsFlag = FALSE;
			}
			/*parse the command*/
			else{
				noErrorsFlag &= parseCommand(line, is_label, labelName);/*counter lines for the code(IC) and finds errors
																		does not encoding the commands, we do it in the
																		second pass*/
			}
		}
		resetLine(line, labelName, data);
	}
	
	if (!noErrorsFlag){
		freeSymbolsList(&symbolListHead);
		freeDataList(&dataListHead);
		return FALSE;
	}
	
	/* updateDataSymbols(&symbolListHead, IC); */
	fclose(fh);
	return TRUE;
}


/*
Description: parses the .data or .string commands and inserts it to the data list, and updates DC.
Returns true if no errors were found. 
*/
bool ParseData(dataPtr *dataListHead, dataPtr *dataListTail, char *line, bool is_label, char *labelName)
{
	bool hasError;
	int i, dataLength, strLength, numRequiredBytes, count;
	char *token, *temp, *dataTempBuffer;

	char stringBuffer[MAX_LINE_LENGTH + 1] = { 0 };
	char symbolType[MAX_LINE_LENGTH + 1] = { 0 };
	char *stringSymbol = DataCommand[0]; 
	char *dataSymbol = DataCommand[1];

	const int stringSymbolLength = STRING_COMMAND_LENGTH;
	const int dataSymbolLength = DATA_COMMAND_LENGTH;

	i = dataLength = strLength = numRequiredBytes = count = 0;
	dataLength = strlen(line);

	dataTempBuffer = (char*)malloc(dataLength + 1);
	hasError = FALSE;

	if (!dataTempBuffer){
		printError(ALLOCATE_MEMORY_ERROR);
		return FALSE;
	}

	strncpy(dataTempBuffer, line, dataLength + 1);

	/*get data type*/

	if (getSymbol(dataTempBuffer, symbolType, is_label) == FALSE){
		printError(INVALID_DATA_TYPE);
		free(dataTempBuffer);
		return FALSE;
	}

	temp = dataTempBuffer;

	if(is_label)
		temp += strlen(labelName) + 1;

	temp = trimLeftString(temp);
	
	/* .string */
	if (strncmp(symbolType, stringSymbol, stringSymbolLength) == 0){
		temp += stringSymbolLength;
		temp = trimString(temp);

		if (*temp != '\0' && !isString(temp)){
			printError(INVALID_CHARACTERS);
			free(dataTempBuffer);
			return FALSE;
		}
		strcpy(dataTempBuffer, temp);
		token = strtok(dataTempBuffer, "\"");
		if (token == NULL){
			printError(MISSING_ARGUMENTS_IN_DIRECTIVE);
			hasError = TRUE;
		}
		else{
			strcpy(stringBuffer, token);
			DC = addStringToData(dataListHead, dataListTail, stringBuffer, DC);	
			IC+=DC;  
			free(dataTempBuffer);
			return TRUE;
		}
	}
	/* .data */
	else if (strncmp(symbolType, dataSymbol, dataSymbolLength) == 0){
		temp += dataSymbolLength;
		temp = trimLeftString(temp);
		token = strtok(temp, ","); /* parse .data 6, -5,15 to 6 -5 -15 */
		
		if (token == NULL){
			printError(MISSING_ARGUMENTS_IN_DIRECTIVE);
			hasError = TRUE;
		}

		else{
			/* iterate tokens */			
			while (token != NULL){
				
				/* check if data member is number */
				if (!(isNumber(token))){
					printError(NOT_NUMBER_IN_DATA);
					hasError = TRUE;
				}

				else if (!hasError){
					i = atoi(trimString(token)); 

					if (i != 0 || (strcmp(token, "0") == 0)){
						if (!addNumberToDataList(dataListHead, dataListTail, DC, i)){
							free(dataTempBuffer);
							return FALSE;
						}
						DC++;
						IC++; 
					}
		
					else{
						printError(INVALID_NUMBER_IN_DATA_LABEL);
					}
	
				}

				token = strtok(NULL, ","); /* proceed to next token*/
			}
		}
	}
	
	else{
		free(dataTempBuffer);
		return FALSE;
	}
	free(dataTempBuffer);
	if (hasError)
		return FALSE;
	return TRUE;
}


/*
Description: gets a line that contains an extern label (.extern) and adds it to the symbols list.
Returns false if the label already defined.
*/
bool AddExternLabelToSymbolsList(char *label){
	char* temp = label;
	/* an extern label could be already defined, regardless it will be printed output on second pass to extern file */
	return addToSymbolsList(&symbolListHead, &symbolListTail, temp, 0, TRUE, FALSE, FALSE, FALSE);
}


/*
gets a line with a command (for example "mov r1, r2"), and if it has a label, adds it to the symbols list.
afterwards, calculates the size of the command in memory, and increments IC with the command size.
Returns true if no errors were found. 
*/
bool parseCommand(char *line, bool label, char* labelName){ /*with or without label*/
	int sizeOfCommand = 0;
	char *temp = line;
	temp = trimString(line);

	/* if label exits insert to symbolist */
	if (label){
		temp += strlen(labelName) + 1; /* skip label */
		if (!isLabelDefined(&symbolListHead, labelName)){
			addToSymbolsList(&symbolListHead, &symbolListTail, labelName, IC, FALSE, TRUE, FALSE, FALSE);
		}
	}

	temp = trimString(temp);
	sizeOfCommand = getCommandSize(temp);
	/* printf("LINE -%s IC %ld\n",line,IC); */

	IC += sizeOfCommand;	

	if (sizeOfCommand==0)
		return FALSE;

	return TRUE;
}
