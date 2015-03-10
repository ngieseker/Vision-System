#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <queue>
#include <pthread.h>

/* BlockingQueue<T>: thread-safe std::queue<T> wrapper that blocks on pop()
 *  operations until there is data in the queue
 */
template<typename T>
class BlockingQueue {
public:
	BlockingQueue() : updated(false) {
		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&cond, NULL);
	}

	~BlockingQueue() {
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cond);
	}


	T pop() {
		pthread_mutex_lock(&mutex);
			while (q.empty()) {
				while (!updated) pthread_cond_wait(&cond, &mutex);
				updated = false;
			}
			T ret = q.front();
			q.pop();
		pthread_mutex_unlock(&mutex);
		return ret;
	}

	void push(T &e) {
		pthread_mutex_lock(&mutex);
			q.push(e);
			updated = true;
			pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}

private:
	std::queue<T> q;

	pthread_mutex_t mutex;
	pthread_cond_t cond;
	bool updated;

};

#endif
