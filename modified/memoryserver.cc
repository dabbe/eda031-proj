#include "comserver.h"

#include <algorithm>
#include <sstream>
#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>

int main(int argc, char* argv[]){
	if (argc != 2) {
		cerr << "Usage: myserver port-number" << endl;
		exit(1);
	}
	
	int port = -1;
	try {
		port = stoi(argv[1]);
	} catch (exception& e) {
		cerr << "Wrong port number. " << e.what() << endl;
		exit(1);
	}

	ComServer cs(port);

	if (!cs.isInitialized()) {
		cerr << "Server initialization error." << endl;
		exit(1);
	}
	vector<Newsgroup> ngs;

	while (true) {
		cs.handleActivity();
	}


	return 0;
}