#include "newsgroup.h"

Newsgroup::Newsgroup(unsigned int i, string& s) : id(i), name(s){}

unsigned int& Newsgroup::get_id(){
	return id;
}

string& Newsgroup::get_name(){
	return name;
}

vector<Article>& Newsgroup::get_articles(){
	return articles;
}
