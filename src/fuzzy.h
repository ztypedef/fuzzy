#ifndef FUZZY_H
#define FUZZY_H
#include <string>
#include <map>

namespace fuzzy
{

	typedef struct fnumber
	{
		float value;
		float mu;
	}fnumber;
		
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
		~FIC();
		int addvar(std::string mfname); //return id mf
		int addmf(int idvar, Var *v); //return id var
		int addmf_tri(int idvar, float *x);
		int addrule();
		float genval(float p);
	private:
		fnumber fuzzification(float value);
		float defuzzification(fnumber value);
		
		fnumber fz_and(fnumber v1, fnumber v2);
		fnumber fz_or(fnumber v1, fnumber v2);

		std::map<int, Var*> var;
		std::map<int, std::map<int, Var*>*> mfmap;
	};
}
#endif
