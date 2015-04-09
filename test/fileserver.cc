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
//	cout << "Status for creating root-newsgroup folder: " << s_one << endl;
//	cout << "Status for creating newsgroup folder: " << s_two << endl;
}

void deleteArt(const char* path){
	cout << path << endl;
	if(remove(path) != 0)
		cout << "Error deleting file" << endl;
	else
		cout << "File successfully deleted" << endl;
}

void process_file(string file, string path){
	cout << "Artikel med id: " << file.c_str() << endl;
	cout << "Path" << path << endl;
}

void process_entity(struct dirent* entity, string path)
{
    /*if(entity->d_type == DT_DIR){
        if(entity->d_name[0] == '.'){
            return;
        }
        process_directory(string(entity->d_name));
        return;
    }*/

    if(entity->d_type == DT_REG){
        process_file(string(entity->d_name), path + entity->d_name);
        return;
    }
}

void process_directory(string directory){
	string h = getenv("HOME");
	string home = h + "/newsgroups/";
	string dirToOpen = home + directory;
    auto dir = opendir(dirToOpen.c_str());
    string path = dirToOpen + "/";
    if(NULL == dir){
        return;
    }

    auto entity = readdir(dir);
    while(entity != NULL){
        process_entity(entity, path);
        entity = readdir(dir);
    }

    path.resize(path.length() - 1 - directory.length());
    closedir(dir);
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
	
	ComServer cs(port);

	if (!cs.isInitialized()) {
		cerr << "Server initialization error." << endl;
		exit(1);
	}
	string h = getenv("HOME");
	vector<Newsgroup> ngs; // LÄSA IN ALLt

	while (true) {
		cs.handleActivity();
		int ns = cs.last_modified();

		if (ns != -1) {

			string home = h + "/newsgroups/";
			if (!ngs.empty()) {
				auto it = find_if(ngs.begin(), ngs.end(), [&ns](Newsgroup& n) {return n.get_id() == ns;});
				
				//process_directory(it->get_name());
				//ta bort alla artiklar i mappen
				if (it != ngs.end()) {
					auto articles = it->get_articles();
					for(Article a : articles){
						deleteArt((home + it->get_name() + "/" + to_string(a.get_id())).c_str());
					}
					//rmdir((home + it->get_name()).c_str());
				}
			}
			ngs = cs.get_grps();
			auto it = find_if(ngs.begin(), ngs.end(), [&ns](Newsgroup& n) {return n.get_id() == ns;});
			auto articles = it->get_articles();
			create_newsgroup(it->get_name().c_str());
			home += it->get_name() + "/";
			for(Article a : articles){
				createArt(a.get_title(), a.get_author(), a.get_text(), a.get_id(), home);
			}
		}
		

		// Skriv grupperna till fil
	}


	return 0;
}