#ifndef HASH_H
#define HASH_H

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "immintrin.h"


inline size_t hash_bytes(const void *key, size_t len);

inline uint32_t CRC32(const void *key, size_t len);

inline uint32_t MurmurHash3_x86_32 (const void *key, size_t len, uint32_t seed)            __attribute__((always_inline));
inline uint64_t MurmurHash3_x64_64 (const void *key, size_t len, uint64_t seed)            __attribute__((always_inline));
inline void     MurmurHash3_x86_128(const void* key, size_t len, uint32_t seed, void *out) __attribute__((always_inline));
inline void     MurmurHash3_x64_128(const void *key, size_t len, uint64_t seed, void *out) __attribute__((always_inline));

inline uint32_t CrapWow32(const void *key, size_t len, uint32_t seed) __attribute__((always_inline));
inline uint64_t CrapWow64(const void *key, size_t len, uint64_t seed) __attribute__((always_inline));


/*** hash functors for multiple built-in types ***/
template <typename T> struct hash;

template <> struct hash<char>;
template <> struct hash<unsigned char>;
template <> struct hash<short>;
template <> struct hash<unsigned short>;
template <> struct hash<int>;
template <> struct hash<unsigned int>;
template <> struct hash<long int>;
template <> struct hash<unsigned long int>;
template <> struct hash<long long>;
template <> struct hash<unsigned long long>;
template <> struct hash<float>;
template <> struct hash<double>;


template <typename T>
struct hash {
	size_t operator()(const T& k) const;
};


template <>
struct hash<char> {
	size_t operator()(const char& k) const {
		return hash_bytes(&k, sizeof(char));
	}
};

template <>
struct hash<unsigned char> {
	size_t operator()(const unsigned char& k) const {
		return hash_bytes(&k, sizeof(unsigned char));
	}
};


template <>
struct hash<short> {
	size_t operator()(const short& k) const {
		return hash_bytes(&k, sizeof(short));
	}
};

template <>
struct hash<unsigned short> {
	size_t operator()(const unsigned short& k) const {
		return hash_bytes(&k, sizeof(unsigned short));
	}
};


template <>
struct hash<int> {
	size_t operator()(const int& k) const {
		return hash_bytes(&k, sizeof(int));
	}
};

template <>
struct hash<unsigned int> {
	size_t operator()(const unsigned int& k) const {
		return hash_bytes(&k, sizeof(unsigned int));
	}
};


template <>
struct hash<long int> {
	size_t operator()(const long int& k) const {
		return hash_bytes(&k, sizeof(long int));
	}
};

template <>
struct hash<unsigned long int> {
	size_t operator()(const unsigned long int& k) const {
		return hash_bytes(&k, sizeof(unsigned long int));
	}
};


template <>
struct hash<long long> {
	size_t operator()(const long long& k) const {
		return hash_bytes(&k, sizeof(long long));
	}
};

template <>
struct hash<unsigned long long> {
	size_t operator()(const unsigned long long& k) const {
		return hash_bytes(&k, sizeof(unsigned long long));
	}
};


template <>
struct hash<float> {
	size_t operator()(const float& k) const {
		return hash_bytes(&k, sizeof(float));
	}
};


template <>
struct hash<double> {
	size_t operator()(const double& k) const {
		return hash_bytes(&k, sizeof(double));
	}
};



/*************************************
 *         Hashing Functions         *
 *************************************/

// Global wrapper
inline size_t hash_bytes(const void *key, size_t len) {
#if __SIZEOF_SIZE_T == 8
	return MurmurHash3_x64_64(key, len, 1237891247);
	//return MurmurHash3_x86_32(key, len, 1237891247);
#else
	return MurmurHash3_x86_32(key, len, 1237891247);
#endif
}


/******** Hardware CRC32 ********/
// Extremely fast, requires SSE4.2
// Not really a hashing function, but can serve as one

