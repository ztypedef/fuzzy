#include <stdio.h>
#include <string.h>
#include <cmath>
#include "fuzzy.h"
#include "memcheck.h"
#include "fuzzyplot.h"

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
	float mu = 0;
	if(pvar[1] < v && v < pvar[2])
		 mu = 1 - (v - pvar[1]) / (pvar[2] - pvar[1]);
	if(pvar[0] < v && v < pvar[1])
		 mu = 1 - (pvar[1] - v) / (pvar[1] - pvar[0]);
	if(v == pvar[1])
		mu = 1.0f;
	if(mu > trc_h) mu = trc_h;
	return mu;
}

void trimf::getx(float *x, float* n, float mu)
{
	x[0] =  (mu - 1) * (pvar[1] - pvar[0]) + pvar[1];
	x[1] = -(mu - 1) * (pvar[2] - pvar[1]) + pvar[1];
	*n = 2;
}

std::vector<float> trimf::getvx(float mu)
{
	std::vector<float> vf;
	float tmp;
	if((tmp = +(mu - 1) * (pvar[1] - pvar[0]) + pvar[1]) > trc_h) tmp = trc_h;
	vf.push_back(trc_h);
	if((tmp = -(mu - 1) * (pvar[2] - pvar[1]) + pvar[1]) > trc_h) tmp = trc_h;
	vf.push_back(tmp);
	return vf;
}

float trimf::integral_left(float l, float r)
{
	return (r * r - l * l 
		 - (r - l) * pvar[0] * 2.0f) / 2.0f / (pvar[1] - pvar[0]);
}

float trimf::integral_right(float l, float r)
{
	return (r - l) * pvar[2] / (pvar[2] - pvar[1]) 
		  - (r * r - l * l) / ( 2.0f * (pvar[2] - pvar[1]));
}

#define FUZZY_TRI_WARNING(a) printf("[WARNING] limit of integration must lie in the triangle: %i\n", a)
float trimf::integral(float l, float r, float h)
{
	float cr = 0, lc = 0, x[2];
	float rest_l = 0, rest_r = 0, n;
	int method_intg = 0;
	if(l > r) throw "left margin must be greater than the right";
	if(l < pvar[0]) {l = pvar[0]; FUZZY_TRI_WARNING(0);};
	if(l > pvar[2]) {l = pvar[2]; FUZZY_TRI_WARNING(1);};
	if(r < pvar[0]) {r = pvar[0]; FUZZY_TRI_WARNING(2);};
	if(r > pvar[2]) {r = pvar[2]; FUZZY_TRI_WARNING(3);};
	
	if(l > pvar[1]) method_intg = 1;
	if(r < pvar[1]) method_intg = 2;
	getx(x, &n, h);
	if(x[0] > x[1]) throw "critical error";
	printf("l: %f; r: %f\n", l, r);
	if(l > x[0]) x[0] = l;
	if(x[1] > r) x[1] = r;
	
	switch(method_intg)
	{
		case 0:
			lc = integral_left(l, x[0]);
			if(x[0] < pvar[1]) x[0] = l;
			rest_l = (pvar[1] - x[0]) * h;
			cr = integral_right(x[1], r);
			if(x[1] < pvar[1]) x[1] = pvar[1];
			rest_r = (x[1] - pvar[1]) * h;
			break;
		case 1:
			lc = integral_left(l, x[0]);
			if(x[0] < pvar[1]) x[0] = l;
			rest_l = (pvar[1] - x[0]) * h;
			break;
		case 2:
			cr = integral_right(x[1], r);
			if(x[1] < pvar[1]) x[1] = pvar[1];
			rest_r = (x[1] - pvar[1]) * h;
			break;
	}
	printf("x[0] = %f, x[1] = %f\n", x[0], x[1]);
	printf("cr: %f; lc: %f; rest_l: %f; rest_r: %f\n", cr, lc, rest_l, rest_r);
	return cr + lc + rest_l + rest_r;
}

