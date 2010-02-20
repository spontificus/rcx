#ifndef _RCX_TEXT_FILE_H
#define _RCX_TEXT_FILE_H
//definition of class for easy text file processing
//(provides a list of words for each line)
#include <stdio.h>

//initial values for holding data (automatically resized if needed)
#define INITIAL_BUFFER_SIZE 150 //how manny characters
#define INITIAL_LIST_SIZE 8 //how many words

class Text_File
{
	public:
		//true if could open file
		bool open;

		//vector of strings (each string is a word)
		//std::vector<std::string> words;
		//how many words read from line
		char **words;
		int word_count;

		//Text_File(std::string name);
		Text_File(const char *name);
		~Text_File();

		//read the next/current line
		bool Read_Line();

		//actual function for opening new file
		//(useful if wanting to reuse object)
		bool Open(const char *file);

	private:
		//std::ifstream stream;
		FILE *fp;

		//manualy allocate/free buffer to allow reallocating
		char *buffer;
		size_t buffer_size;

		//how many words can be stored in "words"
		int list_size;

		//throws text until newline
		bool Throw_Line();
		//find first word (that isn't commented)
		bool Seek_First();
		//copy the line to buffer
		bool Line_To_Buffer();
		//split buffer to list of words
		bool Buffer_To_List();

		//copy word to list
		void Append_To_List(char * word);
		//clear the word list
		void Clear_List();

		//function for closing fp and freeing word list
		void Close();
};
#endif
