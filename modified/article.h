#include <string>
using namespace std;
class Article{
public:
	Article(string& t, string& a, string& txt, unsigned int i);
	string& get_title();
	string& get_author();
	string& get_text();
	unsigned int& get_id();
private:
	string title;
	string author;
	string text;
	unsigned int id;
};
