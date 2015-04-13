#include <string>
#include "article.h"

using namespace std;

Article::Article(string& t, string& a, string& txt, unsigned int i) : title(t), author(a), text(txt), id(i){}

string& Article::get_title(){
	return title;
}
string& Article::get_author(){
	return author;
}
string& Article::get_text(){
	return text;
}
unsigned int& Article::get_id(){
	return id;
}


