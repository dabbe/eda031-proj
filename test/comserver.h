#ifndef COMSERVER_H
#define COMSERVER_H

#include "server.h"
#include "newsgroup.h"

#include <algorithm>
#include <sstream>
#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>

class ComServer : private Server {
	public:
		ComServer(int port);
		ComServer(int port, std::vector<Newsgroup> grps);
		void handleActivity();
		void protocol_err(std::string err);
		std::vector<Newsgroup> get_grps();
		bool isInitialized() const;
	private:
		std::vector<Newsgroup> ngs;
		unsigned int ngcounter;
};

#endif