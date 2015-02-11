#include <stdio.h>
#include "fuzzy.h"
#include "memcheck.h"

//git clone git://gitorious.org/gnuplot-iostream/gnuplot-iostream.git

int main(int argc, char *argv[])
{
	//printf("[] sizeof var: %i\n", sizeof(fuzzy::MF));
	//printf("[] sizeof trimf: %i\n", sizeof(fuzzy::trimf));
	for(int i = 0; i < argc; i++)
	{
		printf("args: %s\n", argv[i]);
	}
	float value_input = 15;
	if(argc >= 2) 
	{
		float tmpf;
		int err = sscanf(argv[1], "%f", &tmpf);
		if(err != -1) value_input = tmpf;
	}

	fuzzy::FIC *fic = new fuzzy::FIC();
	int id_in = fic->addvar("min", fuzzy::INPUT);
	int id_out = fic->addvar("mout", fuzzy::OUTPUT);

	
	float val_in1[] =  {1.0, 2.0, 7.0};
	float val_in2[] =  {6.0, 14.3, 20.0};
	float val_in3[] =  {16.0, 25.0, 30.0};
	float val_out1[] = {1.0, 5.0, 9.0};
	float val_out2[] = {3.0, 10.0, 14.0};
	float val_out3[] = {13.0, 26.0, 29.0};
	try
	{
		int id_i1 = fic->addmf_tri(id_in, val_in1, fuzzy::INPUT);
		int id_i2 = fic->addmf_tri(id_in, val_in2, fuzzy::INPUT);
		int id_i3 = fic->addmf_tri(id_in, val_in3, fuzzy::INPUT);
		int id_o1 = fic->addmf_tri(id_out, val_out1, fuzzy::OUTPUT);
		int id_o2 = fic->addmf_tri(id_out, val_out2, fuzzy::OUTPUT);
		int id_o3 = fic->addmf_tri(id_out, val_out3, fuzzy::OUTPUT);
		//printf("id: %i %i %i %i %i %i\n", id_i1, id_i2, id_i3, id_o1, id_o2, id_o3);
					  //in  //out   //weight //operator
		int r[3][4] = {{id_i1, id_o1,  1, 1},
					   {id_i2, id_o2,  1, 1},
					   {id_i3, id_o3,  1, 1}};
		
		fic->addrule(r[0], 3, 4);
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
