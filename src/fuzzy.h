#ifndef FUZZY_H
#define FUZZY_H
#include <string>
#include <map>
#include <vector>

namespace fuzzy
{
		
	class Var
	{
	public:
		Var(float *v, int len);
		~Var();
		virtual float get_mmbrsp(float v){return 0;};
	protected:
		float *pvar;
		int len;
	};
	
	class trimf : public Var
	{
	public:
		trimf(float *v);
		~trimf();
		float get_mmbrsp(float v);
	};

	class FIC
	{
	public:
		FIC();
		~FIC();
		int addvar(std::string mfname); //return id mf
		int addmf(int idvar, Var *v); //return id var
		int addmf_tri(int idvar, float *x);
		void addrule(int id_in_mf, int id_out_mf);
		float genval(float p);
	//private:
		float fuzzification(float value);
		float defuzzification(float value);
		
		float fz_and(float v1, float v2);
		float fz_or(float v1, float v2);
	private:
		std::vector<int*> rule;
		std::map<int, Var*> var;
		std::map<int, std::map<int, Var*>*> mfmap;
	};
}
#endif
