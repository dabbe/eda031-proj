#include "newsgroup.h"

Newsgroup::Newsgroup(unsigned int i, string& s) : id(i), name(s), artcounter(0){}

unsigned int& Newsgroup::get_id(){
	return id;
}

string& Newsgroup::get_name(){
	return name;
}

vector<Article>& Newsgroup::get_articles(){
	return articles;
}

unsigned int& Newsgroup::get_artcounter(){
	return artcounter;
}
