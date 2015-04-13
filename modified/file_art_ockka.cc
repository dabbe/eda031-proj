#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <string.h>
using namespace std;

void createArt(string title, string author, string text, int art_nbr){
	ofstream outfile;
	outfile.open (to_string(art_nbr));
	outfile << "#" + title;
	outfile << "#" + author;
	outfile << "#" + text;
  	outfile.close();
  	Article(title, author, text, art_nbr);
}

Article& void readArt(int art_nbr){
	ifstream myfile (to_string(art_nbr));
	string line;
	if (myfile.is_open())
	{
		while (getline (myfile,line))
		{
			cout << line << '\n';
		}
		myfile.close();
	}
	else cout << "Unable to open file";

	char str[line.size()+1];
	strcpy(str, line.c_str());
	char* pch = strtok (str,"#");
	string words[3];
	int counter = 0;
	while (pch != NULL)
	{
		string w(pch);
		words[counter] = w;
		counter++;
		pch = strtok (NULL, "#");
		
	}
	return Article(art_nbr, words[0], words[1], words[2]);

}

void deleteArt(string art_nbr){
	if(remove(art_nbr) != 0)
		perror("Error deleting file");
	else
		puts("File successfully deleted");
}

int main(){
	createArt("Wow", "Oscar" , "Shit has happend", 1);
	readArt(1);
}