/*
	File containing all functions for multiple object tracking
	3 July 2015
*/
#include "func.h"
#include "fxbcam.h"
#include "globals.h"
	
void FXBcam::getObjects(std::vector<Marker> markers, std::vector<Object> &objs, int &obj_id, unsigned markernum)
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
							std::cout<<"For Object # "<<obj_id<<":"<<std::endl<<"marker 1: "<<P1.x<< " marker 2: "<<P2.x<< " marker 3: "<<P3.x<<" marker 4: "<<P4.x<<std::endl;
							#endif

							Object obj = Object(P1,P2,P3,P4,obj_id);
							objs.push_back(obj);

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

bool FXBcam::sortByX(const Marker &lhs, const Marker &rhs)
{
		return lhs.x > rhs.x;
}

bool FXBcam::inDistanceThreshold(Marker A, Marker B)
{
  // possibly make this into a functor to store/update cosntants

	double x = A.x - B. x;
	double y = A.y - B.y;
	double distance = (x*x + y*y); // SQRT is too time intensive

	if (distance > dist_min and distance < dist_max) {return true;}
	else {return false;}
}
