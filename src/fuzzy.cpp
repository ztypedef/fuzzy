#include <stdio.h>
#include <string.h>
#include "fuzzy.h"


using namespace fuzzy;

Var::Var(float *v, int len)
{
	pvar = new float[len];
	printf("[Var] new pointer: %p\n", pvar);
	memcpy(pvar, v, sizeof(float) * len);
}

Var::~Var()
{
	if(pvar != NULL)
	{
		printf("[Var] delete pointer: %p\n", pvar);
		delete [] pvar;
		pvar = NULL;
	}
}


trimf::trimf(float *v) : Var(v, 3) {}

trimf::~trimf() {}

float trimf::get_mmbrsp(float v)
{
	if(pvar[1] < v && v < pvar[2])
		 return 1 - (v - pvar[1]) / (pvar[2] - pvar[1]);
	if(pvar[0] < v && v < pvar[1])
		return 1 - (pvar[1] - v) / (pvar[1] - pvar[0]);
	if(v == pvar[1])
		return 1.0f;
	return 0.0;
}

FIC::~FIC()
{
	std::map<int, std::map<int, Var*>*>::iterator it = mfmap.begin();
	for (it=mfmap.begin(); it!=mfmap.end(); ++it) 
	{
		std::map<int, Var*>* pmap = it->second;
		Var *pvard;
		while (!pmap->empty())
		{
			
			pvard = pmap->begin()->second;
			printf("[FIC] delete Var: %p\n", pvard);
			delete pvard;
			pmap->erase(pmap->begin());
		}
		printf("[FIC] delete vector pointer: %p\n", it->second);
		delete it->second;
		mfmap.erase(it);
	}
}

int FIC::addvar(std::string mfname)
{
	std::map<int, Var*> *pv = new std::map<int, Var*>();
	printf("[FIC] new vector pointer: %p\n", pv);
	
	std::map<int, std::map<int, Var*>*>::iterator it = mfmap.end();
	int id = it->first + 1;
	it = mfmap.begin();
	mfmap.insert(it, std::pair<int, std::map<int, Var*>*>(id, pv));
	return id;	
}


int FIC::addmf(int idvar, Var *v)
{
	std::map<int, std::map<int, Var*>*>::iterator itm = mfmap.find(idvar);
	std::map<int, Var*> *pv = itm->second;
		
	std::map<int, Var*>::iterator it = pv->end();
	int id = it->first + 1;
	it = pv->begin();
	pv->insert(it, std::pair<int, Var*>(id, v));
	return id;
}

int FIC::addmf_tri(int idvar, float *x)
{
	if (x[0] > x[1]) throw "var 1 > var 2";
	if (x[1] > x[2]) throw "var 2 > var 3";
	trimf *tri = new trimf(x);
	printf("[FIC] new Triangular MF: %p\n", tri);
	addmf(idvar, (Var*)tri);
	return 0;
}
