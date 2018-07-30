#include "assembler.h"

/*
Description: This function gets a file name and assembles a single file
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
	/* compliation succedded */

}