#ifdef __SSE4_2__
inline uint32_t CRC32(const void *key, size_t len) {
	const uint8_t *data = (const uint8_t *)key;

	uint64_t crc64 = 0xFFFFFFFF;

	for (size_t i = 0; i < len / 8; i++) {
		crc64 = _mm_crc32_u64(crc64, *(uint64_t*)data);
		data += sizeof(uint64_t);
	}

	uint32_t crc = (uint32_t)crc64;

	switch (len & 7) {
		case 7:
			crc = _mm_crc32_u32(crc, *(uint32_t *)(data + 0));
			crc = _mm_crc32_u16(crc, *(uint16_t*) (data + 4));
			crc = _mm_crc32_u8 (crc, *(uint8_t *) (data + 6));
			break;
		case 6:
			crc = _mm_crc32_u32(crc, *(uint32_t *)(data + 0));
			crc = _mm_crc32_u16(crc, *(uint16_t*) (data + 4));
			break;
		case 5:
			crc = _mm_crc32_u32(crc, *(uint32_t *)(data + 0));
			crc = _mm_crc32_u8 (crc, *(uint8_t *) (data + 4));
			break;
		case 4:
			crc = _mm_crc32_u32(crc, *(uint32_t *)(data + 0));
			break;
		case 3:
			crc = _mm_crc32_u16(crc, *(uint16_t*) (data + 0));
			crc = _mm_crc32_u8 (crc, *(uint8_t *) (data + 2));
			break;
		case 2:
			crc = _mm_crc32_u16(crc, *(uint16_t*) (data + 0));
			break;
		case 1:
			crc = _mm_crc32_u8 (crc, *(uint8_t *) (data + 0));
			break;
	}

	return crc;
}
#endif


/******** MurmurHash3 ********/
// By Austin Appleby
// Fast and good

namespace {
#define	FORCE_INLINE inline __attribute__((always_inline))

	inline uint32_t rotl32 ( uint32_t x, int8_t r )
	{
	  return (x << r) | (x >> (32 - r));
	}

	inline uint64_t rotl64 ( uint64_t x, int8_t r )
	{
	  return (x << r) | (x >> (64 - r));
	}

#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

	//-----------------------------------------------------------------------------
	// Block read - if your platform needs to do endian-swapping or can only
	// handle aligned reads, do the conversion here

	FORCE_INLINE uint32_t getblock ( const uint32_t * p, int i )
	{
	  return p[i];
	}

	FORCE_INLINE uint64_t getblock ( const uint64_t * p, int i )
	{
	  return p[i];
	}

	//-----------------------------------------------------------------------------
	// Finalization mix - force all bits of a hash block to avalanche

	FORCE_INLINE uint32_t fmix ( uint32_t h )
	{
	  h ^= h >> 16;
	  h *= 0x85ebca6b;
	  h ^= h >> 13;
	  h *= 0xc2b2ae35;
	  h ^= h >> 16;

	  return h;
	}


	FORCE_INLINE uint64_t fmix ( uint64_t k )
	{
	  k ^= k >> 33;
	  k *= BIG_CONSTANT(0xff51afd7ed558ccd);
	  k ^= k >> 33;
	  k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
	  k ^= k >> 33;

	  return k;
	}
}

inline uint32_t MurmurHash3_x86_32(const void *key, size_t len, uint32_t seed) {
	const uint8_t * data = (const uint8_t*)key;
	const int nblocks = len / 4;

	uint32_t h1 = seed;

	const uint32_t c1 = 0xcc9e2d51;
	const uint32_t c2 = 0x1b873593;

	//----------
	// body

	const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

	for(int i = -nblocks; i; i++) {
		uint32_t k1 = getblock(blocks,i);

		k1 *= c1;
		k1 = ROTL32(k1,15);
		k1 *= c2;

		h1 ^= k1;
		h1 = ROTL32(h1,13); 
		h1 = h1*5+0xe6546b64;
	}

	//----------
	// tail

	const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

	uint32_t k1 = 0;

	switch(len & 3) {
		case 3: k1 ^= tail[2] << 16;
		case 2: k1 ^= tail[1] << 8;
		case 1: k1 ^= tail[0];
		        k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
	};

	//----------
	// finalization

	h1 ^= len;

	h1 = fmix(h1);

	return h1;
} 

