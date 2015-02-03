#include <stdio.h>
#include "fuzzy.h"

int main()
{
	printf("test\n");
	fuzzy::FIC fic;
	int id = fic.addmf("m1");
	//printf("[main] id mf: %i\n", id);
	
	float val[] = {1.0, 10.0, 15.0};
	fuzzy::Var v = fuzzy::Var(fuzzy::TRN, val);
	fic.addvar(id, &v);
	
	
	return 0;
}
