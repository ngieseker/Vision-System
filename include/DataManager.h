#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "fxbcam.h"
#include "ThreadDeque.h"

#include <pthread.h>

#include <algorithm>
#include <vector>
#include <set>

const size_t min_matches   = 2;
const double max_time_diff = 0.015;
const double match_delay   = 0.0173;
const double match_expire  = 2*match_delay;

typedef ThreadDeque<Ray> RayArray[4];
typedef std::vector<std::vector<Ray> > DataResults;

class DataManager {
public:
	DataManager();
	~DataManager();

	void addRay(size_t client, const Ray& r);
	void addImage(size_t client, size_t id, std::string& img);
	void removeClient(size_t client);

	void logStreams();
	void syncData(DataResults& results);

private:
	struct Match {
		std::set<Ray> rays;

		double time;

		size_t n_clients;

		void print(FILE *out = stdout) {
			fprintf(out, "%lu {", rays.begin()->marker);
			for (std::set<Ray>::iterator r = rays.begin(); r != rays.end();) {
				double time = r->time_sec + 1e-9*(double)r->time_nsec;
				fprintf(out, "%2lu:%15.6f", r->client_id, time);

				if (++r != rays.end())
					fprintf(out, ", ");
			}
			fprintf(out, " }\n");
		}

		Match(double time, size_t n_clients) : time(time), n_clients(n_clients) {}

		bool operator<(const Match &rhs) const {
			return time < rhs.time;
		}
	};

	struct syncSearch {
		std::vector<Match> matches;

		void mergeMatches() {
			for (std::vector<Match>::iterator m = matches.begin(); m != matches.end(); ++m) {
				for (std::vector<Match>::iterator n = m+1; n != matches.end();) {
					
					// Check that all rays are valid
					for (std::set<Ray>::iterator m_ray = m->rays.begin(); m_ray != m->rays.end(); ++m_ray) {
						for (std::set<Ray>::iterator n_ray = n->rays.begin(); n_ray != n->rays.end(); ++n_ray) {
							double m_time = m_ray->time_sec + 1e-9*(double)m_ray->time_nsec;
							double n_time = n_ray->time_sec + 1e-9*(double)n_ray->time_nsec;

							if (fabs(m_time - n_time) >= max_time_diff) {
								goto skipMatch;
							}
						}
					}

					m->rays.insert(n->rays.begin(), n->rays.end());
					n = matches.erase(n);
					continue;

skipMatch:
					++n;
					continue;
				}
			}
		}
			
		bool getResults(DataResults& results, double cur_time) {
			std::stable_sort(matches.begin(), matches.end());

			for (std::vector<Match>::iterator m = matches.begin(); m != matches.end();) {
				if (m->time + match_expire <= cur_time) {
#ifdef SYNC_DEBUG
					printf("  Expired: "); m->print();
#endif

					m = matches.erase(m);
				} else if (m->time + match_delay <= cur_time && m->rays.size() >= min_matches) {
#ifdef SYNC_DEBUG
					printf("  Timed out match %15.6f: ", cur_time); m->print();
#endif

					//results.push_back(m->rays);
					results.push_back(std::vector<Ray>(m->rays.begin(), m->rays.end()));

					m = matches.erase(m);

					return true;
				} else if (m->rays.size() >= m->n_clients && m->rays.size() >= min_matches) {
#ifdef SYNC_DEBUG
					printf("  Full match %15.6f: ", cur_time); m->print();
#endif

					//results.push_back(m->rays);
					results.push_back(std::vector<Ray>(m->rays.begin(), m->rays.end()));

					m = matches.erase(m);

					return true;
				} else {
					++m;
				}
			}

			return false;
		}

	};

	struct DataSource {
		bool dead;
		RayArray rays;

		void add(const Ray& r) {
			if (dead) dead = false; // Lock needed?
			rays[r.marker].locked_push_back(r);
		}

		void remove() {
			dead = true;
		}

		void lock() {
			for (size_t i = 0; i < 4; i++) {
				rays[i].lock();
			}
		}

		void unlock() {
			for (size_t i = 0; i < 4; i++) {
				rays[i].unlock();
			}
		}

		DataSource() : dead(false) {}
	};
	
	std::map<size_t, DataSource> data;
	syncSearch search[4];

	pthread_mutex_t mutex;

};

#endif
