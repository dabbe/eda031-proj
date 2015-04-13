#include "connection.h"
#include "protocol.h"

#include <string>
#include <sstream>
#include <memory>
using namespace std;

void write_ns(const Connection& conn, const unsigned int& i){
	conn.write((i >> 24) & 0xFF);
	conn.write((i >> 16) & 0xFF);
	conn.write((i >> 8) & 0xFF);
	conn.write(i & 0xFF);
}

void write_n(const Connection& conn, const unsigned int& i){
	conn.write(Protocol::PAR_NUM);
	write_ns(conn, i);
}
void write_number(const shared_ptr<Connection>& conn, const unsigned int& i){
	write_n(*conn, i);
}

unsigned int read_n(const Connection& conn){
	unsigned char byte1 = conn.read();
	unsigned char byte2 = conn.read();
	unsigned char byte3 = conn.read();
	unsigned char byte4 = conn.read();
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

unsigned int read_number(const shared_ptr<Connection>& conn){
	return read_n(*conn);
}

string read_s(const Connection& conn){
	unsigned int n = read_n(conn);
	stringstream ss;
	for(unsigned int i = 0; i != n; ++i){
		unsigned char c = conn.read();
		ss << c;
	}
	return ss.str();
}

string read_string(const shared_ptr<Connection>& conn){
	return read_s(*conn);
}

void write_s(const Connection& conn, const string& s){
	conn.write(Protocol::PAR_STRING);
	write_ns(conn, s.size());
	for(char c : s){
		conn.write(c);
	}
}
void write_string(const shared_ptr<Connection>& conn, const string& s){
	write_s(*conn, s);
}


