#include <stdio.h>
#include "fuzzy.h"
#include "memcheck.h"


int main(int argc, char *argv[])
{
	/*
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
	}*/

	fuzzy::FIC *fic = new fuzzy::FIC();
	fic->set_xrange(0, 20, 0.1);
	int id_in1 = fic->addvar("x1", fuzzy::INPUT);
	//int id_in2 = fic->addvar("x2", fuzzy::INPUT);
	
	int id_out = fic->addvar("y1", fuzzy::OUTPUT);
	
	float val_in11[] =  {0.0,  0.0,  5.0};
	float val_in12[] =  {0.0,  5.0,  10.0};
	float val_in13[] =  {5.0,  10.0, 15.0};
	float val_in14[] =  {10.0, 15.0, 20.0};
	float val_in15[] =  {15.0, 20.0, 20.0};
	/*
	float val_in21[] =  {0.0,  0.0,  5.0};
	float val_in22[] =  {0.0,  5.0,  10.0};
	float val_in23[] =  {5.0,  10.0, 15.0};
	float val_in24[] =  {10.0, 15.0, 20.0};
	float val_in25[] =  {15.0, 20.0, 20.0};
	*/
	
	float val_out1[] =  {0.0,  0.0,  5.0};
	float val_out2[] =  {0.0,  5.0,  10.0};
	float val_out3[] =  {5.0,  10.0, 15.0};
	float val_out4[] =  {10.0, 15.0, 20.0};
	float val_out5[] =  {15.0, 20.0, 20.0};
	
	try
	{
		//int id_i11 = fic->addmf(fuzzy::INPUT, id_in1, "a", fuzzy::TRIMF, val_in11);
		int id_i11 = fic->addmf_tri(id_in1, val_in11, fuzzy::INPUT, "a");
		int id_i12 = fic->addmf_tri(id_in1, val_in12, fuzzy::INPUT, "b");
		int id_i13 = fic->addmf_tri(id_in1, val_in13, fuzzy::INPUT, "c");
		int id_i14 = fic->addmf_tri(id_in1, val_in14, fuzzy::INPUT, "d");
		int id_i15 = fic->addmf_tri(id_in1, val_in15, fuzzy::INPUT, "e");
		/*
		int id_i21 = fic->addmf_tri(id_in2, val_in21, fuzzy::INPUT, "g");
		int id_i22 = fic->addmf_tri(id_in2, val_in22, fuzzy::INPUT, "i");
		int id_i23 = fic->addmf_tri(id_in2, val_in23, fuzzy::INPUT, "k");
		int id_i24 = fic->addmf_tri(id_in2, val_in24, fuzzy::INPUT, "l");
		int id_i25 = fic->addmf_tri(id_in2, val_in25, fuzzy::INPUT, "m");
		*/
		int id_o1 = fic->addmf_tri(id_out, val_out1, fuzzy::OUTPUT, "oa");
		int id_o2 = fic->addmf_tri(id_out, val_out2, fuzzy::OUTPUT, "ob");
		int id_o3 = fic->addmf_tri(id_out, val_out3, fuzzy::OUTPUT, "oc");
		int id_o4 = fic->addmf_tri(id_out, val_out4, fuzzy::OUTPUT, "od");
		int id_o5 = fic->addmf_tri(id_out, val_out5, fuzzy::OUTPUT, "oe");

					  //in  //out   //weight //operator
		int r[5][4] = {{id_i11, id_o1,  1, 0},
					   {id_i12, id_o2,  1, 0},
					   {id_i13, id_o3,  1, 0},
					   {id_i14, id_o4,  1, 0},
					   {id_i15, id_o5,  1, 0}};
		
		fic->addrule(r[0], 5, 4);
		fic->gensurf();
		fic->plotmf(fuzzy::INPUT, 1);
		//fic->plotmf(fuzzy::INPUT, 2);
		fic->plotmf(fuzzy::OUTPUT, 1);
		//fic->getfis();
		//fic->rmmf(fuzzy::INPUT, 1, 1);
		//fic->rmvar(fuzzy::INPUT, 1);
		//fic->getfis();
		//int sr[] = {2, 1};
		//fic->showrule(sr, sizeof(sr) / sizeof(int));
		fic->showrule();
		
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
