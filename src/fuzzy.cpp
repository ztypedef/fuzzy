#include <stdio.h>
#include <string.h>
#include <cmath>
#include <algorithm> 
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
	//printf("[trimf::get_fuzzynum] mu: %f, %f\n", v, mu);
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
	mf_allmax = NULL;
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
	
	MEMCHECK.rm_ptr(mf_allmax);
	delete[] mf_allmax;
}

int FIC::addvar(std::string varname, var_t type)
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
			input_name_var.insert(std::pair<int, std::string>(id, varname));
			return id;
		}
		case OUTPUT:
		{
			std::map<int, std::map<int, MF*>*>::iterator it = output_var.end();
			int id = it->first + 1;
			it = output_var.begin();
			output_var.insert(it, std::pair<int, std::map<int, MF*>*>(id, pv));
			output_name_var.insert(std::pair<int, std::string>(id, varname));
			return id;
		}
		default:
			printf("[FIC] delete vector pointer: %p\n", pv);
			delete pv;
			break;
	}
	return -1;
}

int FIC::addmf(var_t var_type, int var_index, std::string namemf, mf_t mf_type, float* mf_params)
{
	
	switch(mf_type)
	{
		case TRIMF: return addmf_tri(var_index, mf_params, var_type, namemf);
		default: return -1;
	}
}

int FIC::_addmf(int idvar, MF *v, var_t type)
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

int FIC::addmf_tri(int idvar, float *x, var_t type, std::string name_mf)
{
	if (x[0] > x[1]) throw "var 1 > var 2";
	if (x[1] > x[2]) throw "var 2 > var 3";
	trimf *tri = new trimf(x);
	tri->set_name(name_mf);
	MEMCHECK.add_ptr(tri, "[FIC::addmf_tri] tri");
	return _addmf(idvar, (MF*)tri, type);
}

