#ifndef FUZZY_PLOT_H
#define FUZZY_PLOT_H
#include <vector>
#include <string>

class Fuzzyplot
{
public:
	Fuzzyplot();
	~Fuzzyplot();
	void plot(std::vector<std::pair<double, double>> xy_pts_A);
	void plot(float *x, float *y, int len);
	void plotv(float value, std::string linename);
	void plotv(float value);
	void set_multiplot(){fprintf(gp, "set multiplot\n");};
	void unset_multiplot(){fprintf(gp, "unset multiplot\n");};
	void set_xrange(int xmin, int xmax){fprintf(gp,"set xrange [%i:%i]\n", xmin, xmax);};

private:
	FILE *gp;
};

#endif
