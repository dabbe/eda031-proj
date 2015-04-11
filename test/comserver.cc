#include "protocol.h"
#include "connectionclosedexception.h"
#include "messagehandler.cc"
#include "comserver.h"

#include <memory>
#include <iostream>
#include <algorithm>
#include <sys/types.h>	 /* socket(), bind(), select() */
#include <sys/socket.h>	 /* socket(), bind(), getsockname(), listen() */
#include <arpa/inet.h>	 /* htons(), ntohs() */
#include <unistd.h>		 /* close(), select() */
#include <sys/time.h>	 /* select() */
#include <netinet/in.h>	 /* sockaddr_in */

using namespace std;

ComServer::ComServer(int port) : Server(port), ngcounter(0) {}

ComServer::ComServer(int port, vector<Newsgroup> grps) : Server(port), ngs(grps), ngcounter(grps.size()) {}

void ComServer::handleActivity() {
	last_changed = -1;
	auto conn = waitForActivity();
	if(conn != nullptr) {
		try {
			unsigned char command = conn->read();
			switch(command) {
				case Protocol::COM_LIST_NG: {
					if (conn->read() != Protocol::COM_END) {
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);
					} else {
						conn->write(Protocol::ANS_LIST_NG);
						write_number(conn, ngs.size());
						for (Newsgroup ng : ngs) {
							write_number(conn, ng.get_id());
							write_string(conn, ng.get_name());
						}
						conn->write(Protocol::ANS_END);
					}
					last_changed = -1;
					break;
				}
				case Protocol::COM_CREATE_NG: {
					if (conn->read() != Protocol::PAR_STRING) {
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);
					} else {
						string ng = read_string(conn);
						if(conn->read() != Protocol::COM_END){
							protocol_err("Client is not following communication protocol");
							deregisterConnection(conn);
						} else {
							conn->write(Protocol::ANS_CREATE_NG);
							if(find_if(ngs.begin(), ngs.end(), [ng](Newsgroup& x){return x.get_name() == ng;}) != ngs.end()){
								conn->write(Protocol::ANS_NAK);
								conn->write(Protocol::ERR_NG_ALREADY_EXISTS);
							} else {
								Newsgroup n = Newsgroup(++ngcounter, ng);
								ngs.push_back(n);
								conn->write(Protocol::ANS_ACK);
								last_changed = n.get_id();
							}
							conn->write(Protocol::ANS_END);
						}
					}
					break;
				}
				case Protocol::COM_DELETE_NG: {
					if(conn->read() != Protocol::PAR_NUM){
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);
					} else {
						unsigned int nbr = read_number(conn);
						if(conn->read() != Protocol::COM_END){
							protocol_err("Client is not following communication protocol");
							deregisterConnection(conn);
						} else {
							conn->write(Protocol::ANS_DELETE_NG);
							auto it = remove_if(ngs.begin(), ngs.end(), [nbr](Newsgroup& x){return x.get_id() == nbr;});
							if(it != ngs.end()){
								last_changed = it->get_id();
								ngs.erase(it);
								conn->write(Protocol::ANS_ACK);
							} else {
								conn->write(Protocol::ANS_NAK);
								conn->write(Protocol::ERR_NG_DOES_NOT_EXIST);
							}
							conn->write(Protocol::ANS_END);
						}
					}
					break;
				}		
				case Protocol::COM_LIST_ART: {
					if(conn->read() != Protocol::PAR_NUM){
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);
					} else {
						unsigned int group_id = read_number(conn);
						if(conn->read() != Protocol::COM_END){
							protocol_err("Client is not following communication protocol");
							deregisterConnection(conn);
						} else {
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
					}
					last_changed = -1;
					break;
				}
				case Protocol::COM_CREATE_ART: {
					if(conn->read() != Protocol::PAR_NUM){
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);
						break;
					}
					unsigned int id = read_number(conn);
					if(conn->read() != Protocol::PAR_STRING){
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);
						break;
					}
					string title = read_string(conn);
					if(conn->read() != Protocol::PAR_STRING){
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);
						break;
					}
					string author = read_string(conn);
					if(conn->read() != Protocol::PAR_STRING){
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);
						break;
					}
					string text = read_string(conn);
					if(conn->read() != Protocol::COM_END){
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);
						break;
					}
					conn->write(Protocol::ANS_CREATE_ART);
					auto it = find_if(ngs.begin(), ngs.end(), [id](Newsgroup& x){return x.get_id() == id;});
					if(it != ngs.end()){
						last_changed = it->get_id();
						it->get_articles().push_back(Article(title, author, text, ++(it->get_artcounter())));
						conn->write(Protocol::ANS_ACK);
					} else{
						conn->write(Protocol::ANS_NAK);
						conn->write(Protocol::ERR_NG_DOES_NOT_EXIST);
					}
					conn->write(Protocol::ANS_END);
					break;
				}
				case Protocol::COM_DELETE_ART: {
					if(conn->read() != Protocol::PAR_NUM){
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);
						break;
					}
					unsigned int group_nbr = read_number(conn);
					if(conn->read() != Protocol::PAR_NUM){
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);
						break;
					}
					unsigned int article_nbr = read_number(conn);
					if(conn->read() != Protocol::COM_END){
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);
						break;
					}
					conn->write(Protocol::ANS_DELETE_ART);
					auto it = find_if(ngs.begin(), ngs.end(), [group_nbr](Newsgroup& ng){return ng.get_id() == group_nbr;});
					if(it != ngs.end()){
						last_changed = it->get_id();
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
					break;
				}
				case Protocol::COM_GET_ART: {
					if(conn->read() != Protocol::PAR_NUM){
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);	
					}
					unsigned int group_id = read_number(conn);
					if(conn->read() != Protocol::PAR_NUM){
						protocol_err("Client is not following communication protocol");
						deregisterConnection(conn);
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
					last_changed = -1;
					break;
				}
			}
		} catch (ConnectionClosedException&) {
			deregisterConnection(conn);
			cout << "Client closed connection" << endl;
		}
	} else {
		conn = make_shared<Connection>();
		registerConnection(conn);
		cout << "New client connects" << endl;
	}	
}

int ComServer::last_modified() {
	return last_changed;
}

void ComServer::protocol_err(string err) {
	cerr << err << endl;
}

vector<Newsgroup> ComServer::get_grps() {
	return ngs;
}

bool ComServer::isInitialized() const {
	return isReady();
}
