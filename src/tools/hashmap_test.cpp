#define __STDC_LIMIT_MACROS

#include <cstdio>

#include "Timer.hpp"
#include "HashMap.hpp"

#include "hash.h"
#include <functional>
#include <random>

#include <unordered_map>


struct hashy {
	size_t operator()(const size_t &k) const {
		//return std::_Hash_bytes(&k, sizeof(size_t), 1056323);
		return k;
	}
};


int main() {
	const size_t runs = 1000000;

	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<size_t> dist(0, UINT64_MAX);

	uint64_t *data = (uint64_t *)malloc(sizeof(uint64_t)*runs);
	for (size_t i = 0; i < runs; i++)
		data[i] = dist(gen);

	printf("Random data:\n");

	printf("Insertion:\n");
	for (size_t c = 0; c < 3; c++) {
		HashMap<size_t, size_t> hm2;

		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		for (size_t i = 0; i < runs; i+=1)
			hm2[data[i]] = i;

		t.end();

		printf(" HM: %15.10fms total, %15.10fns per insertion\n", t.elapsed()*1000, t.elapsed()*1e9/runs);
	}
	printf("\n");

	for (size_t c = 0; c < 3; c++) {
		std::unordered_map<size_t, size_t> um2;

		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		for (size_t i = 0; i < runs; i+=1)
			um2[data[i]] = i;

		t.end();

		printf(" UM: %15.10fms total, %15.10fns per insertion\n", t.elapsed()*1000, t.elapsed()*1e9/runs);
	}
	printf("\n\n");


	printf("Retrieval:\n");
	for (size_t c = 0; c < 3; c++) {
		HashMap<size_t, size_t> hm2;

		for (size_t i = 0; i < runs; i+=1)
			hm2[data[i]] = i;

		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		volatile size_t v = 0;
		for (size_t i = 0; i < runs; i+=1) {
			v = hm2[data[i]];
			if (v != i)
				printf("Error @ %15lu: expected %15lu, got %15lu\n", data[i], i, v);
		}

		t.end();

		printf(" HM: %15.10fms total, %15.10fns per access\n", t.elapsed()*1000, t.elapsed()*1e9/runs);
	}
	printf("\n");

	for (size_t c = 0; c < 3; c++) {
		std::unordered_map<size_t, size_t> um2;

		for (size_t i = 0; i < runs; i+=1)
			um2[data[i]] = i;

		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		volatile size_t v = 0;
		for (size_t i = 0; i < runs; i+=1) {
			v = um2[data[i]];
			if (v != i)
				printf("Error @ %15lu: expected %15lu, got %15lu\n", data[i], i, v);
		}

		t.end();

		printf(" UM: %15.10fms total, %15.10fns per access\n", t.elapsed()*1000, t.elapsed()*1e9/runs);
	}
	printf("\n\n");



	for (size_t i = 0; i < runs; i++)
		data[i] = i;

	printf("Sequential data:\n");

	printf("Insertion:\n");
	for (size_t c = 0; c < 3; c++) {
		HashMap<size_t, size_t> hm2;

		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		for (size_t i = 0; i < runs; i+=1)
			hm2[data[i]] = i;

		t.end();

		printf(" HM: %15.10fms total, %15.10fns per insertion\n", t.elapsed()*1000, t.elapsed()*1e9/runs);
	}
	printf("\n");

	for (size_t c = 0; c < 3; c++) {
		std::unordered_map<size_t, size_t> um2;

		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		for (size_t i = 0; i < runs; i+=1)
			um2[data[i]] = i;

		t.end();

		printf(" UM: %15.10fms total, %15.10fns per insertion\n", t.elapsed()*1000, t.elapsed()*1e9/runs);
	}
	printf("\n\n");


	printf("Retrieval:\n");
	for (size_t c = 0; c < 3; c++) {
		HashMap<size_t, size_t> hm2;

		for (size_t i = 0; i < runs; i+=1)
			hm2[data[i]] = i;
		//hm2.print();

		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		volatile size_t v = 0;
		for (size_t i = 0; i < runs; i+=1) {
			v = hm2[data[i]];
			if (v != i)
				printf("Error @ %15lu: expected %15lu, got %15lu\n", data[i], i, v);
		}

		t.end();

		printf(" HM: %15.10fms total, %15.10fns per access\n", t.elapsed()*1000, t.elapsed()*1e9/runs);
	}
	printf("\n");

	for (size_t c = 0; c < 3; c++) {
		std::unordered_map<size_t, size_t> um2;

		for (size_t i = 0; i < runs; i+=1)
			um2[data[i]] = i;

		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		volatile size_t v = 0;
		for (size_t i = 0; i < runs; i+=1) {
			v = um2[data[i]];
			if (v != i)
				printf("Error @ %15lu: expected %15lu, got %15lu\n", data[i], i, v);
		}

		t.end();

		printf(" UM: %15.10fms total, %15.10fns per access\n", t.elapsed()*1000, t.elapsed()*1e9/runs);
	}
	printf("\n\n");


	return 0;
}
