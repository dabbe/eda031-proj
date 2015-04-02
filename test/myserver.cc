/* myserver.cc: sample server program */ 
#include "server.h"
#include "protocol.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "newsgroup.h"

#include <algorithm>
#include <sstream>
#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>

using namespace std;

/*
 * Read an integer from a client.
 */
int read_number(const shared_ptr<Connection>& conn) {
	unsigned char byte1 = conn->read();
	unsigned char byte2 = conn->read();
	unsigned char byte3 = conn->read();
	unsigned char byte4 = conn->read();
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

/*
 * Send a string to a client.
 */
void write_string(const shared_ptr<Connection>& conn, const string& s) {
	for (char c : s) {
		conn->write(c);
	}
//	conn->write('$');
}

void write_int(const shared_ptr<Connection>& conn, const int& i){
	conn->write((i >> 24) & 0xFF);
	conn->write((i >> 16) && 0xFF);
	conn->write((i >> 8) && 0xFF);
	conn->write(i && 0xFF);
}
void write_number(const shared_ptr<Connection>& conn, const int& i){
	conn->write(Protocol::PAR_NUM);
	write_int(conn, i);
}

string read_string(const shared_ptr<Connection>& conn){
	unsigned char n = read_number(conn);
	stringstream ss;
	for(int i = 0; i != n; ++i){
		unsigned char c = conn->read();
		ss << c;
	}
	return ss.str();
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
	int ng_counter = -1;

	while (true) {
		auto conn = server.waitForActivity();
		if (conn != nullptr) {
			try {
				unsigned char a = conn->read();
				switch(a){
				case Protocol::COM_LIST_NG:
					cout << "com list ng" << endl;
					if(conn->read() != Protocol::COM_END){
						//exit or disc
						cout << "ngt weird" << endl;
					}
					write_int(conn, Protocol::ANS_LIST_NG);
					write_number(conn, 0);
					/*write_number(conn, ngs.size());
					for(Newsgroup ng : ngs){
						write_number(conn, ng.get_id());
						write_string(conn, ng.get_name());
					}*/
					write_int(conn, Protocol::ANS_END);
				break;
				case Protocol::COM_CREATE_NG:
				{
					if(conn->read() != Protocol::PAR_STRING){
						//exit / disconnect client
					}
					string ng = read_string(conn);
					if(conn->read() != Protocol::COM_END){
						// exit or disconnect client
					}
					write_int(conn, Protocol::ANS_CREATE_NG);
					if(find_if(ngs.begin(), ngs.end(), [ng](Newsgroup& x){return x.get_name() == ng;}) != ngs.end()){
						write_int(conn, Protocol::ANS_NAK);
						write_int(conn, Protocol::ERR_NG_ALREADY_EXISTS);
					} else{
						ngs.push_back(Newsgroup(++ng_counter, ng));
						write_int(conn, Protocol::ANS_ACK);
					}
					write_int(conn, Protocol::ANS_END);

					cout << "size aer " << ngs.size() << endl;
					break;
				}
				case Protocol::COM_DELETE_NG:
				{
					if(conn->read() != Protocol::PAR_NUM){
							//exit or disc
					}
					int nbr = read_number(conn);
					if(conn->read() != Protocol::COM_END){
					}
					write_int(conn, Protocol::ANS_DELETE_NG);
					auto it = remove_if(ngs.begin(), ngs.end(), [nbr](Newsgroup& x){return x.get_id() == nbr;});
					if(it != ngs.end()){
						ngs.erase(it);
						write_int(conn, Protocol::ANS_ACK);
					} else{
						write_int(conn, Protocol::ANS_NAK);
						write_int(conn, Protocol::ERR_NG_DOES_NOT_EXIST);
					}
					write_int(conn, Protocol::ANS_END);
					break;
				}
				case Protocol::COM_LIST_ART:
				{
					if(conn->read() != Protocol::PAR_NUM){
						//exit or disc
					}
					int list_number = read_number(conn);
					if(conn->read() != Protocol::COM_END){
					}
					write_int(conn, Protocol::ANS_LIST_ART);
					// list articles
					write_int(conn, Protocol::ANS_END);
				}
				break;
				case Protocol::COM_CREATE_ART:
				{
					if(conn->read() != Protocol::PAR_NUM){}
					int id = read_number(conn);
					if(conn->read() != Protocol::PAR_STRING){}
					string title = read_string(conn);
					if(conn->read() != Protocol::PAR_STRING){}
					string author = read_string(conn);
					if(conn->read() != Protocol::PAR_STRING){}
					string text = read_string(conn);
					if(conn->read() != Protocol::COM_END){}
					write_int(conn, Protocol::ANS_CREATE_ART);
					//create shiet
					write_int(conn, Protocol::ANS_END);

				}
				break;
				case Protocol::COM_DELETE_ART:
				{
					if(conn->read() != Protocol::PAR_NUM){}
					int group_nbr = read_number(conn);
					if(conn->read() != Protocol::PAR_NUM){}
					int article_nbr = read_number(conn);
					if(conn->read() != Protocol::COM_END){}
					write_int(conn, Protocol::ANS_DELETE_ART);
					write_int(conn, Protocol::ANS_END);
				}
				break;
				case Protocol::COM_GET_ART:
				{
					if(conn->read() != Protocol::PAR_NUM){}
					int group_id = read_number(conn);
					if(conn->read() != Protocol::PAR_NUM){}
					int article_id = read_number(conn);
					write_int(conn, Protocol::ANS_GET_ART);
					// do all sorts of stuff
					write_int(conn, Protocol::ANS_END);
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
