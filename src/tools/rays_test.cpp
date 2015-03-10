#include <stdio.h>
#include <iostream>
#include <vector>
#include "fxbcam.h"
#include "Timer.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cerr << "USAGE: ./ray_test time" << endl;
		exit(1);
	}
	
	int time = atol(argv[1]);

	while (1) {
		Timer t;
		t.start();
		int i = 0;
		while (t.timePassed()*1000 < time) {
			vector< Ray > rays;
			for (int j = 0; j < 8; j++) {
				Ray a(1, rand(), rand(), rand(), 1);
				a.x = rand();
				a.y = rand();
				a.z = rand();

				rays.push_back(a);
			}
			FXBcam vis;

			vis.intersectRays(rays);
			i++;
		}
		t.end();
		printf("Time Each: %15.10lfms Total: %15.10lfms NumCalc'ed: %10d\n", (t.elapsed()/(double)i)*1000, t.elapsed()*1000, i);
	}
	return 0;
}
