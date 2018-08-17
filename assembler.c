#include "assembler.h"

/*
Description: This function gets a file name and assembles a single file output object file 
*/
void runAssembler(char* fileName){
	
	if (!firstpass(fileName)){ /* failed first Pass */
		printFileError(ERRORS_IN_FIRST_PASS, fileName);
		return;	
	}
	if (!secondPass(fileName)){ /* failed second Pass */
		printFileError(ERRORS_IN_SECOND_PASS, fileName);
		return;			
	}	
	return; /* compilation succeeded */

}
