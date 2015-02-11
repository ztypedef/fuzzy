#include <stdio.h>
#include "fuzzy.h"
#include "memcheck.h"

//git clone git://gitorious.org/gnuplot-iostream/gnuplot-iostream.git

int main(int argc, char *argv[])
{
	for(int i = 0; i < argc; i++)
	{
		printf("args: %s\n", argv[i]);
	}
	float value_input[] = {15, 15};
	if(argc >= 2) 
	{
		float tmpf;
		int err = sscanf(argv[1], "%f", &tmpf);
		if(err != -1) value_input[0] = tmpf;
	}

	fuzzy::FIC *fic = new fuzzy::FIC();
	int id_in1 = fic->addvar("min", fuzzy::INPUT);
	int id_in2 = fic->addvar("l",   fuzzy::INPUT);
	
	int id_out = fic->addvar("mout", fuzzy::OUTPUT);
	
	float val_in11[] =  {1.0, 2.0, 7.0};
	float val_in12[] =  {6.0, 14.3, 20.0};
	float val_in13[] =  {16.0, 25.0, 30.0};
	
	float val_in21[] =  {1.0, 5.0, 8.0};
	float val_in22[] =  {7.0, 10.0, 30.0};
	
	float val_out1[] = {1.0, 5.0, 9.0};
	float val_out2[] = {3.0, 10.0, 14.0};
	float val_out3[] = {13.0, 26.0, 29.0};
	
	try
	{
		int id_i11 = fic->addmf_tri(id_in1, val_in11, fuzzy::INPUT);
		int id_i12 = fic->addmf_tri(id_in1, val_in12, fuzzy::INPUT);
		int id_i13 = fic->addmf_tri(id_in1, val_in13, fuzzy::INPUT);
		
		int id_i21 = fic->addmf_tri(id_in2, val_in21, fuzzy::INPUT);
		int id_i22 = fic->addmf_tri(id_in2, val_in22, fuzzy::INPUT);
		
		int id_o1 = fic->addmf_tri(id_out, val_out1, fuzzy::OUTPUT);
		int id_o2 = fic->addmf_tri(id_out, val_out2, fuzzy::OUTPUT);
		int id_o3 = fic->addmf_tri(id_out, val_out3, fuzzy::OUTPUT);
		//printf("id: %i %i %i %i %i %i\n", id_i1, id_i2, id_i3, id_o1, id_o2, id_o3);
		printf("id input  var 1: %i %i %i\n", id_i11, id_i12, id_i13);
		printf("id input  var 2: %i %i\n",    id_i21, id_i22);
		printf("id output var 1: %i %i %i\n", id_o1, id_o2, id_o3);
					  //in  //out   //weight //operator
		int r[3][5] = {{id_i11, id_i21, id_o1,  1, 0},
					   {id_i12, id_i21, id_o2,  1, 0},
					   {id_i13, id_i22, id_o3,  1, 0}};
		
		fic->addrule(r[0], 3, 5);
		fic->fuzzification(value_input);
		
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
