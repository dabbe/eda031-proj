
#include "server.h"
#include "protocol.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "newsgroup.h"
#include "messagehandler.cc"

#include <algorithm>
#include <sstream>
#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>

using namespace std;

vector<Newsgroup> newsgroups;

vector<Newsgroup>& get_list(){
	return newsgroups;
}

bool newsgroup_exists(string& ng){
	vector<Newsgroup> ngs = get_list();
	return find_if(ngs.begin(), ngs.end(), [ng](Newsgroup& x){return x.get_name() == ng;}) != ngs.end();
}

bool newsgroup_id_exists(unsigned int& id){
	vector<Newsgroup> ngs = get_list();
	return find_if(ngs.begin(),  ngs.end(), [group_id](Newsgroup& x){return x.get_id() == group_id;}) != ngs.end();
					
}

void save_newsgroup(string& ng){
	vector<Newsgroup> ngs = get_list();
	unsigned int id = 0;
	for(Newsgroup ng : ngs){
		id = max(id, ng.get_id());
	}
	ngs.push_back(Newsgroup(++id, ng));
}

void remove_newsgroup(unsigned int& i){
	auto it = remove_if(ngs.begin(), ngs.end(), [nbr](Newsgroup& x){return x.get_id() == nbr;});
	if(it != ngs.end()){
		get_list().erase(it);
		return true;
	}
	return false;
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
	
	Server server(port);
	if (!server.isReady()) {
		cerr << "Server initialization error." << endl;
		exit(1);
	}

	while (true) {
		auto conn = server.waitForActivity();
		if (conn != nullptr) {
			try {
				unsigned char command = conn->read();
				switch(command){
				case Protocol::COM_LIST_NG: {
					if(conn->read() != Protocol::COM_END){
						//exit or disc
						cout << "ngt weird" << endl;
					}
					conn->write(Protocol::ANS_LIST_NG);
					
					vector<Newsgroup> ngs = get_list();
					write_number(conn, ngs.size());
					for(Newsgroup ng : ngs){
						write_number(conn, ng.get_id());
						write_string(conn, ng.get_name());
					}
					conn->write(Protocol::ANS_END);
					break;
				}
				case Protocol::COM_CREATE_NG:
				{
					if(conn->read() != Protocol::PAR_STRING){
						//exit / disconnect client
						cout << "weird 2";
					}
					string ng = read_string(conn);
					if(conn->read() != Protocol::COM_END){
						// exit or disconnect client
						cout << "weird 1";
					}
					conn->write(Protocol::ANS_CREATE_NG);

					if(newsgroup_exists(ng)){
						conn->write(Protocol::ANS_NAK);
						conn->write(Protocol::ERR_NG_ALREADY_EXISTS);
					}else{
						save_newsgroup(ng);
						conn->write(Protocol::ANS_ACK);
					}
					conn->write(Protocol::ANS_END);
					break;
				}
				case Protocol::COM_DELETE_NG:
				{
					if(conn->read() != Protocol::PAR_NUM){
						cout << "ngt konstigt1";
					}
					unsigned int nbr = read_number(conn);
					if(conn->read() != Protocol::COM_END){
						cout << "ngt konstigt2";
					}
					conn->write(Protocol::ANS_DELETE_NG);

					vector<Newsgroup> ngs = get_list();
					if(remove_newsgroup(nbr)){
						conn->write(Protocol::ANS_ACK);
					} else{
						conn->write(Protocol::ANS_NAK);
						conn->write(Protocol::ERR_NG_DOES_NOT_EXIST);
					}
					conn->write(Protocol::ANS_END);
					break;
				}
				case Protocol::COM_LIST_ART:
				{
					if(conn->read() != Protocol::PAR_NUM){
						//exit or disc
						cout << "weiridid";
					}
					unsigned int group_id = read_number(conn);
					if(conn->read() != Protocol::COM_END){
						cout << "weird09";
					}
					conn->write(Protocol::ANS_LIST_ART);

					if(newsgroup_id_exists(group_id)){
						conn->write(Protocol::ANS_ACK);
						Newsgroup group = *it;
						vector<Article> articles = group.get_articles();
						write_number(conn, articles.size());
						for(Article a : articles){
							write_number(conn, a.get_id());
							write_string(conn, a.get_title());
						}
					} else{
						conn->write(Protocol::ANS_NAK);
						conn->write(Protocol::ERR_NG_DOES_NOT_EXIST);
					}

					conn->write(Protocol::ANS_END);
				}
				break;
				case Protocol::COM_CREATE_ART:
				{
					if(conn->read() != Protocol::PAR_NUM){
						cout << "konstigt3";
					}
					unsigned int id = read_number(conn);
					if(conn->read() != Protocol::PAR_STRING){
						cout << "konstigt4";
					}
					string title = read_string(conn);
					if(conn->read() != Protocol::PAR_STRING){
						cout << "konstigt5";
					}
					string author = read_string(conn);
					if(conn->read() != Protocol::PAR_STRING){
						cout << "konstigt6";
					}
					string text = read_string(conn);
					if(conn->read() != Protocol::COM_END){
						cout << "konstigt";
					}
					conn->write(Protocol::ANS_CREATE_ART);
					auto it = find_if(ngs.begin(), ngs.end(), [id](Newsgroup& x){return x.get_id() == id;});
					if(it != ngs.end()){
						Newsgroup& group = *it;
						unsigned int id = 0;
						for(Article a : it->get_articles()){
							id = max(id, a.get_id());
						}
						it->get_articles().push_back(Article(title, author, text, ++id));
						conn->write(Protocol::ANS_ACK);
					} else{
						conn->write(Protocol::ANS_NAK);
						conn->write(Protocol::ERR_NG_DOES_NOT_EXIST);
					}
					conn->write(Protocol::ANS_END);

				}
				break;
				case Protocol::COM_DELETE_ART:
				{
					if(conn->read() != Protocol::PAR_NUM){
						cout << "konstigt7";
					}
					unsigned int group_nbr = read_number(conn);
					if(conn->read() != Protocol::PAR_NUM){
						cout << "konstigt8";
					}
					unsigned int article_nbr = read_number(conn);
					if(conn->read() != Protocol::COM_END){
						cout << "konstigt9";
					}
					conn->write(Protocol::ANS_DELETE_ART);
					auto it = find_if(ngs.begin(), ngs.end(), [group_nbr](Newsgroup& ng){return ng.get_id() == group_nbr;});
					if(it != ngs.end()){
						auto it2 = remove_if(it->get_articles().begin(), it->get_articles().end(), [article_nbr](Article& a){return a.get_id() == article_nbr;});
						if(it2 != it->get_articles().end()){
							conn->write(Protocol::ANS_ACK);
							it->get_articles().erase(it2);
						} else{
							conn->write(Protocol::ANS_NAK);
							conn->write(Protocol::ERR_ART_DOES_NOT_EXIST);
						}
					} else{
						conn->write(Protocol::ANS_NAK);
						conn->write(Protocol::ERR_NG_DOES_NOT_EXIST);
					}
					conn->write(Protocol::ANS_END);
				}
				break;
				case Protocol::COM_GET_ART:
				{
					if(conn->read() != Protocol::PAR_NUM){
						cout << "konstigt10";
					}
					unsigned int group_id = read_number(conn);
					if(conn->read() != Protocol::PAR_NUM){
						cout << "konstigt11";
					}
					unsigned int article_id = read_number(conn);
					conn->write(Protocol::ANS_GET_ART);
					auto it = find_if(ngs.begin(), ngs.end(), [group_id](Newsgroup& ng){return ng.get_id() == group_id;});
					if(it != ngs.end()){
						auto it2 = find_if(it->get_articles().begin(), it->get_articles().end(), [article_id](Article& a){return a.get_id() == article_id;});
						if(it2 != it->get_articles().end()){
							conn->write(Protocol::ANS_ACK);
							write_string(conn, it2->get_title());
							write_string(conn, it2->get_author());
							write_string(conn, it2->get_text());
						} else{
							conn->write(Protocol::ANS_NAK);
							conn->write(Protocol::ERR_ART_DOES_NOT_EXIST);
						}
					} else{
						conn->write(Protocol::ANS_NAK);
						conn->write(Protocol::ERR_NG_DOES_NOT_EXIST);
					}
					conn->write(Protocol::ANS_END);
					break;
				}
				}

			} catch (ConnectionClosedException&) {
				server.deregisterConnection(conn);
				cout << "Client closed connection" << endl;
			}
		} else {
			conn = make_shared<Connection>();
			server.registerConnection(conn);
			cout << "New client connects" << endl;
		}
	}
}
