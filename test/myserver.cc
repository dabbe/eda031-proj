
/* myserver.cc: sample server program */ 
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

void protocol_err(){
	cerr << "Protocol error, shutting down connection" << endl;
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
	vector<Newsgroup> ngs;
	unsigned int ngcounter = 0;
	
	while (true) {
		auto conn = server.waitForActivity();
		if (conn != nullptr) {
			try {
				unsigned char command = conn->read();
				switch(command){
				case Protocol::COM_LIST_NG: {
					if(conn->read() != Protocol::COM_END){
						protocol_err();
						server.deregisterConnection(conn);
					}
					conn->write(Protocol::ANS_LIST_NG);

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
						protocol_err();
						server.deregisterConnection(conn);
					}
					string ng = read_string(conn);
					if(conn->read() != Protocol::COM_END){
						protocol_err();
						server.deregisterConnection(conn);
					}
					conn->write(Protocol::ANS_CREATE_NG);
					if(find_if(ngs.begin(), ngs.end(), [ng](Newsgroup& x){return x.get_name() == ng;}) != ngs.end()){
						conn->write(Protocol::ANS_NAK);
						conn->write(Protocol::ERR_NG_ALREADY_EXISTS);
					} else{
						ngs.push_back(Newsgroup(++ngcounter, ng));
						conn->write(Protocol::ANS_ACK);
					}
					conn->write(Protocol::ANS_END);
					break;
				}
				case Protocol::COM_DELETE_NG:
				{
					if(conn->read() != Protocol::PAR_NUM){
						protocol_err();
						server.deregisterConnection(conn);
					}
					unsigned int nbr = read_number(conn);
					if(conn->read() != Protocol::COM_END){
						protocol_err();
						server.deregisterConnection(conn);
					}
					conn->write(Protocol::ANS_DELETE_NG);
					auto it = remove_if(ngs.begin(), ngs.end(), [nbr](Newsgroup& x){return x.get_id() == nbr;});
					if(it != ngs.end()){
						ngs.erase(it);
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
						protocol_err();
						server.deregisterConnection(conn);
					}
					unsigned int group_id = read_number(conn);
					if(conn->read() != Protocol::COM_END){
						protocol_err();
						server.deregisterConnection(conn);
					}
					conn->write(Protocol::ANS_LIST_ART);
					auto it = find_if(ngs.begin(),  ngs.end(), [group_id](Newsgroup& x){return x.get_id() == group_id;});
					if(it != ngs.end()){
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
						protocol_err();
						server.deregisterConnection(conn);
					}
					unsigned int id = read_number(conn);
					if(conn->read() != Protocol::PAR_STRING){
						protocol_err();
						server.deregisterConnection(conn);
					}
					string title = read_string(conn);
					if(conn->read() != Protocol::PAR_STRING){
						protocol_err();
						server.deregisterConnection(conn);
					}
					string author = read_string(conn);
					if(conn->read() != Protocol::PAR_STRING){
						protocol_err();
						server.deregisterConnection(conn);
					}
					string text = read_string(conn);
					if(conn->read() != Protocol::COM_END){
						protocol_err();
						server.deregisterConnection(conn);
					}
					conn->write(Protocol::ANS_CREATE_ART);
					auto it = find_if(ngs.begin(), ngs.end(), [id](Newsgroup& x){return x.get_id() == id;});
					if(it != ngs.end()){
						it->get_articles().push_back(Article(title, author, text, ++(it->get_artcounter())));
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
						protocol_err();
						server.deregisterConnection(conn);
					}
					unsigned int group_nbr = read_number(conn);
					if(conn->read() != Protocol::PAR_NUM){
						protocol_err();
						server.deregisterConnection(conn);
					}
					unsigned int article_nbr = read_number(conn);
					if(conn->read() != Protocol::COM_END){
						protocol_err();
						server.deregisterConnection(conn);
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
						protocol_err();
						server.deregisterConnection(conn);	
					}
					unsigned int group_id = read_number(conn);
					if(conn->read() != Protocol::PAR_NUM){
						protocol_err();
						server.deregisterConnection(conn);
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
