#include <stdio.h>
#include <string.h>
#include "fuzzy.h"
#include "memcheck.h"


using namespace fuzzy;

MF::MF(float *v, int len)
{
	pvar = new float[len];
	MEMCHECK.add_ptr(pvar, "[MF] pvar");
	memcpy(pvar, v, sizeof(float) * len);
}

MF::~MF()
{
	if(pvar != NULL)
	{
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

void trimf::getx(float *x, float mu)
{
	x[0] = -(mu - 1) * (pvar[2] - pvar[1]) + pvar[1];
	x[1] = -(mu - 1) * (pvar[1] - pvar[0]) - pvar[1];
}


 #define FUZZY_TRI_WARNING(a) printf("[WARNING] limit of integration must lie in the triangle: %i\n", a)
float trimf::integral(float l, float r, float h)
{
	int method_intg = 0;
	if(l > r) throw "left margin must be greater than the right";
	if(l < pvar[0]) {l = pvar[0]; FUZZY_TRI_WARNING(0);};
	if(l > pvar[2]) {l = pvar[2]; FUZZY_TRI_WARNING(1);};
	if(r < pvar[0]) {r = pvar[0]; FUZZY_TRI_WARNING(2);};
	if(r > pvar[2]) {r = pvar[2]; FUZZY_TRI_WARNING(3);};
	
	if(l > pvar[1]) method_intg = 1;
	if(r < pvar[1]) method_intg = 2;
	float cr = 0, lc = 0, x[2];
	getx(x, h);
	
	switch(method_intg)
	{
		case 0:
			cr = (r - pvar[1]) * pvar[2] / (pvar[2] - pvar[1]) 
						- (r * r - pvar[1] * pvar[1]) / ( 2.0f * (pvar[2] - pvar[1]));
			lc = (pvar[1] * pvar[1] - l * l - (pvar[1] - l) * pvar[0] * 2.0f) / 2.0f / (pvar[1] - pvar[0]);
			break;
		case 1:
			cr = (r - l) * pvar[2] / (pvar[2] - pvar[1]) 
						- (r * r  - l * l) / ( 2.0f * (pvar[2] - pvar[1]));
			lc = (pvar[1] - pvar[0]) / 2);
			break;
		case 2:
			cr = (pvar[2] - pvar[1]) / 2;
			lc = (r * r - l * l - (r - l) * pvar[0] * 2.0f) / 2.0f / (pvar[1] - pvar[0]);
			break;
	}
	//printf("cr: %f; lc: %f\n", cr, lc);
	return cr + lc;
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
			MEMCHECK.rm_ptr(fic_rule[i]);
			delete [] fic_rule[i];
		}
		MEMCHECK.rm_ptr(fic_rule);
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
			MEMCHECK.rm_ptr(pvard);
			delete pvard;
			pmap->erase(pmap->begin());
		}
		
		MEMCHECK.rm_ptr(it->second);
		delete it->second;
		input_var.erase(it);
	}
	
	for(auto it = output_var.begin(); it != output_var.end(); it++) 
	{
		std::map<int, MF*>* pmap = it->second;
		MF *pvard;
		while (!pmap->empty())
		{
			
			pvard = pmap->begin()->second;
			MEMCHECK.rm_ptr(pvard);
			delete pvard;
			pmap->erase(pmap->begin());
		}
		
		MEMCHECK.rm_ptr(it->second);
		delete it->second;
		output_var.erase(it);
	}
}

int FIC::addvar(std::string mfname, var_t type)
{
	std::map<int, MF*> *pv = new std::map<int, MF*>();
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
	it = pv->begin();
	pv->insert(it, std::pair<int, MF*>(id, v));
	return id;
}

int FIC::addmf_tri(int idvar, float *x, var_t type)
{
	if (x[0] > x[1]) throw "var 1 > var 2";
	if (x[1] > x[2]) throw "var 2 > var 3";
	trimf *tri = new trimf(x);
	MEMCHECK.add_ptr(tri, "[FIC::addmf_tri] tri");
	return addmf(idvar, (MF*)tri, type);
}

void FIC::addrule(int rule[], int collumn, int row)
{
	if(fic_rule != NULL) return;
	if((input_var.size() + output_var.size() + 2) != (unsigned)row) throw "size of the matrix must be equal: ";
								
	rule_collumn = collumn;
	rule_row = row;
	fic_rule = new int*[collumn];
	MEMCHECK.add_ptr(fic_rule, "[FIC::addrule] fic_rule");
	
	for(int i = 0; i < collumn; i++)
	{
		fic_rule[i] = new int[row];
		MEMCHECK.add_ptr(fic_rule[i], "[FIC::addrule] fic_rule[i]");
		memcpy(fic_rule[i], &rule[i * row], sizeof(int) * row);
	}
}

MF* FIC::get_input_MF(int idvar, int idmf)
{
	std::map<int, MF*>* mapmf = input_var.find(idvar)->second;
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
	int k = 0;
	
	for(unsigned n = 0; n < input_var.size(); n++)
	{
		idmf = fic_rule[k][n];
		if(idmf == 0) continue;
		MF* mf = get_input_MF(idvar, idmf);
		printf("[fuzzification] get mf value: %f\n", mf->get_fuzzynum(value));
		printf("[fuzzification] get mf integral: %f\n", mf->integral(1, 3.0, 0.5));
	}
	
	return 0.0;
}
