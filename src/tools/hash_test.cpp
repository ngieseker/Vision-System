#define __STDC_LIMIT_MACROS

#include "Timer.hpp"

#include <cstdio>
#include <stdint.h>

#include "hash.h"
#include <functional>
#include <random>


// A simple hash function (but not SimpleHash)
struct simplehash {
	size_t operator()(const size_t& k) {
		size_t n = k + (k << 16);
		n ^= n >> 13;
		n += n << 4;
		n ^= n >> 7;
		n += n << 10;
		n ^= n >> 5;
		n += n << 8;
		n ^= n >> 16;
		return n;
	}
};

struct fnv1a {
	size_t operator()(const size_t& k) {
		size_t len = sizeof(k);
		uint8_t *s = (uint8_t *)&k;
		unsigned long h = 14695981039346656037ul;
		while (len--) {
			h ^= *s++;
			h *= 1099511628211ul;
		}
		return h;
	}
};


int main() {
#if 0
	std::hash<size_t> h = std::hash<size_t>();
	uint32_t a = 0, aa = 0;
	uint64_t b = 0;
	uint64_t c[2] = {0, 0};
	for (size_t i = 0; i < 100; i++) {
#if 0
		a = MurmurHash3_x86_32(&i, sizeof(size_t), 134789237);
		b = MurmurHash3_x64_64(&i, sizeof(size_t), 134789237);
		MurmurHash3_x64_128(&i, sizeof(size_t), 134789237, c);

		printf("%03lu  %08X  %016zX  %016zX %016zX\n", i, a, b, c[0], c[1]);
#endif

#if 1
		a = CRC32(&i, sizeof(size_t));
		aa = CRC32_x64(&i, sizeof(size_t));

		printf("%03lu  %08X %08X\n", i, a, aa);
#endif
	}
#endif


#if 1
	const size_t runs = 300000000;

	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<size_t> dist(0, UINT64_MAX);

	uint64_t *data = (uint64_t *)malloc(sizeof(uint64_t)*runs);
	for (size_t i = 0; i < runs; i++)
		data[i] = dist(gen);

#ifdef __SSE4_2__
	for (int c = 0; c < 3; c++) {
		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		volatile uint32_t a = 0;
		for (size_t i = 0; i < runs; i+=1) {
			a = CRC32(data + i, 1*sizeof(size_t));
		}

		t.end();
		printf("          CRC32: ");
		printf("%15.10fms total, %15.10fns per hash, %7.3f cycles per hash\n", t.elapsed()*1000, t.elapsed()*1e9/runs, 3.7*1e9*t.elapsed()/runs);
	}
	puts("");
#endif

	for (int c = 0; c < 3; c++) {
		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		volatile uint64_t a = 0;
		for (size_t i = 0; i < runs; i++) {
			a = MurmurHash3_x86_32(data + i, sizeof(size_t), 134789237);
		}

		t.end();
		printf(" Murmur3_x86_32: ");
		printf("%15.10fms total, %15.10fns per hash, %7.3f cycles per hash\n", t.elapsed()*1000, t.elapsed()*1e9/runs, 3.7*1e9*t.elapsed()/runs);
	}
	puts("");

	for (int c = 0; c < 3; c++) {
		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		volatile uint64_t a = 0;
		for (size_t i = 0; i < runs; i++) {
			a = MurmurHash3_x64_64(data + i, sizeof(size_t), 134789237);
		}

		t.end();
		printf(" Murmur3_x64_64: ");
		printf("%15.10fms total, %15.10fns per hash, %7.3f cycles per hash\n", t.elapsed()*1000, t.elapsed()*1e9/runs, 3.7*1e9*t.elapsed()/runs);
	}
	puts("");

	for (int c = 0; c < 3; c++) {
		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		uint64_t a[2] = {0, 0};
		for (size_t i = 0; i < runs; i+=2) {
			MurmurHash3_x64_128(data + i, 2*sizeof(size_t), 134789237, a);
		}

		t.end();
		printf("Murmur3_x64_128: ");
		printf("%15.10fms total, %15.10fns per hash, %7.3f cycles per hash\n", t.elapsed()*1000, t.elapsed()*1e9/runs, 3.7*1e9*t.elapsed()/runs);
	}
	puts("");

	for (int c = 0; c < 3; c++) {
		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		volatile size_t a = 0;
		for (size_t i = 0; i < runs; i++) {
			a = std::_Hash_bytes(data + i, sizeof(size_t), 2346239467);
		}

		t.end();
		printf("    _Hash_bytes: ");
		printf("%15.10fms total, %15.10fns per hash, %7.3f cycles per hash\n", t.elapsed()*1000, t.elapsed()*1e9/runs, 3.7*1e9*t.elapsed()/runs);
	}
	puts("");

	for (int c = 0; c < 3; c++) {
		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		volatile size_t a = 0;
		for (size_t i = 0; i < runs; i++) {
			a = std::_Fnv_hash_bytes(data + i, sizeof(size_t), 2346239467);
		}

		t.end();
		printf("_Fnv_hash_bytes: ");
		printf("%15.10fms total, %15.10fns per hash, %7.3f cycles per hash\n", t.elapsed()*1000, t.elapsed()*1e9/runs, 3.7*1e9*t.elapsed()/runs);
	}
	puts("");

	for (int c = 0; c < 3; c++) {
		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		volatile size_t a = 0;
		for (size_t i = 0; i < runs; i++) {
			a = CrapWow64(data + i, sizeof(size_t), 2314781923);
		}

		t.end();
		printf("        CrapWow: ");
		printf("%15.10fms total, %15.10fns per hash, %7.3f cycles per hash\n", t.elapsed()*1000, t.elapsed()*1e9/runs, 3.7*1e9*t.elapsed()/runs);
	}
	puts("");

	for (int c = 0; c < 3; c++) {
		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		simplehash h;
		volatile size_t a = 0;
		for (size_t i = 0; i < runs; i++) {
			a = h(data[i]);
		}

		t.end();
		printf("         Simple: ");
		printf("%15.10fms total, %15.10fns per hash, %7.3f cycles per hash\n", t.elapsed()*1000, t.elapsed()*1e9/runs, 3.7*1e9*t.elapsed()/runs);
	}
	puts("");

	for (int c = 0; c < 3; c++) {
		Timer t(CLOCK_PROCESS_CPUTIME_ID);
		t.start();

		fnv1a h;
		volatile size_t a = 0;
		for (size_t i = 0; i < runs; i++) {
			a = h(data[i]);
		}

		t.end();
		printf("          fnv1a: ");
		printf("%15.10fms total, %15.10fns per hash, %7.3f cycles per hash\n", t.elapsed()*1000, t.elapsed()*1e9/runs, 3.7*1e9*t.elapsed()/runs);
	}
	puts("");
#endif

	return 0;
}
