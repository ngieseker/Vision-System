#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "hash.h"

#include <cstdlib>
#include <vector>
#include <math.h>

#include <cstdio>


#define BUCKET_SIZE 16
static const double max_load_factor = 8.0;

/* General purpose hash table
 *  Key type must have hash functor, ==, and copy operations defined
 */
template <typename K, typename V, class H = hash<K> >
class HashMap {
public:
	HashMap() : n_elements(0) {
		buckets = 5;
		next_rehash = buckets * max_load_factor;

		data = new HashBucket[buckets];
	}

	HashMap(const HashMap &hm) {
		copy(hm);
	}


	~HashMap() {
		delete[] data;
	}

	bool insert(const K& key, const V& val) {
		if (n_elements + 1 >= next_rehash)
			rehash();

		size_t i = hash_func(key) % buckets;
		HashBucket *b = &data[i];

		while (b->size == BUCKET_SIZE) {
			rehash();

			i = hash_func(key) % buckets;
			b = &data[i];
		}

		size_t e = 0;
		for (; e < b->size; e++) {
			if (b->data[e].key == key)
				return false;
		}
		b->data[e].key = key;
		b->data[e].val = val;
		b->size++;

		n_elements++;
		return true;
	}

	V& get(const K& key) {
		size_t i = hash_func(key) % buckets;
		HashBucket *b = &data[i];

		for (size_t e = 0; e < b->size; e++) {
			if (b->data[e].key == key)
				return b->data[e].val;
		}


		while (n_elements + 1 >= next_rehash || b->size == BUCKET_SIZE) {
			rehash();

			i = hash_func(key) % buckets;
			b = &data[i];
		}

		b->data[b->size].key = key;
		b->size++;

		n_elements++;
		return b->data[b->size-1].val;
	}

	bool contains(const K& key) {
		size_t i = hash_func(key) % buckets;
		HashBucket *b = &data[i];

		for (size_t e = 0; e < b->size; e++) {
			if (b->data[e].key == key)
				return true;
		}

		return false;
	}

	bool remove(const K& key) {
		size_t i = hash_func(key) % buckets;
		HashBucket *b = &data[i];

		for (size_t e = 0; e < b->size; e++) {
			if (b->data[e].key == key) {
				memmove(b->data + e, b->data + e + 1, (b->size - e - 1)*sizeof(HashElement));
				b->size--;
				return true;
			}
		}

		return false;
	}

	size_t size() {
		return n_elements;
	}

	V& operator[](const K& key) {
		return get(key);
	}

	const V& operator[](const K& key) const {
		return get(key);
	}


	void print() {
		for (size_t i = 0; i < buckets; i++) {
			printf("[%03lu]", i);
			HashBucket *b = &data[i];
			for (size_t e = 0; e < b->size; e++) {
				printf("=");
			}
			printf("\n");
		}
		printf("\n");
	}


	HashMap& operator=(const HashMap& hm) {
		copy(hm);
		return *this;
	}

private:
	struct HashElement {
		K key;
		V val;
	};

	struct HashBucket {
		HashElement data[BUCKET_SIZE];
		size_t size;

		HashBucket() : size(0) {}

		HashBucket& operator=(const HashBucket& hb) {
			for (size_t i = 0; i < size; i++) {
				data[i].key = hb.data[i].key;
				data[i].val = hb.data[i].val;
			}

			return *this;
		}
	};

	HashBucket *data;
	size_t buckets;
	size_t n_elements;

	size_t next_rehash;

	H hash_func;


	void copy(const HashMap& hm) {
		buckets = hm.buckets;
		next_rehash = hm.next_rehash;

		data = new HashBucket[buckets];

		for (size_t i = 0; i < buckets; i++) {
			data[i] = hm.data[i];
		}
	}

	void rehash() {
		size_t new_buckets;
		HashBucket *new_data;

startrehash:
		//print();
		new_buckets = buckets * 2;
		next_rehash = new_buckets * max_load_factor;

		new_data = new HashBucket[new_buckets];

		size_t h;
		HashBucket *b, *n;
		for (size_t i = 0; i < buckets; i++) {
			b = &data[i];

			for (size_t e = 0; e < b->size; e++) {
				h = hash_func(b->data[e].key) % new_buckets;
				n = &new_data[h];

				if (n->size == BUCKET_SIZE) {
					printf("DAMN\n");
					delete[] new_data;
					goto startrehash;
				}

				n->data[n->size] = b->data[e];
				n->size++;
			}
		}

		buckets = new_buckets;
		delete[] data;
		data = new_data;
	}


};

#endif
