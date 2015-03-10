#include "common.h"


double elapsed(struct timespec *start, struct timespec *end) {
	return (double)end->tv_sec + 1e-9*(double)end->tv_nsec - (double)start->tv_sec - 1e-9*(double)start->tv_nsec;
}

