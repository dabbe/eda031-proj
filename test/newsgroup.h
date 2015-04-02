#include <string>
using namespace std;

class Newsgroup{
public:
	Newsgroup(int& id, string& name);
	int& get_id();
	string& get_name();
private:
	int id;
	string name;
};
