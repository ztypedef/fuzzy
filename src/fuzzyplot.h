#ifndef FUZZY_PLOT_H
#define FUZZY_PLOT_H
#include <vector>

class Fuzzyplot
{
public:
	Fuzzyplot();
	~Fuzzyplot();
	void plot(std::vector<std::pair<double, double>> xy_pts_A);
	void set_multiplot(){fprintf(gp, "set multiplot\n");};
	void unset_multiplot(){fprintf(gp, "unset multiplot\n");};

private:
	FILE *gp;
};

#endif
