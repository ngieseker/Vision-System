/* Brief file to hold all function declarations */
/* Possibly a repeat of comm.h? */

#ifndef FUNC
#define FUNC

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

/* Multiple object tracking */
void getObjects(std::vector<Marker> &markers, std::vector<double> &objs, int &obj_id, int markernum);
bool sortByX(const Marker &lhs, const Marker &rhs);
bool inDistanceThreshold(Marker A, Marker B);

/* Benchmarking */
double elapsed(struct timespec *start, struct timespec *end);

