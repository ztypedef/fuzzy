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
	
	typedef enum mf_t
	{
		TRIMF  = 0x1,
	}mf_t;
		
	class MF
	{
	public:
		MF(float *v, int len);
		virtual ~MF();
		virtual float get_fuzzynum(float v){return 0;};
		virtual float integral(float l, float r, float h){return 0;};
		virtual float integral(float h){return 0;};
		virtual std::vector<float> getvx(float mu){std::vector<float> vi; return vi;};
		virtual void set_truncation_h(float h){trc_h = h;};
		std::string get_name(){return namemf;};
		void set_name(std::string n_mf){namemf = n_mf;};
	protected:
		virtual void getx(float *x, float* n, float mu){};
		float *pvar;
		int len;
		float trc_h = 1;
		std::string namemf;
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
		
		/**********************MATLAB STYLE*************************/
		void getfis();
		void showrule();
		void showrule(int* index_list, int len);
		void showrule(int* index_list, int len, int format); //don't work
	
		int addvar(std::string varname, var_t type);		
		int addmf(var_t var_type, int var_index, std::string namemf, mf_t mf_type, float* mf_params);	
		void addrule(int rule[], int collumn, int row);

		void rmvar(var_t var_type, int var_index);
		void rmmf(var_t var_type, int var_index, int mf_index);
		
		void gensurf();
		void plotmf(var_t var_type, int var_index);
		
		void evalfis(float *value, float* out_value);
		
		/***********************************************/
		int _addmf(int idvar, MF *v, var_t type); //return id var
		int addmf_tri(int idvar, float *x, var_t type, std::string name_mf);	
		void set_xrange(float x_min, float x_max, float step_x);
		
	private:
		MF* get_input_MF(int idvar, int idmf);
		MF* get_output_MF(int idvar, int idmf);
		float fuzzification(float* value);
		float truncation(int id_out_var, std::map<int, float> map_table_value_mf);
		float composition();
		float defuzzification();
		/*********************************/
		float intersecting_mf(MF* a, MF* b);
	private:
		std::string get_name_mf(var_t var_type, int var_index, int mf_index);
		int get_id_output_mf(int input_var, int id_input_mf);
		std::pair<int, float> get_id_maxv(std::map<int, float> map_table_value_mf);
		int **fic_rule;
		int rule_collumn, rule_row;
		float * mf_allmax;
		float xmin = 0, xmax = 20, step = 0.01;
		std::map<int, MF*> var;
		std::map<int, std::map<int, MF*>*> input_var;
		std::map<int, std::map<int, MF*>*> output_var;
		
		std::map<int, std::string> input_name_var;
		std::map<int, std::string> output_name_var;
	};
}
#endif
