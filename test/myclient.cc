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
	}
	catch (exception& e) {
		cerr << "Wrong port number. " << e.what() << endl;
		exit(1);
	}

	Connection conn(argv[1], port);
	if (!conn.isConnected()) {
		cerr << "Connection attempt failed" << endl;
		exit(1);
	}

	int alternative;
	while (1) {
		list_alternatives();
		cin >> alternative;
		if (cin.fail()){
			cout << "Command " << alternative << " does not exist" << endl;
		}
		else{
			try {
				cout << endl;
				switch (alternative){
				case 1: {
					cout << "List newsgroup:" << endl;
					conn.write(Protocol::COM_LIST_NG);
					conn.write(Protocol::COM_END);

					if (conn.read() != Protocol::ANS_LIST_NG){
						cout << "Server Error" << endl;
						break;
					}
					if (conn.read() != Protocol::PAR_NUM){
						cout << "Server Error" << endl;
						break;
					}
					unsigned int nbr = read_n(conn);
					for (unsigned int i = 0; i != nbr; ++i){
						if (conn.read() != Protocol::PAR_NUM){
							cout << "Server Error" << endl;
							break;
						}
						unsigned int id = read_n(conn);
						if (conn.read() != Protocol::PAR_STRING){
							cout << "Server Error" << endl;
							break;
						}
						string s = read_s(conn);
						cout << id << ". " + s << endl;
					}
					if (conn.read() != Protocol::ANS_END){
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
					write_s(conn, name);
					conn.write(Protocol::COM_END);

					if (conn.read() != Protocol::ANS_CREATE_NG){
						cout << "ngt weird1";
					}
					unsigned char c = conn.read();
					switch (c){
					case Protocol::ANS_NAK:
						if (conn.read() != Protocol::ERR_NG_ALREADY_EXISTS){
							cout << "ngt felililil";
						}
						else{
							cout << "Group already exists";
							break;
						}
					case Protocol::ANS_ACK:
						cout << "Group created" << endl;
						break;
					default:
						cout << "wadafuk";
						break;
					}
					if (conn.read() != Protocol::ANS_END){
						cout << "hmm???";
					}
					break;
				}
				case 3:{
					cout << "Delete newsgroup:" << endl;
					cout << "Type the group id you want to remove: ";
					conn.write(Protocol::COM_DELETE_NG);
					unsigned int id;
					cin >> id;
					if (cin.fail()){
						cout << "Wrong input format" << endl;
						break;
					}
					write_n(conn, id);
					conn.write(Protocol::COM_END);

					if (conn.read() != Protocol::ANS_DELETE_NG){
						cout << "smth weird";
					}
					unsigned char c = conn.read();
					switch (c){
					case Protocol::ANS_NAK:
						if (conn.read() != Protocol::ERR_NG_DOES_NOT_EXIST){
							cout << "nagat fel";
						}
						else{
							cout << "Group does not exist" << endl;
						}
					case Protocol::ANS_ACK:
						cout << "Group deleted" << endl;
						break;
					default:
						cout << "wadafuk";
						break;
					}
					if (conn.read() != Protocol::ANS_END){
						cout << "hmm???";
					}
					break;
				}
				case 4:{
					cout << "List articles in newsgroup:" << endl;
					cout << "NewsgroupId?: ";
					conn.write(Protocol::COM_LIST_ART);
					unsigned int id;
					cin >> id;
					if (cin.fail()){
						cout << "Wrong input format" << endl;
						break;
					}
					write_n(conn, id);
					conn.write(Protocol::COM_END);

					if (conn.read() != Protocol::ANS_LIST_ART){
						cout << "konstigtgt";
					}
					unsigned char c = conn.read();
					switch (c){
					case Protocol::ANS_NAK:
						if (conn.read() != Protocol::ERR_NG_DOES_NOT_EXIST){
							cout << "nagat fel";
						}
						else{
							cout << "Group does not exist" << endl;
						}
						break;
					case Protocol::ANS_ACK:{
						if (conn.read() != Protocol::PAR_NUM){
							cout << "ngt fel";
						}
						unsigned int n = read_n(conn);
						for (unsigned int i = 0; i != n; ++i){
							if (conn.read() != Protocol::PAR_NUM){
								cout << "lide konstigt";
							}
							unsigned int id = read_n(conn);
							if (conn.read() != Protocol::PAR_STRING){
								cout << "konstet";
							}
							string s = read_s(conn);
							cout << id << ". " + s << endl;
						}
						break;
					}
					default:
						cout << "wadafuk";
						break;
					}
					if (conn.read() != Protocol::ANS_END){
						cout << "hmm???";
					}
					break;
				}
				case 5:{
					conn.write(Protocol::COM_CREATE_ART);
					cout << "Create article:" << endl;
					cout << "ID?: ";
					unsigned int id;
					cin >> id;
					if (cin.fail()){
						cout << "Wrong input format" << endl;
						break;
					}
					write_n(conn, id);

					cout << "Title?: ";
					string title;
					cin.ignore();
					getline(cin, title);
					write_s(conn, title);

					cout << "Author?:";
					string author;
					getline(cin, author);
					write_s(conn, author);

					cout << "Text?:";
					string text;
					getline(cin, text);
					write_s(conn, text);

					conn.write(Protocol::COM_END);

					if (conn.read() != Protocol::ANS_CREATE_ART){
						cout << "konstigt";
					}
					unsigned int com = conn.read();
					switch (com){
					case Protocol::ANS_NAK:
						if (conn.read() != Protocol::ERR_NG_DOES_NOT_EXIST){
							cout << "???";
						}
						else{
							cout << "Newsgroup does not exist" << endl;
						}
						break;
					case Protocol::ANS_ACK:
						break;
					default:
						cout << "oh you done wrong";
						break;
					}
					if (conn.read() != Protocol::ANS_END){
						cout << "FEL";
					}
					break;
				}
				case 6:{
					cout << "Delete article:" << endl;
					cout << "What group id?: ";
					conn.write(Protocol::COM_DELETE_ART);
					unsigned int group_id;
					cin >> group_id;
					if (cin.fail()){
						cout << "Wrong input format" << endl;
						break;
					}
					write_n(conn, group_id);

					cout << "What article id?: ";
					unsigned int art_id;
					cin >> art_id;
					if (cin.fail()){
						cout << "Wrong input format" << endl;
						break;
					}
					write_n(conn, art_id);

					conn.write(Protocol::COM_END);

					if (conn.read() != Protocol::ANS_DELETE_ART){
						cout << "smth konstit";
					}
					unsigned int comm = conn.read();
					switch (comm){
					case Protocol::ANS_ACK:
						cout << "Article deleted" << endl;
						break;
					case Protocol::ANS_NAK:{
						unsigned int err = conn.read();
						switch (err){
						case Protocol::ERR_NG_DOES_NOT_EXIST:
							cout << "Group does not exist" << endl;
							break;
						case Protocol::ERR_ART_DOES_NOT_EXIST:
							cout << "Article does not exist!" << endl;
							break;
						default:
							cout << "now sometjhing is weird!";
							break;
						}
						break;
					}
					default:
						cout << "WRONG! " << comm;
					}
					if (conn.read() != Protocol::ANS_END){
						cout << "should be smth eleze here";
					}
					break;
				}
				case 7:{
					cout << "Get article:" << endl;
					cout << "Group id?: ";
					conn.write(Protocol::COM_GET_ART);
					unsigned int group_id;
					cin >> group_id;
					if (cin.fail()){
						cout << "Wrong input format" << endl;
						break;
					}
					write_n(conn, group_id);

					cout << "Article id?: ";
					unsigned int art_id;
					cin >> art_id;
					if (cin.fail()){
						cout << "Wrong input format" << endl;
						break;
					}
					write_n(conn, art_id);

					conn.write(Protocol::COM_END);

					if (conn.read() != Protocol::ANS_GET_ART){
						cout << "Nagat ar fel";
					}
					unsigned int com = conn.read();
					switch (com){
					case Protocol::ANS_ACK:{
						if (conn.read() != Protocol::PAR_STRING){
							cout << "should be str";
						}
						cout << "Title: " + read_s(conn) << endl;
						if (conn.read() != Protocol::PAR_STRING){
							cout << "should be str1";
						}
						cout << "Author: " + read_s(conn) << endl;
						if (conn.read() != Protocol::PAR_STRING){
							cout << "should be str2";
						}
						cout << "Text: " + read_s(conn) << endl;
						break; }
					case Protocol::ANS_NAK:{
						unsigned int err = conn.read();
						switch (err){
						case Protocol::ERR_NG_DOES_NOT_EXIST:
							cout << "Group does not exist" << endl;
							break;
						case Protocol::ERR_ART_DOES_NOT_EXIST:
							cout << "Article does not exist!" << endl;
							break;
						default:
							cout << "now sometjhing is weird!";
							break;
						}
						break;
					}
					default:
						cout << "weiiiird";
					}
					if (conn.read() != Protocol::ANS_END){
						cout << "dafuq";
					}
					break;
				}
				default:
					cout << "Command " << alternative << " does not exist" << endl;
				}
			}

			catch (ConnectionClosedException&) {
				cout << "No reply from server. Exiting." << endl;
				exit(1);
			}
		}
		/*write_number(conn, nbr);
		string reply = read_string(conn);
		cout << " " << reply << endl;*/
	}
}
}

