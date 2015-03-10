#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include "json/json.h"
#include <stdint.h>
#include <string>
#include <map>


struct CameraParams {
	int id;
	uint64_t guid;
	std::string calib;
};

struct ClientParams {
	std::string ip;
	std::string name;
};

class Configuration {
public:
	Configuration() {}
	Configuration(const char *filename);
	Configuration(const std::string &file);

	bool read(const char *filename);
	bool read(const std::string &file);


	int getInt(const char *k);
	double getReal(const char *k);
	bool getBool(const char *k);
	std::string getStr(const char *k);
	const char *getCStr(const char *k);

	bool getClient(const char *ip, ClientParams *c);
	bool getCamera(uint64_t guid, CameraParams *c);

private:
	std::string config_data;
	Json::Value config;

	std::map<uint64_t, CameraParams>    cameras;
	std::map<std::string, ClientParams> clients;

	bool parse();

};

#endif
