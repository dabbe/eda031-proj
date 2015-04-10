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
#include <climits>

using namespace std;

void protocol_err(string message){
	cerr << "Protocol error, expected " + message + " shutting down Client" << endl;
	exit(1);
}

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
							protocol_err("Protocol::ANS_LIST_NG");
						}
						if (conn.read() != Protocol::PAR_NUM){
							protocol_err("Protocol::PAR_NUM");
						}
						unsigned int nbr = read_n(conn);
						for (unsigned int i = 0; i != nbr; ++i){
							if (conn.read() != Protocol::PAR_NUM){
								protocol_err("Protocol::PAR_NUM");
							}
							unsigned int id = read_n(conn);
							if (conn.read() != Protocol::PAR_STRING){
								protocol_err("Protocol::PAR_STRING");
							}
							string s = read_s(conn);
							cout << id << ". " + s << endl;
						}
						if (conn.read() != Protocol::ANS_END){
							protocol_err("Protocol::ANS_END");
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
							protocol_err("Protocol::ANS_CREATE_NG");
						}
						unsigned char c = conn.read();
						switch (c){
							case Protocol::ANS_NAK:
								if (conn.read() != Protocol::ERR_NG_ALREADY_EXISTS){
									protocol_err("Protocol::ERR_NG_ALREADY_EXISTS");
								}
								else{
									cout << "Group already exists";
									break;
								}
							case Protocol::ANS_ACK:
								cout << "Group created";
								break;
							default:
								protocol_err("Protocol::ANS_NAK or Protocol::ANS_ACK");
						}
						if (conn.read() != Protocol::ANS_END){
							protocol_err("Protocol::ANS_END");
						}
						cout << endl << endl;
						break;
					}
					case 3:{
						cout << "Delete newsgroup:" << endl;
						cout << "Type the group id you want to remove: ";
						unsigned int id;
						cin >> id;
						while (cin.fail()){
							cout << "Wrong input format" << endl;
							cin.clear();
							cin.ignore(INT_MAX, '\n');
							cout << "Type the group id you want to remove: ";
							cin >> id;
						}
						conn.write(Protocol::COM_DELETE_NG);
						write_n(conn, id);
						conn.write(Protocol::COM_END);

						if (conn.read() != Protocol::ANS_DELETE_NG){
							protocol_err("Protocol::ANS_DELETE_NG");
						}
						unsigned char c = conn.read();
						switch (c){
							case Protocol::ANS_NAK:
							if (conn.read() != Protocol::ERR_NG_DOES_NOT_EXIST){
								protocol_err("Protocol::ERR_NG_DOES_NOT_EXIST");
							}
							else{
								cout << "Group does not exist";
							}
							case Protocol::ANS_ACK:
							cout << "Group deleted";
							break;
							default:
							protocol_err("Protocol::ANS_NAK or Protocol::ANS_ACK");
						}
						if (conn.read() != Protocol::ANS_END){
							protocol_err("Protocol::ANS_END");
						}
						cout << endl << endl;
						break;
					}
					case 4:{
						cout << "List articles in newsgroup:" << endl;
						cout << "Newsgroup id?: ";
						unsigned int id;
						cin >> id;
						while (cin.fail()){
							cout << "Wrong input format" << endl;
							cin.clear();
							cin.ignore(INT_MAX, '\n');
							cout << "Which newsgroup id?: ";
							cin >> id;
						}
						conn.write(Protocol::COM_LIST_ART);
						write_n(conn, id);
						conn.write(Protocol::COM_END);

						if (conn.read() != Protocol::ANS_LIST_ART){
							protocol_err("Protocol::ANS_LIST_ART");
						}
						unsigned char c = conn.read();
						switch (c){
							case Protocol::ANS_NAK:
							if (conn.read() != Protocol::ERR_NG_DOES_NOT_EXIST){
								protocol_err("Protocol::ERR_NG_DOES_NOT_EXIST");
							}
							else{
								cout << "Group does not exist";
							}
							break;
							case Protocol::ANS_ACK:{
								if (conn.read() != Protocol::PAR_NUM){
									protocol_err("Protocol::PAR_NUM");
								}
								unsigned int n = read_n(conn);
								for (unsigned int i = 0; i != n; ++i){
									if (conn.read() != Protocol::PAR_NUM){
										protocol_err("Protocol::PAR_NUM");
									}
									unsigned int id = read_n(conn);
									if (conn.read() != Protocol::PAR_STRING){
										protocol_err("Protocol::PAR_STRING");
									}
									string s = read_s(conn);
									cout << id << ". " + s << endl;
								}
								break;
							}
							default:
							protocol_err("Protocol::ANS_NAK or Protocol::ANS_ACK");
						}
						if (conn.read() != Protocol::ANS_END){
							protocol_err("Protocol::ANS_END");
						}
						cout << endl << endl;
						break;
					}
					case 5:{
						cout << "Create article:" << endl;
						cout << "Group ID?: ";
						unsigned int id;
						cin >> id;
						while (cin.fail()){
							cout << "Wrong input format" << endl;
							cin.clear();
							cin.ignore(INT_MAX, '\n');
							cout << "Group id?: ";
							cin >> id;
						}
						conn.write(Protocol::COM_CREATE_ART);
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
							protocol_err("Protocol::ANS_CREATE_ART");
						}
						unsigned int com = conn.read();
						switch (com){
							case Protocol::ANS_NAK:
							if (conn.read() != Protocol::ERR_NG_DOES_NOT_EXIST){
								protocol_err("Protocol::ERR_NG_DOES_NOT_EXIST");
							}
							else{
								cout << "Newsgroup does not exist" << endl;
							}
							break;
							case Protocol::ANS_ACK:
							cout << "Article created!" << endl;
							break;
							default:
							protocol_err("Protocol::ANS_NAK or Protocol::ANS_ACK");
						}
						if (conn.read() != Protocol::ANS_END){
							protocol_err("Protocol::ANS_NAK or Protocol::ANS_ACK");
						}
						cout << endl << endl;
						break;
					}
					case 6:{
						cout << "Delete article:" << endl;
						cout << "What group id?: ";
						conn.write(Protocol::COM_DELETE_ART);
						unsigned int group_id;
						cin >> group_id;
						while (cin.fail()){
							cout << "Wrong input format" << endl;
							cin.clear();
							cin.ignore(INT_MAX, '\n');
							cout << "What group id?: ";
							cin >> group_id;
						}
						write_n(conn, group_id);

						cout << "What article id?: ";
						unsigned int art_id;
						cin >> art_id;
						while (cin.fail()){
							cout << "Wrong input format" << endl;
							cin.clear();
							cin.ignore(INT_MAX, '\n');
							cout << "What article id?: ";
							cin >> art_id;
						}
						write_n(conn, art_id);

						conn.write(Protocol::COM_END);

						if (conn.read() != Protocol::ANS_DELETE_ART){
							protocol_err("Protocol::ANS_DELETE_ART");
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
									protocol_err("Protocol::ERR_ART_DOES_NOT_EXIST or Protocol::ERR_NG_DOES_NOT_EXIST");
								}
								break;
							}
							default:
							protocol_err("Protocol::ANS_NAK or Protocol::ANS_ACK");
						}
						if (conn.read() != Protocol::ANS_END){
							protocol_err("Protocol::ANS_END");
						}
						cout << endl << endl;
						break;
					}
					case 7:{
						cout << "Get article:" << endl;
						cout << "Group id?: ";
						conn.write(Protocol::COM_GET_ART);
						unsigned int group_id;
						cin >> group_id;
						while (cin.fail()){
							cout << "Wrong input format" << endl;
							cin.clear();
							cin.ignore(INT_MAX, '\n');
							cout << "What group id?: ";
							cin >> group_id;
						}
						write_n(conn, group_id);

						cout << "Article id?: ";
						unsigned int art_id;
						cin >> art_id;
						while (cin.fail()){
							cout << "Wrong input format" << endl;
							cin.clear();
							cin.ignore(INT_MAX, '\n');
							cout << "What article id?: ";
							cin >> art_id;
						}
						write_n(conn, art_id);

						conn.write(Protocol::COM_END);

						if (conn.read() != Protocol::ANS_GET_ART){
							protocol_err("Protocol::ANS_GET_ART");
						}
						unsigned int com = conn.read();
						switch (com){
							case Protocol::ANS_ACK:{
								if (conn.read() != Protocol::PAR_STRING){
									protocol_err("Protocol::PAR_STRING");
								}
								cout << "Title: " + read_s(conn) << endl;
								if (conn.read() != Protocol::PAR_STRING){
									protocol_err("Protocol::PAR_STRING");
								}
								cout << "Author: " + read_s(conn) << endl;
								if (conn.read() != Protocol::PAR_STRING){
									protocol_err("Protocol::PAR_STRING");
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
										protocol_err("Protocol::ERR_ART_DOES_NOT_EXIST or Protocol::ERR_NG_DOES_NOT_EXIST");
									}
									break;
								}
								default:
								protocol_err("Protocol::ANS_NAK");
							}
							if (conn.read() != Protocol::ANS_END){
								protocol_err("Protocol::ANS_END");
							}
							cout << endl << endl;
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
		cin.clear();
		//cin.ignore(INT_MAX, '\n');

	}
}

