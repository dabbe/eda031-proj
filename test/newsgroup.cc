#include "newsgroup.h"

Newsgroup::Newsgroup(int& i, string& s) : id(i), name(s){}

int& Newsgroup::get_id(){
	return id;
}

string& Newsgroup::get_name(){
	return name;
}
