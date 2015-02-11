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
	void plot3d(float *x, float *y, float *z, int len);
	void plotv(float value, std::string linename);
	void plotv(float value);
	void set_multiplot(){fprintf(gp, "set multiplot\n");};
	void unset_multiplot(){fprintf(gp, "unset multiplot\n");};
	void set_xrange(int xmin, int xmax){fprintf(gp,"set xrange [%i:%i]\n", xmin, xmax);};
	void set_yrange(int ymin, int ymax){fprintf(gp,"set yrange [%i:%i]\n", ymin, ymax);};
	void set_zrange(int zmin, int zmax){fprintf(gp,"set zrange [%i:%i]\n", zmin, zmax);};

private:
	FILE *gp;
};

#endif
