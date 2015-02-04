#ifndef FUZZY_H
#define FUZZY_H
#include <string>
#include <map>
#include <vector>

namespace fuzzy
{
	typedef enum var_t
	{
		INPUT  = 0x0,
		OUTPUT = 0x1,
	}var_t;
		
	class MF
	{
	public:
		MF(float *v, int len);
		~MF();
		virtual float get_mmbrsp(float v){return 0;};
	protected:
		float *pvar;
		int len;
	};
	
	class trimf : public MF
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
		//int addvar(std::string mfname); //return id mf
		int addvar(std::string mfname, var_t type);
		int addmf(int idvar, MF *v); //return id var
		int addmf_tri(int idvar, float *x);
		void addrule(int rule[], int collumn, int row);
		float genval(float p);
	//private:
		float fuzzification(float value);
		float defuzzification(float value);
		
		float fz_and(float v1, float v2);
		float fz_or(float v1, float v2);
	private:
		int **fic_rule;
		int rule_collumn, rule_row;
		std::map<int, MF*> var;
		std::map<int, std::map<int, MF*>*> input_var;
		std::map<int, std::map<int, MF*>*> output_var;
	};
}
#endif
