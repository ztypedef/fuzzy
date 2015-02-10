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
		virtual float get_fuzzynum(float v){return 0;};
		virtual float integral(float l, float r, float h){return 0;};
		virtual float integral(float h){return 0;};
		virtual std::vector<float> getvx(float mu){std::vector<float> vi; return vi;};
		virtual void set_truncation_h(float h){trc_h = h;};
	protected:
		virtual void getx(float *x, float* n, float mu){};
		float *pvar;
		int len;
		float trc_h = 1;
	};
	
	class trimf : public MF
	{
	public:
		trimf(float *v);
		~trimf();
		float get_fuzzynum(float v);
		float integral(float l, float r, float h);
		float integral(float h);
		std::vector<float> getvx(float mu);
	private:
		void getx(float *x, float* n, float mu);
		float integral_left(float l, float r);
		float integral_right(float l, float r);
	};

	class FIC
	{
	public:
		FIC();
		~FIC();
		//int addvar(std::string mfname); //return id mf
		int addvar(std::string mfname, var_t type);
		int addmf(int idvar, MF *v, var_t type); //return id var
		int addmf_tri(int idvar, float *x, var_t type);
		void addrule(int rule[], int collumn, int row);
		float genval(float p);
		float intersecting_mf(MF* a, MF* b);
	//private:
		MF* get_input_MF(int idvar, int idmf);
		MF* get_output_MF(int idvar, int idmf);
		float fuzzification(float value);
		float defuzzification(float value);
		
		float fz_and(float v1, float v2);
		float fz_or(float v1, float v2);
	
	private:
		int get_id_output_mf(int input_var, int id_input_mf);
		std::pair<int, float> get_id_maxv(std::map<int, float> map_table_value_mf);
		int **fic_rule;
		int rule_collumn, rule_row;
		std::map<int, MF*> var;
		std::map<int, std::map<int, MF*>*> input_var;
		std::map<int, std::map<int, MF*>*> output_var;
	};
}
#endif
