#include "comserver.h"

#include <algorithm>
#include <sstream>
#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>

void createArt(string title, string author, string text, int art_nbr, string path){
	ofstream outfile;
	cout << path << endl;
	outfile.open (path + to_string(art_nbr));
	outfile << "#" + title;
	outfile << "#" + author;
	outfile << "#" + text;
  	outfile.close();
}

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
	string h = getenv("HOME");
	

	while (true) {
		cs.handleActivity();
		ngs = cs.get_grps();
		int ns = cs.last_modified();

		if (ns != -1) {
			string home = h + "/newsgroups/";
			auto it = find_if(ngs.begin(), ngs.end(), [&ns](Newsgroup& n) {return n.get_id() == ns;});
			create_newsgroup(it->get_name().c_str());
			auto articles = it->get_articles();
			home += it->get_name() + "/";
			for(Article a : articles){
				createArt(a.get_title(), a.get_author(), a.get_text(), a.get_id(), home);
			}
		}
		

		// Skriv grupperna till fil
	}


	return 0;
}