#include <stdio.h>
#include <string.h>
#include "fuzzy.h"


using namespace fuzzy;

Var::Var(mftype mf_type, float *v)
{
	mft = mf_type;
	pvar = NULL;
	switch(mf_type)
	{
		case TRN:
			pvar = new float[3];
			printf("[var] new pointer: %p\n", pvar);
			memcpy(pvar, v, sizeof(float) * 3);
			break;
		
		default:
			printf("[Var] error: add parameter var\n");
			break;
	}
}

Var::~Var()
{
	if(pvar != NULL)
	{
		printf("[var] delete pointer: %p\n", pvar);
		delete [] pvar;
		pvar = NULL;
	}
}

FIC::~FIC()
{
	std::map<int, std::vector<Var>*>::iterator it = mfmap.begin();
	for (it=mfmap.begin(); it!=mfmap.end(); ++it) 
	{
		printf("[FIC] delete vector pointer: %p\n", it->second);
		delete it->second;
		mfmap.erase(it);
	}
}

int FIC::addmf(std::string mfname)
{
	std::vector<Var> *pv = new std::vector<Var>();
	printf("[FIC] new vector pointer: %p\n", pv);
	
	std::map<int, std::vector<Var>*>::iterator it = mfmap.end();
	int id = it->first + 1;
	it = mfmap.begin();
	mfmap.insert(it, std::pair<int, std::vector<Var>*>(id, pv));
	return id;	
}


int FIC::addvar(int idmf, Var *v)
{
	std::map<int, std::vector<Var>*>::iterator it = mfmap.find(idmf);
	std::vector<Var> *pv = it->second;
	pv->push_back();
}