void FIC::set_xrange(float x_min, float x_max, float step_x)
{
	xmin = x_min;
	xmax = x_max;
	step = step_x;
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

void FIC::rmvar(var_t var_type, int var_index)
{
	std::map<int, std::map<int, MF*>*>* var;
	std::map<int, std::map<int, MF*>*>::iterator it;
	std::map<int, MF*>* mapmf;
	switch(var_type)
	{
		case OUTPUT: 
			var = &output_var;
			output_name_var.erase(var_index);
			break;
		case INPUT:  
			var = &input_var; 
			input_name_var.erase(var_index);
			break;
		default: break;
	}
	it = var->find(var_index);
	mapmf = it->second;
	for(auto itm = mapmf->begin(); itm != mapmf->end(); ++itm)
	{
		auto pmf = itm->second;
		MEMCHECK.rm_ptr(pmf);
		delete pmf;
		mapmf->erase(itm);
	}
	MEMCHECK.rm_ptr(mapmf);
	delete mapmf;
	var->erase(it);
}

void FIC::rmmf(var_t var_type, int var_index, int mf_index)
{
	std::map<int, std::map<int, MF*>*>::iterator it;
	std::map<int, MF*>::iterator itm;
	switch(var_type)
	{
		case OUTPUT:
			it = output_var.find(var_index);
			MEMCHECK.rm_ptr(it->second->find(mf_index)->second);
			itm = it->second->find(mf_index);
			delete itm->second;
			it->second->erase(itm);
			break;
		case INPUT:
			it = input_var.find(var_index);
			MEMCHECK.rm_ptr(it->second->find(mf_index)->second);
			itm = it->second->find(mf_index);
			delete itm->second;
			it->second->erase(itm);
			break;
		default:
			break;
	}
}

void FIC::getfis()
{
	printf("[FIC::getfis]\n");
	printf("amount input  var: %i\n", input_var.size());
	printf("amount output var: %i\n", output_var.size());
	
	
	for(auto it = input_var.begin(); it != input_var.end(); it++)
		printf("input  var[%i] size: %i\n", it->first, it->second->size());
		
	for(auto it = output_var.begin(); it != output_var.end(); it++)
		printf("output var[%i] size: %i\n", it->first, it->second->size());
	
}

std::string FIC::get_name_mf(var_t var_type, int var_index, int mf_index)
{
	std::map<int, std::map<int, MF*>*>* var;
	switch(var_type)
	{
		case OUTPUT: var = &output_var; break;
		case INPUT:  var = &input_var;  break;
		default: break;
	}
	auto mf = var->find(var_index)->second;
	return mf->find(mf_index)->second->get_name();
}

void FIC::showrule()
{
	int *sr = new int[rule_collumn];
	for(int i = 0; i < rule_collumn; i++)
		sr[i] = i + 1;
	showrule(sr, rule_collumn);
}

void FIC::showrule(int* index_list, int len)
{
	for(int j = 0; j < len; j++)
	{
		int clmn = index_list[j] - 1;
		printf("%i. If ", clmn + 1);
		for(unsigned i = 0; (int)i < rule_row - 2; i++)
		{
			if(i < input_var.size())
			{
				if(!fic_rule[clmn][i]) continue;
				printf("(%s ", input_name_var.find(i + 1)->second.c_str());
				printf("is %s) ", get_name_mf(INPUT, i + 1, fic_rule[clmn][i]).c_str());
				if(i != input_var.size() - 1)
				{
					if (fic_rule[clmn][rule_row - 1]) printf("or  ");
					else printf("and ");
				}
				else
				{
					printf("then ");
				}
			}
			else
			{
				int idout = i + 1 - input_var.size();
				printf("(%s ", output_name_var.find(idout)->second.c_str());
				printf("is %s)\n", get_name_mf(OUTPUT, idout, fic_rule[clmn][i]).c_str());
			}
		}
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
	for(auto it = map_table_value_mf.begin(); it != map_table_value_mf.end(); it++)
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

float FIC::fuzzification(float* value)
{
	std::vector<float> vm;
	std::map<int, float> map_table_value_mf;
	for(int i = 0; i < rule_collumn; i++)
	{
		vm.clear();
		for(unsigned num_var = 0; num_var < input_var.size(); num_var++)
		{
			int id_in = fic_rule[i][num_var];
			if(!id_in) continue;
			//printf("[FIC::fuzzification] get id in: %i\n", id_in);
			auto it_iv = input_var.find(num_var + 1);
			MF *mf = ((it_iv->second)->find(id_in))->second;
			float fn = mf->get_fuzzynum(value[num_var]);
			vm.push_back(fn);
			//printf("[FIC::fuzzification] var: %i, id: %i, mu: %f\n", num_var + 1, id_in, fn);
		}
		
		float divide;
		if(fic_rule[i][rule_row - 1])
			divide = *std::min_element(vm.begin(), vm.end());
		else
			divide = *std::max_element(vm.begin(), vm.end());
		//printf("[FIC::fuzzification] rule: %i, insert: %i, %f\n", 
		//	fic_rule[i][rule_row - 1], fic_rule[i][input_var.size()], divide);
			
		map_table_value_mf.insert(std::pair<int, float>
			(fic_rule[i][input_var.size()], divide));
	}
	truncation(1, map_table_value_mf);
	return 0.0;
}

float FIC::truncation(int id_out_var, std::map<int, float> map_table_value_mf)
{
	auto it_outv = output_var.find(id_out_var); //TODO hardcode
	for(auto it = map_table_value_mf.begin(); it != map_table_value_mf.end(); ++it)
	{
		int id_out = it->first;
		MF *mf = ((it_outv->second)->find(id_out))->second;
		mf->set_truncation_h(it->second);
	}	
	return 0;
}

float FIC::composition()
{
	float h = step;
	float xvarmin = xmin, xvarmax = xmax;
	int len_a = (int)((xvarmax - xvarmin) / h) + 1;
	if(mf_allmax != NULL)
	{
		MEMCHECK.rm_ptr(mf_allmax);
		delete[] mf_allmax;
	}
	mf_allmax = new float[len_a];
	MEMCHECK.add_ptr(mf_allmax, "mf_allmax");
	float *xmf = new float[len_a];
	MEMCHECK.add_ptr(xmf, "xmf");
	memset(mf_allmax, 0, sizeof(float) * len_a);
	
	auto outv = output_var.find(1)->second; //TODO hardcode
	
	for(auto it = outv->begin(); it != outv->end(); ++it)
	{
		MF *mf = it->second;
		for(int i = 0; i < len_a; i++)
		{
			xmf[i] = (float)i * h + xvarmin; //TODO kostil
			float current_mf = mf->get_fuzzynum((float)i * h + xvarmin);
			if(mf_allmax[i] < current_mf)
			{
				mf_allmax[i] = current_mf;
			}
		}
	}
	/*
	Fuzzyplot fp;
	fp.set_multiplot();
	fp.set_xrange(xmin, xmax);
	fp.plot(xmf, mf_allmax, len_a);
	fp.plotv(defuzzification());*/
	
	MEMCHECK.rm_ptr(xmf);
	delete[] xmf;
	return 0.0;
}

float FIC::defuzzification()
{
	int len_a = (int)((xmax - xmin) / step) + 1;
	float sumy = 0;
	float sumxy = 0;
	for(int i = 0; i < len_a; i++)
	{
		sumy += mf_allmax[i];
		sumxy += ((float)i * step + xmin) * mf_allmax[i];
	}
	//printf("[FIC::defuzzification] %f\n", sumxy / sumy);
	return sumxy / sumy;
}

void FIC::evalfis(float *value, float* out_value)
{
	fuzzification(value);
	composition();
	out_value[0] = defuzzification();
}

void FIC::gensurf()
{
	float _h = 1;
	int len_a = (int) 30 / _h;
	float value_inp[2];
	float **value_xy = new float*[len_a + 1];
	for(int i = 0; i < len_a + 1; i++)
		value_xy[i] = new float[len_a + 1];
	value_xy[0][0] = len_a;
	
	for(int i = 0; i < len_a; i++)
	{
		for(int j = 0; j < len_a; j++)
		{
			value_inp[0] = j * _h;
			value_inp[1] = i * _h;
			value_xy[0][j + 1] = value_inp[0];
			value_xy[i + 1][0] = value_inp[1];
			fuzzification(value_inp);
			composition();
			
			value_xy[i + 1][j + 1] = defuzzification();
		}
		//printf("=");
		fflush(stdout);
	}
	value_xy[1][1] = 0;
	//printf("\n");
	printf("end\n");

	Fuzzyplot fp;
	fp.set_xrange(xmin, xmax);
	fp.set_yrange(xmin, xmax);
	fp.set_zrange(xmin, xmax);
	fp.plot3d(value_xy, len_a);

	for(int i = 0; i < len_a + 1; i++)
		delete [] value_xy[i];
	delete [] value_xy;
}

void FIC::plotmf(var_t var_type, int var_index)
{
	std::map<int, fuzzy::MF*>* mapmf;
	std::map<int, std::map<int, MF*>*>::iterator itvar;
	switch(var_type)
	{
		case OUTPUT:
			itvar = output_var.find(var_index);
			if(itvar == output_var.end()) return;
			mapmf = itvar->second;
			break;
		case INPUT:
			itvar = input_var.find(var_index);
			if(itvar == input_var.end()) return;
			mapmf = itvar->second;
			break;
		default:
			break;
	}
	Fuzzyplot fp;
	fp.set_xrange(xmin, xmax);
	fp.set_yrange(0, 1);
	fp.set_multiplot();
	
	for(auto it = mapmf->begin(); it != mapmf->end(); ++it)
	{
		std::vector<std::pair<float, float>> xy_pts_A;
		xy_pts_A.clear();
		it->second->set_truncation_h(1.0);
		for(float x = 0; x < 30; x += 0.01)
		{
			float y = it->second->get_fuzzynum(x);
			xy_pts_A.push_back(std::make_pair(x, y));
		}
		fp.plot(xy_pts_A);
	}
}
