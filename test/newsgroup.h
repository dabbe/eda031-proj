#include <string>
#include <vector>

#include "article.h"

using namespace std;

class Newsgroup{
public:
	Newsgroup(unsigned int id, string& name);
	unsigned int& get_id();
	string& get_name();
	vector<Article>& get_articles();
	unsigned int& get_artcounter();
private:
	vector<Article> articles;
	unsigned int id;
	string name;
	unsigned int artcounter;
};
