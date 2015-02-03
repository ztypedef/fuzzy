#include <stdio.h>
#include "fuzzy.h"



int main()
{
	
	//printf("[] sizeof var: %i\n", sizeof(fuzzy::Var));
	//printf("[] sizeof trimf: %i\n", sizeof(fuzzy::trimf));	
	
	fuzzy::FIC fic;
	int id = fic.addvar("m1");
	float val[] = {1.0, 10.0, 15.0};
	try
	{
		fic.addmf_tri(id, val);
	}
	catch (const char* ch)
	{
		printf("[exeption] error: %s\n", ch);
		return -1;
	}
	
	return 0;
}
