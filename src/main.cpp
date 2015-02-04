#include <stdio.h>
#include "fuzzy.h"



int main()
{
	
	//printf("[] sizeof var: %i\n", sizeof(fuzzy::Var));
	//printf("[] sizeof trimf: %i\n", sizeof(fuzzy::trimf));	
	
	fuzzy::FIC fic;
	int id_in = fic.addvar("min", fuzzy::INPUT);
	int id_out = fic.addvar("mout", fuzzy::OUTPUT);
	
	float val[] = {1.0, 10.0, 15.0};
	float val_out[] = {10.0, 12.0, 65.0};
	try
	{
		int id_i1 = fic.addmf_tri(id_in, val);
		int id_i2 = fic.addmf_tri(id_in, val);
		int id_o1 = fic.addmf_tri(id_out, val_out);
		int id_o2 = fic.addmf_tri(id_out, val_out);
					  //in  //out   //weight //operator
		int r[2][4] = {{id_i1,  id_o1,  1, 1},
					   {id_i2,  id_o2,  1, 0}};
		
		fic.addrule(r[0], 2, 4);
	}
	catch (const char* ch)
	{
		printf("[exeption] error: %s\n", ch);
		return -1;
	}
	
	return 0;
}