inline uint64_t MurmurHash3_x64_64(const void *key, size_t len, uint64_t seed) {
	const uint8_t * data = (const uint8_t*)key;
	const int nblocks = len / 8;

	uint64_t h1 = seed;

	const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
	const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

	//----------
	// body

	const uint64_t * blocks = (const uint64_t *)(data);

	for(int i = 0; i < nblocks; i++) {
		uint64_t k1 = getblock(blocks,i);

		k1 *= c1;
		k1  = ROTL64(k1,31);
		k1 *= c2;

		h1 ^= k1;
		h1 = ROTL64(h1,27);
		h1 = h1*5+0x52dce729;
	}

	//----------
	// tail

	const uint8_t * tail = (const uint8_t*)(data + nblocks*8);

	uint64_t k1 = 0;

	switch(len & 7) {
		case  8: k1 ^= uint64_t(tail[ 7]) << 56;
		case  7: k1 ^= uint64_t(tail[ 6]) << 48;
		case  6: k1 ^= uint64_t(tail[ 5]) << 40;
		case  5: k1 ^= uint64_t(tail[ 4]) << 32;
		case  4: k1 ^= uint64_t(tail[ 3]) << 24;
		case  3: k1 ^= uint64_t(tail[ 2]) << 16;
		case  2: k1 ^= uint64_t(tail[ 1]) << 8;
		case  1: k1 ^= uint64_t(tail[ 0]) << 0;
		         k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
	};

	//----------
	// finalization

	h1 ^= len;

	h1 = fmix(h1);

	return h1;
}

void MurmurHash3_x86_128(const void* key, size_t len, uint32_t seed, void *out) {
	const uint8_t * data = (const uint8_t*)key;
	const int nblocks = len / 16;

	uint32_t h1 = seed;
	uint32_t h2 = seed;
	uint32_t h3 = seed;
	uint32_t h4 = seed;

	const uint32_t c1 = 0x239b961b; 
	const uint32_t c2 = 0xab0e9789;
	const uint32_t c3 = 0x38b34ae5; 
	const uint32_t c4 = 0xa1e38b93;

	//----------
	// body

	const uint32_t * blocks = (const uint32_t *)(data + nblocks*16);

	for(int i = -nblocks; i; i++) {
		uint32_t k1 = getblock(blocks,i*4+0);
		uint32_t k2 = getblock(blocks,i*4+1);
		uint32_t k3 = getblock(blocks,i*4+2);
		uint32_t k4 = getblock(blocks,i*4+3);

		k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;

		h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;

		k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;

		h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;

		k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;

		h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;

		k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;

		h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
	}

	//----------
	// tail

	const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

	uint32_t k1 = 0;
	uint32_t k2 = 0;
	uint32_t k3 = 0;
	uint32_t k4 = 0;

	switch(len & 15) {
		case 15: k4 ^= tail[14] << 16;
		case 14: k4 ^= tail[13] << 8;
		case 13: k4 ^= tail[12] << 0;
		         k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;

		case 12: k3 ^= tail[11] << 24;
		case 11: k3 ^= tail[10] << 16;
		case 10: k3 ^= tail[ 9] << 8;
		case  9: k3 ^= tail[ 8] << 0;
		         k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;

		case  8: k2 ^= tail[ 7] << 24;
		case  7: k2 ^= tail[ 6] << 16;
		case  6: k2 ^= tail[ 5] << 8;
		case  5: k2 ^= tail[ 4] << 0;
		         k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;

		case  4: k1 ^= tail[ 3] << 24;
		case  3: k1 ^= tail[ 2] << 16;
		case  2: k1 ^= tail[ 1] << 8;
		case  1: k1 ^= tail[ 0] << 0;
		         k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
	};

	//----------
	// finalization

	h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;

	h1 += h2; h1 += h3; h1 += h4;
	h2 += h1; h3 += h1; h4 += h1;

	h1 = fmix(h1);
	h2 = fmix(h2);
	h3 = fmix(h3);
	h4 = fmix(h4);

	h1 += h2; h1 += h3; h1 += h4;
	h2 += h1; h3 += h1; h4 += h1;

	((uint32_t*)out)[0] = h1;
	((uint32_t*)out)[1] = h2;
	((uint32_t*)out)[2] = h3;
	((uint32_t*)out)[3] = h4;
}

