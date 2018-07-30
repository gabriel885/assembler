#include "errors.h"


void printFileError(char* error, char* fileName)
{
	fprintf(stderr, "[%s] : %s %s - \"%s\"\n", AssemblyfileName, "[Error] -> ", error, fileName);
}

void printErrorWithArgument(char *error, char *arg){
	fprintf(stderr, "%s %s",error,arg);
}

void printError(char* error)
{
	fprintf(stderr, "[%s] : [IN LINE - %d] %s %s\n", AssemblyfileName, lineCounter, "[Error] -> ", error);
}
void printExtendedError(char* error, char* ref)
{
	fprintf(stderr, "[%s] : [IN LINE - %d] %s %s - \"%s\"\n", AssemblyfileName, lineCounter, "[Error] -> ", error, ref);
}

void printWarning(char* warn, char* ref)
{
	fprintf(stderr, "[%s] : [IN LINE - %d] %s %s - \"%s\"\n", AssemblyfileName, lineCounter, "[Warning]-> ", warn, ref);
}


void printRuntimeError(char* error, char* ref)
{
	fprintf(stderr, "[%s] : [IN LINE - %d] %s %s - \"%s\"\n", AssemblyfileName, lineCounter, "[RUNTIME ERROR] -> ", error, ref);
}
