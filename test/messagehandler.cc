#include "connection.h"
#include "protocol.h"

#include <string>
#include <sstream>
using namespace std;

unsigned int read_number(const shared_ptr<Connection>& conn){
	unsigned char byte1 = conn->read();
	unsigned char byte2 = conn->read();
	unsigned char byte3 = conn->read();
	unsigned char byte4 = conn->read();
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

void write_nbrs(const shared_ptr<Connection>& conn, const unsigned int& i){
	conn->write((i >> 24) & 0xFF);
	conn->write((i >> 16) & 0xFF);
	conn->write((i >> 8) & 0xFF);
	conn->write(i & 0xFF);
}
void write_number(const shared_ptr<Connection>& conn, const unsigned int& i){
	conn->write(Protocol::PAR_NUM);
	write_nbrs(conn, i);
}

string read_string(const shared_ptr<Connection>& conn){
	unsigned int n = read_number(conn);
	stringstream ss;
	for(unsigned int i = 0; i != n; ++i){
		unsigned char c = conn->read();
		ss << c;
	}
	return ss.str();
}

void write_string(const shared_ptr<Connection>& conn, const string& s){
	conn->write(Protocol::PAR_STRING);
	write_nbrs(conn, s.size());
	for(char c : s){
		conn->write(c);
	}
}
