#include "Configuration.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <streambuf>



Configuration::Configuration(const char *filename) {
	read(filename);
}

Configuration::Configuration(const std::string &file) {
	read(file);
}


bool Configuration::read(const char *filename) {
	std::ifstream config_file(filename, std::ifstream::in);

	if (!config_file.is_open()) {
		std::cerr << "Error: couldn't open configuration file '" << filename << "'" << std::endl;
		return false;
	}

	config_data.assign((std::istreambuf_iterator<char>(config_file)), std::istreambuf_iterator<char>());
	config_file.close();

	return parse();
}

bool Configuration::read(const std::string &file) {
	config_data = file;

	return parse();
}

bool Configuration::parse() {
	Json::Reader config_reader;
	if (!config_reader.parse(config_data, config)) {
		std::cerr << "Error: couldn't read configuration: ";
		std::cerr << config_reader.getFormattedErrorMessages() << std::endl;
		return false;
	}

	for (size_t c = 0; c < config["cameras"].size(); c++) {
		CameraParams cp;
		Json::Value camera = config["cameras"][(int)c];

		cp.id = camera["id"].asInt();
		cp.guid = camera["guid"].asUInt64();
		cp.calib.assign(camera["calib"].asString());

		cameras[cp.guid] = cp;
	}

	for (size_t c = 0; c < config["clients"].size(); c++) {
		ClientParams cp;
		Json::Value client = config["clients"][(int)c];

		cp.ip.assign(client["ip"].asString());
		cp.name.assign(client["name"].asString());

		clients[cp.ip] = cp;
	}

	return true;
}


bool Configuration::getClient(const char *ip, ClientParams *c) {
	std::map<std::string, ClientParams>::iterator cl;
	if ( (cl = clients.find(ip)) == clients.end() )
		return false;

	c->ip.assign(cl->second.ip);
	c->name.assign(cl->second.name);

	return true;
}

bool Configuration::getCamera(uint64_t guid, CameraParams *c) {
	std::map<uint64_t, CameraParams>::iterator cam;
	if ( (cam = cameras.find(guid)) == cameras.end() )
		return false;

	c->id = cam->second.id;
	c->guid = cam->second.guid;
	c->calib.assign(cam->second.calib);

	return true;
}

int Configuration::getInt(const char *k) {
	return config[k].asInt();
}

double Configuration::getReal(const char *k) {
	return config[k].asDouble();
}

bool Configuration::getBool(const char *k) {
	return config[k].asBool();
}

std::string Configuration::getStr(const char *k) {
	return config[k].asString();
}

const char *Configuration::getCStr(const char *k) {
	return config[k].asCString();
}
