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
	pclose(gp);
}

void Fuzzyplot::plot(std::vector<std::pair<double, double>> xy_pts_A) 
{
	
    fprintf(gp, "plot '-' with lines\n");
	for (auto it = xy_pts_A.begin() ; it != xy_pts_A.end(); ++it)
		fprintf(gp, "%f %f\n", it->first, it->second);
	fprintf(gp, "%s\n", "e");
}
