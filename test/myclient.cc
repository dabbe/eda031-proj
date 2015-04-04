/* myclient.cc: sample client program */
#include "connection.h"
#include "connectionclosedexception.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>

using namespace std;

/*
 * Send an integer to the server as four bytes.
 */
void write_number(const Connection& conn, int value) {
	conn.write((value >> 24) & 0xFF);
	conn.write((value >> 16) & 0xFF);
	conn.write((value >> 8)	 & 0xFF);
	conn.write(value & 0xFF);
}

/*
 * Read a string from the server.
 */
string read_string(const Connection& conn) {
	string s;
	char ch;
	while ((ch = conn.read()) != '$') {
		s += ch;
	}
	return s;
}

void list_alternatives(){
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
			case 1:
				cout << "List newsgroup:" << endl;
			break;
			case 2:
				cout << "Create newsgroup:" << endl;
			break;
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

