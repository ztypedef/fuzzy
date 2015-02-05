#include <stdio.h>
#include <string.h>
#include "fuzzy.h"
#include "memcheck.h"


using namespace fuzzy;

MF::MF(float *v, int len)
{
	pvar = new float[len];
	MEMCHECK.add_ptr(pvar, "[MF] pvar");
	printf("[MF] new pointer: %p\n", pvar);
	memcpy(pvar, v, sizeof(float) * len);
}

MF::~MF()
{
	if(pvar != NULL)
	{
		printf("[MF] delete pointer: %p\n", pvar);
		MEMCHECK.rm_ptr(pvar);
		delete [] pvar;
		pvar = NULL;
	}
}


trimf::trimf(float *v) : MF(v, 3) {}

trimf::~trimf() {}

float trimf::get_fuzzynum(float v)
{
	if(pvar[1] < v && v < pvar[2])
		 return 1 - (v - pvar[1]) / (pvar[2] - pvar[1]);
	if(pvar[0] < v && v < pvar[1])
		return 1 - (pvar[1] - v) / (pvar[1] - pvar[0]);
	if(v == pvar[1])
		return 1.0f;
	return 0.0;
}

FIC::FIC()
{
	fic_rule = NULL;
}

FIC::~FIC()
{
	if(fic_rule != NULL)
	{
		for(int i = 0; i < rule_collumn; i++)
		{
			printf("[FIC] delete row rule: %p\n", fic_rule[i]);
			delete [] fic_rule[i];
		}
		printf("[FIC] delete collumn rule: %p\n", fic_rule);
		delete [] fic_rule;
		fic_rule = NULL;
	}
	
	for(auto it = input_var.begin(); it != input_var.end(); it++) 
	{
		std::map<int, MF*>* pmap = it->second;
		MF *pvard;
		while (!pmap->empty())
		{
			
			pvard = pmap->begin()->second;
			printf("[FIC] delete MF: %p\n", pvard);
			delete pvard;
			pmap->erase(pmap->begin());
		}
		printf("[FIC] delete vector pointer: %p\n", it->second);
		MEMCHECK.rm_ptr(it->second);
		delete it->second;
		input_var.erase(it);
	}
}

int FIC::addvar(std::string mfname, var_t type)
{
	std::map<int, MF*> *pv = new std::map<int, MF*>();
	printf("[FIC] new vector pointer: %p\n", pv);
	MEMCHECK.add_ptr(pv, "[FIC::addvar] pv");
	
	switch(type)
	{
		case INPUT:
		{
			std::map<int, std::map<int, MF*>*>::iterator it = input_var.end();
			int id = it->first + 1;
			it = input_var.begin();
			input_var.insert(it, std::pair<int, std::map<int, MF*>*>(id, pv));
			return id;
		}
		case OUTPUT:
		{
			std::map<int, std::map<int, MF*>*>::iterator it = output_var.end();
			int id = it->first + 1;
			it = output_var.begin();
			output_var.insert(it, std::pair<int, std::map<int, MF*>*>(id, pv));
			return id;
		}
		default:
			printf("[FIC] delete vector pointer: %p\n", pv);
			delete pv;
			break;
	}
	return -1;
}

int FIC::addmf(int idvar, MF *v, var_t type)
{
	std::map<int, std::map<int, MF*>*>::iterator itm;
	switch(type)
	{
		case INPUT:
			 itm = input_var.find(idvar);
			break;
		case OUTPUT:
			itm = output_var.find(idvar);
			break;
		default:
			return -1;
	}
	
	std::map<int, MF*> *pv = itm->second;
		
	std::map<int, MF*>::iterator it = pv->end();
	int id = it->first + 1;
	printf("[addmf] calc id: %i\n", id);
	it = pv->begin();
	pv->insert(it, std::pair<int, MF*>(id, v));
	return id;
}

int FIC::addmf_tri(int idvar, float *x, var_t type)
{
	if (x[0] > x[1]) throw "var 1 > var 2";
	if (x[1] > x[2]) throw "var 2 > var 3";
	trimf *tri = new trimf(x);
	printf("[FIC] new Triangular MF: %p\n", tri);
	return addmf(idvar, (MF*)tri, type);
}

void FIC::addrule(int rule[], int collumn, int row)
{
	if(fic_rule != NULL) return;
	if((input_var.size() + output_var.size() + 2) != (unsigned)row) throw "size of the matrix must be equal: ";
								
	rule_collumn = collumn;
	rule_row = row;
	fic_rule = new int*[collumn];
	printf("[FIC] new collumn rule: %p\n", fic_rule);
	
	for(int i = 0; i < collumn; i++)
	{
		fic_rule[i] = new int[row];
		printf("[FIC] new row rule: %p\n", fic_rule[i]);
		memcpy(fic_rule[i], &rule[i * row], sizeof(int) * row);
	}
}

MF* FIC::get_input_MF(int idvar, int idmf)
{
	std::map<int, MF*>* mapmf = input_var.find(idvar)->second;
	printf("[fuzzification] pointer mapmf: %p\n", mapmf);
	return mapmf->find(idmf)->second;
}

MF* FIC::get_output_MF(int idvar, int idmf)
{
	std::map<int, MF*>* mapmf = output_var.find(idvar)->second;
	return mapmf->find(idmf)->second;
}


float FIC::fuzzification(float value)
{
	int idvar = 1, idmf;
	
	for(unsigned n = 0; n < input_var.size(); n++)
	{
		idmf = fic_rule[idvar][n];
		printf("[fuzzification] id var: %i\n", idvar);
		MF* mf = get_input_MF(idvar, idmf);
		printf("[fuzzification] pointer mf: %p\n", mf);
		printf("[fuzzification] get mf value: %f\n", mf->get_fuzzynum(value));
		printf("[fuzzification] test \n");
	}
	
	return 0.0;
}
