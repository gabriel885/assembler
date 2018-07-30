#include "main.h"

int main(int argc, char** argv)
{
	int i, j, filename_length;
	FILE *fp;
	
	i=j=filename_length=0;

	if (argc<2){
		printError(MISSING_INPUT_FILE);
	}

	for (i=1;i<argc;i++){

		filename_length = strlen(argv[i]);

		/* check filename length */
		if (filename_length>MAX_FILENAME_LENGTH){
			printf("%s\n",FILENAME_TOO_LONG);
			continue;	
		}

		AssemblyfileName = (char*)malloc(filename_length + strlen(INPUT_EXTENSION) + 1);

		/* check if memory allocation succedded */
		if (AssemblyfileName == NULL){
			printError(FILE_NAME_MEM_ALLOCATION_FAILURE);
			continue;
		}

		strncpy(AssemblyfileName, argv[i], filename_length);
		
		/* add extension to input file */
		for (j=0;j<strlen(INPUT_EXTENSION);j++){
			*(AssemblyfileName+filename_length+j) = INPUT_EXTENSION[j];
		}
		
		AssemblyfileName[filename_length+strlen(INPUT_EXTENSION)] = '\0';
		
		/* try open the file with input extension */
		if ((fp = fopen(AssemblyfileName,"r")) == NULL){
			printFileError(OPEN_FILE_ERROR,AssemblyfileName);
			continue;
		}
		fclose(fp); 
		runAssembler(AssemblyfileName);
		free(AssemblyfileName);
	}
	return EXIT_SUCCESS;
}