float trimf::integral(float h)
{
	return integral(pvar[0], pvar[2], h);
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

#define MAX_MF 1.0
float FIC::intersecting_mf(MF* a, MF* b)
{
	float mu = MAX_MF;
	float h = 0.01;
	float alpha = 1;

	
	while(mu > 0)
	{
		std::vector<float> v1 = a->getvx(mu);
		std::vector<float> v2 = b->getvx(mu);
		
		for(unsigned i = 0; i < v1.size(); i++)
		{
			for(unsigned j = 0; j < v2.size(); j++)
			{
				if((v1[i] - v2[j] > -0.1) && (v1[i] - v2[j] < 0.1))
				{
					//printf("[%i:%i] %f: %f, %f\n", i, j, mu, v1[i], v2[j]);
					//printf("intersecting: %f, %f\n", v1[i], v2[j]);
					alpha = -(v1[i] - v2[j]);
					//printf("alpha: %f\n", alpha);
					if(alpha == 0) return v1[i];
				}
				//printf("[%i:%i]%f: %f, %f\n", i, j, mu, v1[i], v2[j]);
			}
		}
		mu -= h * alpha;
	}
	
	return -1.0;
}

int FIC::get_id_output_mf(int id_input_var, int id_input_mf)
{
	for(int i = 0; i < rule_collumn; i++)
	{
		if(fic_rule[i][id_input_var] == id_input_mf)
		{
			return fic_rule[i][input_var.size()];
		}
	}
	return -1;
}

std::pair<int, float> FIC::get_id_maxv(std::map<int, float> map_table_value_mf)
{
	int id_max_el = 0;
	float max_value = 0;
	for(auto it = map_table_value_mf.begin(); it != map_table_value_mf.end(); ++it)
	{
		printf("it->second: %i, %f\n", it->first, it->second);
		if(max_value < it->second)
		{
			max_value = it->second;
			id_max_el = it->first;
		}
	}
	return std::make_pair(id_max_el, max_value);
}

float FIC::fuzzification(float value)
{
	std::map<int, float> map_table_value_mf;
	Fuzzyplot fp;
	fp.set_multiplot();
	
	for(auto it = input_var.begin(); it != input_var.end(); ++it)
	{
		std::map<int, MF*>* pmapmf = it->second;
		for(auto itmf = pmapmf->begin(); itmf != pmapmf->end(); ++itmf)
		{
			MF* mf = itmf->second;
			//mf->set_truncation_h(0.5);
			map_table_value_mf.insert(std::pair<int, float>(itmf->first, mf->get_fuzzynum(value)));
			printf("[fuzzification] get mf value: %f\n", mf->get_fuzzynum(value));
			//printf("[fuzzification] get mf integral: %f\n", mf->integral(0.5));
			printf("\n");

			std::vector<std::pair<double, double>> xy_pts_A;
			for(double x = 0; x < 30; x += 0.01) 
			{	
				double y = mf->get_fuzzynum(x);
				xy_pts_A.push_back(std::make_pair(x, y));
			}
			
			fp.plot(xy_pts_A);
		}		
	}
	fp.unset_multiplot();
	
	std::pair<int, float> pair_out = get_id_maxv(map_table_value_mf);
	int id_out = get_id_output_mf(0, pair_out.first);
	printf("[fuzzification] !! %i %i \n", pair_out.first, id_out);
	auto itoutv = output_var.find(1);
	auto itou = (itoutv->second)->find(id_out);
	MF *mfout = itou->second;
	mfout->set_truncation_h(pair_out.second);
	
	Fuzzyplot fp2;
	std::vector<std::pair<double, double>> xy_pts_A;
	for(double x = 0; x < 30; x += 0.01) 
	{	
		double y = mfout->get_fuzzynum(x);
		xy_pts_A.push_back(std::make_pair(x, y));
	}
	
	fp2.plot(xy_pts_A);
	
	
	
	/*
	auto itmf = pmapmf->begin();
	for(;;)
	{
		MF* mf = itmf->second;
		if((++itmf) == pmapmf->end()) break;
		float is = intersecting_mf(mf, itmf->second);
		printf("[fuzzification] intersecting point: %f \n", is);
	}*/
	
	return 0.0;
}
