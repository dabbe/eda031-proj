/* myclient.cc: sample client program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "messagehandler.cc"

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

using namespace std;

void list_alternatives(){
	cout << endl << endl;
	cout << "Choose an alternative:" << endl;
	cout << "1. List newsgroup" << endl;
	cout << "2. Create newsgroup" << endl;
	cout << "3. Delete newsgroup" << endl;
	cout << "4. List articles in newsgroup" << endl;
	cout << "5. Create article" << endl;
	cout << "6. Delete article" << endl;
	cout << "7. Get article" << endl << endl;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		cerr << "Usage: myclient host-name port-number" << endl;
		exit(1);
	}

	int port = -1;
	try {
		port = stoi(argv[2]);
	} catch (exception& e) {
		cerr << "Wrong port number. " << e.what() << endl;
		exit(1);
	}

	Connection conn(argv[1], port);
	if (!conn.isConnected()) {
		cerr << "Connection attempt failed" << endl;
		exit(1);
	}

	list_alternatives();
	int alternative;
	while (cin >> alternative) {
		try {
			cout << endl;
			switch(alternative){
			case 1: {
				cout << "List newsgroup:" << endl;
				conn.write(Protocol::COM_LIST_NG);
				conn.write(Protocol::COM_END);

				if(conn.read() != Protocol::ANS_LIST_NG){
					cout << "konstitttt";
				}
				if(conn.read() != Protocol::PAR_NUM){
					cout << "ngt konstigt123";
				}
				unsigned int nbr = read_n(conn);
				for(unsigned int i = 0; i != nbr; ++i){
					if(conn.read() != Protocol::PAR_NUM){
						cout << "lide konstigt";
					}
					unsigned int id = read_n(conn);
					if(conn.read() != Protocol::PAR_STRING){
						cout << "konstet";
					}
					string s = read_s(conn);
					cout << id << ". " + s << endl;
				}
				if(conn.read() != Protocol::ANS_END){
					cout << "konstit1";
				}
				cout << endl << endl;
			break;
			}
			case 2: {
				cout << "Create newsgroup:" << endl;
				cout << "Type a group name: ";
				conn.write(Protocol::COM_CREATE_NG);
				string name;
				cin.ignore();
				getline(cin, name);
				cout << "name is " + name << endl << endl;
				write_s(conn, name);
				conn.write(Protocol::COM_END);

				if(conn.read() != Protocol::ANS_CREATE_NG){
					cout << "ngt weird1";
				}
				unsigned char c = conn.read();
				switch(c){
				case Protocol::ANS_NAK:
					if(conn.read() != Protocol::ERR_NG_ALREADY_EXISTS){
						cout << "nagat fel";
					}
				case Protocol::ANS_ACK:
				break;
				default:
					cout << "wadafuk";
				break;
				}
				if(conn.read() != Protocol::ANS_END){
					cout << "hmm???";
				}
				break;
			}
			case 3:
				cout << "Delete newsgroup:" << endl;
				
			break;
			case 4:
				cout << "List articles in newsgroup:" << endl;
			break;
			case 5:
				cout << "Create article:" << endl;
			break;
			case 6:
				cout << "Delete article:" << endl;
			break;
			case 7:
				cout << "Get article:" << endl;
			break;
			default:
				cout << "Command " << alternative <<" does not exist" << endl;
			}
			/*write_number(conn, nbr);
			string reply = read_string(conn);
			cout << " " << reply << endl;*/
			list_alternatives();
		} catch (ConnectionClosedException&) {
			cout << " no reply from server. Exiting." << endl;
			exit(1);
		}
	}
}

