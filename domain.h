#pragma once

#include <QString>

struct Bookmaker {
	std::string name;
	std::string event;
	std::string bet;
	double odd;
};

struct Fork {
	Bookmaker booker_1;
	Bookmaker booker_2;
	double profit;
};

struct Settings {
	std::string login;
	std::string password;
	std::string server_url = "server";
	bool hidden    = false;
	bool is_logged = false;
	bool out	   = true;
	int width  = 1920;
	int height = 1080;
};