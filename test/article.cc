#include <string>
#include "article.h"

using namespace std;

Article::Article(){}

string& Article::get_title(){
	return title;
}
string& Article::get_author(){
	return author;
}
string& Article::get_text(){
	return text;
}
int& Article::get_id(){
	return id;
}


