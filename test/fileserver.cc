#include "comserver.h"

#include <algorithm>
#include <sstream>
#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>



void create_newsgroup(const char* s){
	string h = getenv("HOME");
	string home = h + "/newsgroups/";
	int s_one = mkdir(home.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	int s_two = mkdir((home + s).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	cout << "Status for creating root-newsgroup folder: " << s_one << endl;
	cout << "Status for creating newsgroup folder: " << s_two << endl;
}

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
	vector<Newsgroup> ngs; // LÄSA IN ALLt
	ComServer cs(port);

	if (!cs.isInitialized()) {
		cerr << "Server initialization error." << endl;
		exit(1);
	}


	while (true) {
		cs.handleActivity();
		ngs = cs.get_grps();
		int ns = cs.last_modified();
		if (ns != -1) {
			auto it = find_if(ngs.begin(), ngs.end(), [&ns](Newsgroup& n) {return n.get_id() == ns;});
		} 
		

		// Skriv grupperna till fil
	}


	return 0;
}