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
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

void createArt(string title, string author, string text, int art_nbr, string path){
	ofstream outfile;
	outfile.open (path + to_string(art_nbr));
	outfile << title << endl;
	outfile << author << endl;
	outfile << text << endl;
  	outfile.close();
}

Article readArt(string art_dir, string art_name){
	ifstream myfile (art_dir);
	string line;
	string title, author, text;
	if (myfile.is_open())
	{
		while (getline (myfile,line))
		{
			if(title.length() == 0){
				title = line;
			}
			else if(author.length() == 0)
				author = line;
			else{
				text += line + "\n";
			}
		}
		myfile.close();
	}
	else cout << "Unable to open file";

	return Article(title, author, text, stoi(art_name));

}

void create_newsgroup(const char* s){
	string h = getenv("HOME");
	string home = h + "/newsgroups/";
	mkdir(home.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir((home + s).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void deleteArt(const char* path){
	cout << path << endl;
	if(remove(path) != 0)
		cout << "Error deleting file" << endl;
	else
		cout << "File successfully deleted" << endl;
}

void init_vector(vector<Newsgroup>& ngs) {
	string h = getenv("HOME");
	string home = h + "/newsgroups/";
	auto dir = opendir(home.c_str());
	if (dir == NULL) return; // no newsgroups folder
	int i = 0;
	int ngcounter = 0;
	while (auto d = readdir(dir)) {
		++i;
		if (i < 3) continue; // ignoring systemfolders . and .. 
		string ng_name = d->d_name;
		string ng_dir_name = home + ng_name;
		Newsgroup ng(++ngcounter, ng_name);
		auto ng_dir = opendir(ng_dir_name.c_str());
		int j = 0;
		while (auto a = readdir(ng_dir)) {
			++j;
			if (j < 3) continue; // ignoring systemfolders . and .. 
			string art_name = a->d_name;
			string art_dir = ng_dir_name + "/" + art_name;
			ng.get_articles().push_back(readArt(art_dir, art_name));
			++ng.get_artcounter();
		}
		closedir(ng_dir);
		ngs.push_back(ng);
	}
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
	string h = getenv("HOME");
	vector<Newsgroup> ngs;
	init_vector(ngs); //reads from filesystem and imports
	ComServer cs(port, ngs);
	if (!cs.isInitialized()) {
		cerr << "Server initialization error." << endl;
		exit(1);
	}

	while (true) {
		cs.handleActivity();
		unsigned int last_changed = cs.last_modified();
		if (last_changed != 0) {
			string home = h + "/newsgroups/";
			if (!ngs.empty()) {
				auto it = find_if(ngs.begin(), ngs.end(), [last_changed](Newsgroup& n) {return n.get_id() == last_changed;});
				if (it != ngs.end()) {
					auto articles = it->get_articles();
					for(Article a : articles){
						deleteArt((home + it->get_name() + "/" + to_string(a.get_id())).c_str());
					}
					rmdir((home + it->get_name()).c_str());
				}
			}
			ngs = cs.get_grps();
			auto it = find_if(ngs.begin(), ngs.end(), [last_changed](Newsgroup& n) {return n.get_id() == last_changed;});
			if (it != ngs.end()) {

				auto articles = it->get_articles();
				create_newsgroup(it->get_name().c_str());
				home += it->get_name() + "/";
				for(Article a : articles){
					createArt(a.get_title(), a.get_author(), a.get_text(), a.get_id(), home);
				}
			}
		}
	}


	return 0;
}