inline void MurmurHash3_x64_128(const void *key, size_t len, uint64_t seed, void *out) {
	const uint8_t * data = (const uint8_t*)key;
	const int nblocks = len / 16;

	uint64_t h1 = seed;
	uint64_t h2 = seed;

	const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
	const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

	//----------
	// body

	const uint64_t * blocks = (const uint64_t *)(data);

	for(int i = 0; i < nblocks; i++) {
		uint64_t k1 = getblock(blocks,i*2+0);
		uint64_t k2 = getblock(blocks,i*2+1);

		k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;

		h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

		k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

		h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
	}

	//----------
	// tail

	const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

	uint64_t k1 = 0;
	uint64_t k2 = 0;

	switch(len & 15) {
		case 15: k2 ^= uint64_t(tail[14]) << 48;
		case 14: k2 ^= uint64_t(tail[13]) << 40;
		case 13: k2 ^= uint64_t(tail[12]) << 32;
		case 12: k2 ^= uint64_t(tail[11]) << 24;
		case 11: k2 ^= uint64_t(tail[10]) << 16;
		case 10: k2 ^= uint64_t(tail[ 9]) << 8;
		case  9: k2 ^= uint64_t(tail[ 8]) << 0;
		         k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

		case  8: k1 ^= uint64_t(tail[ 7]) << 56;
		case  7: k1 ^= uint64_t(tail[ 6]) << 48;
		case  6: k1 ^= uint64_t(tail[ 5]) << 40;
		case  5: k1 ^= uint64_t(tail[ 4]) << 32;
		case  4: k1 ^= uint64_t(tail[ 3]) << 24;
		case  3: k1 ^= uint64_t(tail[ 2]) << 16;
		case  2: k1 ^= uint64_t(tail[ 1]) << 8;
		case  1: k1 ^= uint64_t(tail[ 0]) << 0;
		         k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
	};

	//----------
	// finalization

	h1 ^= len; h2 ^= len;

	h1 += h2;
	h2 += h1;

	h1 = fmix(h1);
	h2 = fmix(h2);

	h1 += h2;
	h2 += h1;

	((uint64_t *)out)[0] = h1;
	((uint64_t *)out)[1] = h2;
}



/******** CrapWow ********/
// Fast but bad

inline uint32_t CrapWow32(const void *key, size_t len, uint32_t seed) {
#if defined(__GNUC__)
	// esi = k, ebx = h
	uint32_t hash;
	asm(
		"leal 0x5052acdb(%%ecx,%%esi), %%esi\n"
		"movl %%ecx, %%ebx\n"
		"cmpl $8, %%ecx\n"
		"jb DW%=\n"
	"QW%=:\n"
		"movl $0x5052acdb, %%eax\n"
		"mull (%%edi)\n"
		"addl $-8, %%ecx\n"
		"xorl %%eax, %%ebx\n"
		"xorl %%edx, %%esi\n"
		"movl $0x57559429, %%eax\n"
		"mull 4(%%edi)\n"
		"xorl %%eax, %%esi\n"
		"xorl %%edx, %%ebx\n"
		"addl $8, %%edi\n"
		"cmpl $8, %%ecx\n"
		"jae QW%=\n"
	"DW%=:\n"
		"cmpl $4, %%ecx\n"
		"jb B%=\n"
		"movl $0x5052acdb, %%eax\n"
		"mull (%%edi)\n"
		"addl $4, %%edi\n"
		"xorl %%eax, %%ebx\n"
		"addl $-4, %%ecx\n"
		"xorl %%edx, %%esi\n"
	"B%=:\n"
		"testl %%ecx, %%ecx\n"
		"jz F%=\n"
		"shll $3, %%ecx\n"
		"movl $1, %%edx\n"
		"movl $0x57559429, %%eax\n"
		"shll %%cl, %%edx\n"
		"addl $-1, %%edx\n"
		"andl (%%edi), %%edx\n"
		"mull %%edx\n"
		"xorl %%eax, %%esi\n"
		"xorl %%edx, %%ebx\n"
	"F%=:\n"
		"leal 0x5052acdb(%%esi), %%edx\n"
		"xorl %%ebx, %%edx\n"
		"movl $0x5052acdb, %%eax\n"
		"mull %%edx\n"
		"xorl %%ebx, %%eax\n"
		"xorl %%edx, %%esi\n"
		"xorl %%esi, %%eax\n"
		: "=a"(hash), "=c"(len), "=S"(len), "=D"(key)
		: "c"(len), "S"(seed), "D"(key)
		: "%ebx", "%edx", "cc" 
	);
	return hash;
#else
	#define cwfold( a, b, lo, hi ) { p = (uint32_t)(a) * (uint64_t)(b); lo ^= (uint32_t)p; hi ^= (uint32_t)(p >> 32); }
	#define cwmixa( in ) { cwfold( in, m, k, h ); }
	#define cwmixb( in ) { cwfold( in, n, h, k ); }

	const uint32_t m = 0x57559429, n = 0x5052acdb, *key4 = (const uint32_t *)key;
	uint32_t h = len, k = len + seed + n;
	uint64_t p;

	while ( len >= 8 ) { cwmixb(key4[0]) cwmixa(key4[1]) key4 += 2; len -= 8; }
	if ( len >= 4 ) { cwmixb(key4[0]) key4 += 1; len -= 4; }
	if ( len ) { cwmixa( key4[0] & ( ( 1 << ( len * 8 ) ) - 1 ) ) }
	cwmixb( h ^ (k + n) )
	return k ^ h;
#endif
}

