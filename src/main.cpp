#include <stdio.h>
#include "fuzzy.h"
#include "memcheck.h"


int main()
{
	
	//printf("[] sizeof var: %i\n", sizeof(fuzzy::Var));
	//printf("[] sizeof trimf: %i\n", sizeof(fuzzy::trimf));	
	
	int k1 = 0x4;
	int* k = &k1;
	//MEMCHECK.add_ptr(k);
	//MEMCHECK.rm_ptr(k);
	
	
	fuzzy::FIC *fic = new fuzzy::FIC();
	int id_in = fic->addvar("min", fuzzy::INPUT);
	int id_out = fic->addvar("mout", fuzzy::OUTPUT);
	printf("if var: %i %i\n", id_in, id_out);
	
	float val[] = {1.0, 10.0, 15.0};
	float val_out[] = {10.0, 12.0, 65.0};
	try
	{
		int id_i1 = fic->addmf_tri(id_in, val, fuzzy::INPUT);
		int id_i2 = fic->addmf_tri(id_in, val, fuzzy::INPUT);
		int id_o1 = fic->addmf_tri(id_out, val_out, fuzzy::OUTPUT);
		int id_o2 = fic->addmf_tri(id_out, val_out, fuzzy::OUTPUT);
		printf("if mf: %i %i %i %i\n", id_i1, id_i2, id_o1, id_o2);
					  //in  //out   //weight //operator
		int r[2][4] = {{id_i1,  id_o1,  1, 1},
					   {id_i2,  id_o2,  1, 0}};
		
		fic->addrule(r[0], 2, 4);
		fic->fuzzification(5);
		
	}
	catch (const char* ch)
	{
		printf("[exeption] error: %s\n", ch);
		return -1;
	}
	delete fic;
	MEMCHECK.print_ptr();
	return 0;
}
