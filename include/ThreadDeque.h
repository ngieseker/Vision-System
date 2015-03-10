#ifndef THREAD_DEQUE_H
#define THREAD_DEQUE_H

#include <deque>
#include <pthread.h>

/* ThreadDeque<T>: std::deque<T> wrapper that provides lock and unlock functions,
 *  as well as locked versions of accessor functions
 */

template<typename T>
class ThreadDeque {
public:
	ThreadDeque() {
		pthread_mutex_init(&mutex, NULL);
	}

	~ThreadDeque() {
		pthread_mutex_destroy(&mutex);
	}


	void lock() {
		pthread_mutex_lock(&mutex);
	}

	void unlock() {
		pthread_mutex_unlock(&mutex);
	}


	T& operator[](size_t n) {
		return dq[n];
	}

	T& front() {
		return dq.front();
	}

	T& back() {
		return dq.back();
	}

	void push_front(const T& x) {
		dq.push_front(x);
	}

	void push_back(const T& x) {
		dq.push_back(x);
	}

	void pop_front() {
		dq.pop_front();
	}

	void pop_back() {
		dq.pop_back();
	}

	void clear() {
		dq.clear();
	}

	bool empty() {
		return dq.empty();
	}

	size_t size() {
		return dq.size();
	}


	T locked_front() {
		T ret;
		pthread_mutex_lock(&mutex);
			ret = dq.front();
		pthread_mutex_unlock(&mutex);
		return ret;
	}

	T locked_back() {
		T ret;
		pthread_mutex_lock(&mutex);
			ret = dq.back();
		pthread_mutex_unlock(&mutex);
		return ret;
	}

	void locked_push_front(T x) {
		pthread_mutex_lock(&mutex);
			dq.push_front(x);
		pthread_mutex_unlock(&mutex);
	}

	void locked_push_back(T x) {
		pthread_mutex_lock(&mutex);
			dq.push_back(x);
		pthread_mutex_unlock(&mutex);
	}

	void locked_pop_front() {
		pthread_mutex_lock(&mutex);
			dq.pop_front();
		pthread_mutex_unlock(&mutex);
	}

	void locked_pop_back() {
		pthread_mutex_lock(&mutex);
			dq.pop_back();
		pthread_mutex_unlock(&mutex);
	}

	void locked_clear() {
		pthread_mutex_lock(&mutex);
			dq.clear();
		pthread_mutex_unlock(&mutex);
	}


private:
	std::deque<T> dq;

	pthread_mutex_t mutex;

};

#endif
