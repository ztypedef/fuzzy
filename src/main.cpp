#include <stdio.h>
#include "fuzzy.h"



int main()
{
	
	//printf("[] sizeof var: %i\n", sizeof(fuzzy::Var));
	//printf("[] sizeof trimf: %i\n", sizeof(fuzzy::trimf));	
	
	fuzzy::FIC fic;
	int id_in = fic.addvar("min");
	int id_out = fic.addvar("mout");
	
	float val[] = {1.0, 10.0, 15.0};
	float val_out[] = {10.0, 12.0, 65.0};
	try
	{
		int id_i1 = fic.addmf_tri(id_in, val);
		int id_o1 = fic.addmf_tri(id_out, val_out);
		fic.addrule(id_i1, id_o1);
	}
	catch (const char* ch)
	{
		printf("[exeption] error: %s\n", ch);
		return -1;
	}
	
	return 0;
}
