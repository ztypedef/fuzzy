#include <stdio.h>
#include <stdlib.h>

#include "fuzzyplot.h"

Fuzzyplot::Fuzzyplot()
{
	gp = popen("gnuplot -persist","w");
	if (gp == NULL)
    {
        printf("Error opening pipe to GNU plot.\n");
        exit(0);
    }
    fprintf(gp,"set yrange [0:1]\n"); 
}

Fuzzyplot::~Fuzzyplot()
{
	fprintf(gp, "q\n");
	pclose(gp);
}

void Fuzzyplot::plot(std::vector<std::pair<float, float>> xy_pts_A) 
{
	
    fprintf(gp, "plot '-' with lines\n");
	for (auto it = xy_pts_A.begin() ; it != xy_pts_A.end(); ++it)
		fprintf(gp, "%f %f\n", it->first, it->second);
	fprintf(gp, "%s\n", "e");
}

void Fuzzyplot::plot(float *x, float *y, int len)
{
	fprintf(gp, "plot '-' with lines\n");
	for(int i = 0; i < len; i++)
		fprintf(gp, "%f %f\n", x[i], y[i]);
	fprintf(gp, "%s\n", "e");
}

void Fuzzyplot::plot3d(float **xy, int len)
{
	/*
	//fprintf(gp, "set dgrid3d 100,100\n");
	fprintf(gp, "set grid\n");
	fprintf(gp, "set contour\n");
	//fprintf(gp, "set hidden3d\n");
	fprintf(gp, "splot '-'  \n");
	for(int i = 0; i < len; i++)
		fprintf(gp, "%f %f %f\n", x[i], y[i], z[i]);*/
		/*
	for(int i = 0; i < len; i++) fprintf(gp, "%f", x[i]);
	fprintf(gp, "\n");
	for(int i = 0; i < len; i++) fprintf(gp, "%f", y[i]);
	fprintf(gp, "\n");
	for(int i = 0; i < len; i++) fprintf(gp, "%f", z[i]);
	fprintf(gp, "\n");*/
	/*
	fprintf(gp, "%s\n", "e");
	fprintf(gp, "%s\n", "e");
	*/
	fprintf(gp, "set contour\n");
	fprintf(gp, "set grid\n");
	fprintf(gp, "set hidden3d\n");
	int rows = len, cols = len;
	fprintf(gp, "splot '-' nonuniform matrix with lines notitle\n" );
	for(int i = 0; i < rows; i++)
	{
		for(int j = 0; j < cols; j++)
		{
			fprintf(gp, "%f ", xy[i][j]);
		}
		fprintf(gp, "\n");
	}
	fprintf(gp, "e\n" );
	fprintf(gp, "e\n" );
	fflush(gp);
}

void Fuzzyplot::plotv(float value, std::string linename) 
{
    fprintf(gp, "plot '' tit '%s' with lines lt rgb \"#FF00FF\"\n", linename.c_str());
	fprintf(gp, "%f %f \n", value, 0.0f);
	fprintf(gp, "%f %f \n", value, 1.0f);
	fprintf(gp, "%s\n", "e");
}

void Fuzzyplot::plotv(float value) 
{
    fprintf(gp, "plot '' tit '-' with lines lt rgb \"#FF00FF\"\n");
	fprintf(gp, "%f %f \n", value, 0.0f);
	fprintf(gp, "%f %f \n", value, 1.0f);
	fprintf(gp, "%s\n", "e");
}
