#include "ClusterServer.h"
#include <string>
#include <cstdio>

int main(int argc, char *argv[]) {
	std::string config_file("config.json");
	
	if (argc > 2) {
		fprintf(stderr, "server [config file]\n");
		return 1;
	} else if (argc == 2) {
		config_file.assign(argv[1]);
	}


	ClusterServer server;
	if (!server.initialize(config_file.c_str())) {
		fprintf(stderr, "Failed to initialize ClusterServer\n");
		return 1;
	}

	if (!server.run()) {
		fprintf(stderr, "ClusterServer error\n");
		return 1;
	}

	return 0;
}
