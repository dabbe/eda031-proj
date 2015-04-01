/* myserver.cc: sample server program */
#include "server.h"
#include "protocol.h"
#include "connection.h"
#include "connectionclosedexception.h"

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
	conn->write('$');
}

void write_number(const shared_ptr<Connection>& conn,  const int& i){
	conn->write(Protocol::PAR_NUM);
	conn->write(i);
}

string read_string(const shared_ptr<Connection>& conn){
	unsigned char n = read_number(conn);
	stringstream ss;
	cout << "n is: " << n << endl;
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
	
	while (true) {
		auto conn = server.waitForActivity();
		if (conn != nullptr) {
			try {
//				int nbr = readNumber(conn);
//				struct Protocol protocol;
				cout << "First byte is: ";
				unsigned char a = conn->read();
				switch(a){
				case Protocol::COM_LIST_NG:
					if(conn->read() != Protocol::COM_END){
						//exit or disc
					}
					write_number(conn, Protocol::ANS_LIST_NG);
					//get list
				break;
				case Protocol::COM_CREATE_NG:
					cout << "creating newsgroup" << endl;
					if(conn->read() != Protocol::PAR_STRING){
						//exit / disconnect client
					}
					cout << "string is" + read_string(conn) << endl;
					if(conn->read() != Protocol::COM_END){
						// exit or disconnect client
					}
					//create group
				break;
				case Protocol::COM_DELETE_NG:
				{
					if(conn->read() != Protocol::PAR_NUM){
							//exit or disc
					}
					int del_number = read_number(conn);
					if(conn->read() != Protocol::COM_END){
					}
					write_number(conn, Protocol::ANS_DELETE_NG);
					//delete group
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
					write_number(conn, Protocol::ANS_LIST_ART);
					// list articles
				}
				break;
				case Protocol::COM_CREATE_ART:
					if(conn->read() != Protocol::PAR_NUM){}
					int id = read_number(conn);
					if(conn->read() != Protocol::PAR_STRING){}
					string title = read_string(conn);
					if(conn->read() != Protocol::PAR_STRING){}
					string author = read_string(conn);
					if(conn->read() != Protocol::PAR_STRING){}
					string text = read_string(conn);
					if(conn->read() != Protocol::COM_END){}
					write_number(conn, Protocol::ANS_CREATE_ART);

				break;
				case Protocol::COM_DELETE_ART:
				break;
				case Protocol::COM_GET_ART:
				break;
				case Protocol::COM_END:
				break;
				}

//				cout << "Hello" << endl;
//				cout << nbr << endl;
//				cout <<  "Bye" << endl;

//				cout << "nbr " << nbr << endl << endl;

//				string result;
/*				if (nbr > 0) {
					result = "positive";
				} else if (nbr == 0) {
					result = "zero";
				} else {*/
//					result = Protocol::ANS_CREATE_NG;
//				}
//				writeString(conn, result);
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
