#include <string>
using namespace std;
class Article{
public:
	Article();
	string& get_title();
	string& get_author();
	string& get_text();
	int& get_id();
private:
	string title;
	string author;
	string text;
	int id;
};
