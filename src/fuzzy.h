#ifndef FUZZY_H
#define FUZZY_H
#include <string>
#include <map>
#include <vector>

namespace fuzzy
{

	typedef enum mftype
	{
		TRN = 0x0,
	}mftype;

	typedef struct fpoint
	{
		float value;
		float mu;
	}fpoint;
	
	class Var
	{
	public:
		Var(mftype mf_type, float *v);
		~Var();
	private:
		float *pvar;
		mftype mft;	
	};

	class FIC
	{
	public:
		~FIC();
		int addmf(std::string mfname); //return id mf
		int addvar(int idmf, Var *v); //return id var
		int addrule();
		float genval(float p);
	private:
		fpoint fuzzification(float value);
		float defuzzification(fpoint value);
		
		fpoint fz_and(fpoint v1, fpoint v2);
		fpoint fz_or(fpoint v1, fpoint v2);

		std::vector<Var> var;
		std::map<int, std::vector<Var>*> mfmap;
	};
}
#endif
