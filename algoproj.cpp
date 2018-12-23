//
//  main.cpp
//  algorithms
//
//  Copyright © 2018 Dylan Kertson. All rights reserved.
//

#include <iostream>
#include <fstream>
using namespace std;

//sets the maximum number of files the program will produce
#define MAX_FILE_COUNT 1000

//global variables
int filecount = 1;
bool full = false;
int file_rank[MAX_FILE_COUNT];
int file_rank_order[MAX_FILE_COUNT];


//uses trie data structure to store dictionary
class trie{
public:
	trie* children[27];
	//rank is based off of appearence in most used words text file
	//if rank is anything other than 0, it is end of word
	//therefore, no end of word identifier is needed here
	//
	//27 = 26 letters of alphabet and 27 = end of sentence
	//punctuation such as . ; ! ?
	int rank;
	//constructor
	trie(){
		this -> rank = 0;
		for(int i = 0; i < 27; ++i){
			this -> children[i] = NULL;
		}
	}
};

trie* populate();
void decode(string input, unsigned long length, string result, trie* root);
int dictionary_contains(trie* root, string word);
void writefile(string result, trie* root);
void quicksort(int first, int last);
int pivot(int first, int last);




int main() {
	trie *dictionary;
	dictionary = populate();
	ifstream input_file;
	input_file.open("input.txt");
	if(input_file.fail()){
		cout << "error reading input file, please make sure it is in proper directory, and is named input.txt" << endl;
	}
	else{
		string s;
		while(getline(input_file, s)){
			decode(s, s.length(), "", dictionary);
		}
	}
	//sorts array of ranks
	for(int i = 0; i < filecount - 1; ++i){
		file_rank_order[i] = i + 1;
	}
	quicksort(0, filecount - 1);
	//prints file names in ascending order
	cout << "File names in ascending order according to dictionary rank:" << endl;
	for(int i = 1; i < filecount; ++i){
		cout << file_rank_order[i] << ".txt" << endl;
	}
	return 0;
}


//
//
//
//populates trie from dictionary.txt, returns address of root node of trie
trie* populate(){
	trie* root = new trie();
	ifstream infile;
	infile.open("dictionary.txt");
	//if file cannot be opened
	if(infile.fail()){
		cout << "error reading dictionary file, please make sure it is in proper directory" << endl;
	}
	//if file is sucessfully opened
	else{
		string s;
		int likely = 1;
		trie* temp = root;
		//sets string s equal to each line of the file
		while(getline(infile, s)){
			//if it is a comment, the line is ignored
			if(s[0] == '#'){
				continue;
			}
			//makes string lowercase, and remove \r at end of string
			transform(s.begin(), s.end(), s.begin(), ::tolower);
			s.erase(remove(s.begin(), s.end(), '\r'), s.end());
			
			//checks to make sure all of s is part of alphabet
			bool alpha = true;
			for(int i = 0; i < s.length(); ++i){
				if(!isalpha(s[i])){
					alpha = false;
					break;
				}
			}
			//makes sure string is not one letter besides whilelisted words
			if(s.length() == 1 && s != "i" && s != "a"){
				alpha = false;
			}
			//insert s into trie
			if(alpha == true){
				for(int i = 0; i < s.length(); ++i){
					if(temp -> children[s[i] - 97] == NULL){
						temp -> children[s[i] - 97] = new trie();
					}
					temp = temp -> children[s[i] - 97];
				}
				//prevents duplicate word insertions
				if(temp -> rank == 0){
					//creates new node for end of sentence punctuation, identical
					//to the last node created for the word
					temp -> children[26] = new trie();
					temp -> rank = likely;
					temp -> children[26] -> rank = likely;
					++likely;
				}
				temp = root;
			}
		}
	}
	infile.close();
	infile.clear();
	return root;
}


//takes string of entire sequence of characters and the length you wish to decode as input
//outputs each possible outcome to files
//input file must be one line
void decode(string input, unsigned long length, string result, trie* root){
	for(int i = 1; i <= length; ++i){
		string prefix = input.substr(0, i);
		if(dictionary_contains(root, prefix) != 0 && !ispunct(input[i])){
			if(i == length){
				result += prefix;
				writefile(result, root);
				return;
			}
			decode(input.substr(i, length - i), length - i, result + prefix + " ", root);
		}
	}
}


//checks if input string is in dictionary
//if it is, returns the rank value of that word
//if not, returns a zero
int dictionary_contains(trie* root, string word){
	trie* crawl = root;
	int index;
	for(int i = 0; i < word.length(); ++i){
		if(word[i] == '.' || word[i] == '?' || word[i] == '!' || word[i] == ';'){
			index = 26;
		}
		else{
			index = word[i] - 97;
		}
		if(crawl -> children[index] == NULL){
			return 0;
		}
		crawl = crawl -> children[index];
	}
	return crawl -> rank;
}


void writefile(string result, trie* root){
	//will not create file if it exceeds the max alotted file count
	if(full == true){
		return;
	}
	else if(filecount > MAX_FILE_COUNT){
		full = true;
		cout << "Maximum number of files, '" << MAX_FILE_COUNT << "', has been created so program will terminate, if you wish, you can increase the value in the MAX_FILE_COUNT definition at the top of the source code" << endl;
		exit(EXIT_FAILURE);
	}
	int rank_total = 0;
	int j = 0;
	for(int i = 1; i < result.length(); ++i){
		if(result[i] == ' '){
			rank_total += dictionary_contains(root, result.substr(j, i - j));
			++i;
			j = i;
		}
	}
	file_rank[filecount - 1] = rank_total;
	ofstream file;
	file.open(to_string(filecount) + ".txt");
	file << result;
	++filecount;
	file.close();
}



//two versions of quicksort are used because i like making inefficient code
void quicksort(int first, int last){
	int pivotElement;
	
	if(first < last){
		pivotElement = pivot(first, last);
		quicksort(first, pivotElement-1);
		quicksort(pivotElement+1, last);
	}
}


int pivot(int first, int last){
	int  p = first;
	int pivotElement = file_rank[first];
	
	for(int i = first+1 ; i <= last ; i++){
		if(file_rank[i] <= pivotElement){
			++p;
			swap(file_rank[i], file_rank[p]);
			swap(file_rank_order[i], file_rank_order[p]);
		}
	}
	
	swap(file_rank[p], file_rank[first]);
	swap(file_rank_order[p], file_rank_order[first]);
	
	return p;
}