inline uint64_t CrapWow64(const void *key, size_t len, uint64_t seed) {
	const uint64_t m = 0x95b47aa3355ba1a1, n = 0x8a970be7488fda55;
#if defined(__GNUC__) && 0
	uint64_t hash;
	// 3 = m, 4 = n
	// r12 = h, r13 = k, ecx = seed, r12 = key
	asm(
		"leaq (%%rcx,%4), %%r13\n"
		"movq %%rdx, %%r14\n"
		"movq %%rcx, %%r15\n"
		"movq %%rcx, %%r12\n"
		"addq %%rax, %%r13\n"
		"andq $0xfffffffffffffff0, %%rcx\n"
		"jz QW%=\n"
		"addq %%rcx, %%r14\n\n"
		"negq %%rcx\n"
	"XW%=:\n"
		"movq %4, %%rax\n"
		"mulq (%%r14,%%rcx)\n"
		"xorq %%rax, %%r12\n"
		"xorq %%rdx, %%r13\n"
		"movq %3, %%rax\n"
		"mulq 8(%%r14,%%rcx)\n"
		"xorq %%rdx, %%r12\n"
		"xorq %%rax, %%r13\n"
		"addq $16, %%rcx\n"
		"jnz XW%=\n"
	"QW%=:\n"
		"movq %%r15, %%rcx\n"
		"andq $8, %%r15\n"
		"jz B%=\n"
		"movq %4, %%rax\n"
		"mulq (%%r14)\n"
		"addq $8, %%r14\n"
		"xorq %%rax, %%r12\n"
		"xorq %%rdx, %%r13\n"
	"B%=:\n"
		"andq $7, %%rcx\n"
		"jz F%=\n"
		"movq $1, %%rdx\n"
		"shlq $3, %%rcx\n"
		"movq %3, %%rax\n"
		"shlq %%cl, %%rdx\n"
		"addq $-1, %%rdx\n"
		"andq (%%r14), %%rdx\n"
		"mulq %%rdx\n"
		"xorq %%rdx, %%r12\n"
		"xorq %%rax, %%r13\n"
	"F%=:\n"
		"leaq (%%r13,%4), %%rax\n"
		"xorq %%r12, %%rax\n"
		"mulq %4\n"
		"xorq %%rdx, %%rax\n"
		"xorq %%r12, %%rax\n"
		"xorq %%r13, %%rax\n"
		: "=a"(hash), "=c"(key), "=d"(key)
		: "r"(m), "r"(n), "a"(seed), "c"(len), "d"(key)
		: "%r12", "%r13", "%r14", "%r15", "cc" 
	);
	return hash;
#else
	#define cwfold( a, b, lo, hi ) { p = (uint64_t)(a) * (__uint128_t)(b); lo ^= (uint64_t)p; hi ^= (uint64_t)(p >> 64); }
	#define cwmixa( in ) { cwfold( in, m, k, h ); }
	#define cwmixb( in ) { cwfold( in, n, h, k ); }

	const uint64_t *key8 = (const uint64_t *)key;
	uint64_t h = len, k = len + seed + n;
	__uint128_t p;

	while ( len >= 16 ) { cwmixb(key8[0]) cwmixa(key8[1]) key8 += 2; len -= 16; }
	if ( len >= 8 ) { cwmixb(key8[0]) key8 += 1; len -= 8; }
	if ( len ) { cwmixa( key8[0] & ( ( (uint64_t)1 << ( len * 8 ) ) - 1 ) ) }
	cwmixb( h ^ (k + n) )
	return k ^ h;
#endif
}


#endif
