#include "DataManager.h"

DataManager::DataManager() {
	pthread_mutex_init(&mutex, NULL);
}

DataManager::~DataManager() {
	pthread_mutex_destroy(&mutex);
}


void DataManager::addRay(size_t client, const Ray& r) {
	pthread_mutex_lock(&mutex);
	   DataSource &ds = data[client];
	pthread_mutex_unlock(&mutex);

	ds.add(r);
}

void DataManager::addImage(size_t client, size_t id, std::string& img) {
	//TODO
}

void DataManager::removeClient(size_t client) {
	data[client].remove();
}


void DataManager::logStreams() {
	std::vector<DataSource *> clients;

	pthread_mutex_lock(&mutex);
		for(std::map<size_t, DataSource>::iterator d = data.begin(); d != data.end(); ++d) {
			clients.push_back(&d->second);
			d->second.lock();
		}

		size_t n_clients = clients.size();

		timespec cur_time;
		clock_gettime(CLOCK_REALTIME, &cur_time);
		double time = cur_time.tv_sec + 1e-9*(double)cur_time.tv_nsec;

		char cl[3][50] = {"NaN", "NaN", "NaN"};
		size_t print_count = 0;

		for (size_t i = 0; i < n_clients; i++) {
			if (clients[i]->rays[1].empty())
				continue;

			Ray r_i = clients[i]->rays[1].front();
			double i_time = r_i.time_sec + 1e-9*(double)r_i.time_nsec;

			clients[i]->rays[1].pop_front();

			sprintf(cl[i], "%20.6f", i_time);
			print_count++;
		}

		if (print_count != 0)
			fprintf(stderr, "%20.6f %20s %20s %20s\n", time, cl[0], cl[1], cl[2]);

		for (size_t i = 0; i < n_clients; i++) {
			clients[i]->unlock();
		}
	pthread_mutex_unlock(&mutex);
}

void DataManager::syncData(DataResults& results) {
	std::vector<DataSource *> clients;
	pthread_mutex_lock(&mutex);
		for(std::map<size_t, DataSource>::iterator d = data.begin(); d != data.end(); ++d) {
			clients.push_back(&d->second);
			d->second.lock();
		}

		size_t n_clients = clients.size();

		timespec stime;
		clock_gettime(CLOCK_REALTIME, &stime);
		double server_time = stime.tv_sec + 1e-9*(double)stime.tv_nsec;

		for (size_t s = 0; s < 4; s++) {
			//printf("Ray %lu\n", s);

			for (size_t c_i = 0; c_i < n_clients; c_i++) {
				if (clients[c_i]->rays[s].empty())
					continue;

				for (size_t r_i = 0; r_i < clients[c_i]->rays[s].size(); r_i++) {
					Ray ray_i = clients[c_i]->rays[s][r_i];

					Match m(server_time, n_clients);
					m.rays.insert(ray_i);

					search[s].matches.push_back(m);
				}

				clients[c_i]->rays[s].clear();
			}

			search[s].mergeMatches();
			search[s].getResults(results, server_time);

#if 0

			bool printed = false;
			for (std::vector<Match>::iterator m = search[s].matches.begin(); m != search[s].matches.end(); ++m) {
				m->print();
				printed = true;
			}

			if (printed)
				printf("\n");
#endif

		}

		for(std::map<size_t, DataSource>::iterator d = data.begin(); d != data.end();) {
			d->second.unlock();

			if (d->second.dead)
				data.erase(d++);
			else
				++d;
		}

	pthread_mutex_unlock(&mutex);


}

