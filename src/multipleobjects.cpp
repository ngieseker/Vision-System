/*
	File containing all functions for multiple object tracking
	3 July 2015
*/
#include "func.h"
	
void getObjects(std::vector<Marker> &markers, std::vector<Marker> &objs, int &obj_id, unsigned markernum)
{
	int i, j, k;
	int sz = markers.size();
	
	Marker P1 = markers[0];
	for (i = 1; i < sz; i++)
	{
		Marker P2 = markers[i];
		if (inDistanceThreshold(P1,P2))
		{
			for (j = i; j < sz; j++)
			{
				Marker P3 = markers[j];
				if (inDistanceThreshold(P2,P3))
				{
					// After first three markers are tracked, the fourth will be in the -x direction from marker 3
					for (k = j; k > 0; k--)
					{
						Marker P4 = markers[k];
						if (inDistanceThreshold(P3,P4) & inDistanceThreshold(P4,P1))
						{
							
							#ifdef DEBUG
							std::cout<<"For Object # "<<obj_id<<": marker 1: "<<P1.x<< " marker 2: "<<P2.x<< " marker 3: "<<P3.x<<" marker 4: "<<P4.x<<std::endl;
							#endif

							objs.push_back(P1);
							objs.push_back(P2);
							objs.push_back(P3);
							objs.push_back(P4);

							// Skip objects that have already been identified in subsequent operations
							markers[0].x = -1 * markers[0].x;
							markers[i].x = -1 * markers[i].x;
							markers[j].x = -1 * markers[j].x;
							markers[k].x = -1 * markers[k].x;

							obj_id++;

							return;
						}
					}
				}
			} 
		}
	}
	if (objs.size() < markernum)
	{
		std::cout<<"Error detecting multiple objects"<<std::endl;
		return;
	}
}

bool sortByX(const Marker &lhs, const Marker &rhs)
{
		return lhs.x > rhs.x;
}

bool inDistanceThreshold(Marker A, Marker B)
{
	// TODO: Move these constants into constants header file?
  // possibly make this into a functor to store/update cosntants
	double dist_max = 2;
	double dist_min = 1;
	double x = A.x - B. x;
	double y = A.y - B.y;
	double distance = (x*x + y*y); // SQRT is too time intensive

	if (distance > dist_min and distance < dist_max) {return true;}
	else {return false;}
}
