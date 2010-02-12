#include "../shared/shared.hpp"
#include "loaders.hpp"

//NOTE: it might seem strange to use C text processing in C++ code,
//but it is simply more suitable in this case

Text_File::Text_File (const char *name)
{
	open = false; //default until attempting opening
	word_count = 0; //no words read yet

	//open
	Open (name);

	//allocate buffer anyway (even if not open), if reopening
	buffer_size = INITIAL_BUFFER_SIZE;
	list_size = INITIAL_LIST_SIZE;
	buffer = (char*) malloc (buffer_size);
	words = (char**) malloc (list_size*sizeof(char**));
}

Text_File::~Text_File ()
{
	Close();

	free (buffer);
	free (words);
}

bool Text_File::Open (const char *file)
{
	//close old
	Close();

	printlog(2, " * Text_File: opening file %s\n", file);

	//open
#ifndef windows
	//proper version
	fp = fopen(file, "r");
#else
	//windows always expects carriage return before newline
	//(since we are processing unix-based text files, go binary)
	fp = fopen(file, "rb");
#endif

	//check success
	if (fp)
	{
		open = true;
	}
	else
	{
		open = false;
		printlog(0, "ERROR: could not open file %s!\n", file);
	}

	return open;
}

void Text_File::Close()
{
	//make sure no old data is left
	if (open)
	{
		printlog(2, " * Text_File: closing file\n");
		fclose (fp);
		Clear_List();
	}
}

bool Text_File::Read_Line ()
{
	//remove the old words
	Clear_List();

	//the following actions goes false if end of file
	if (!Seek_First())
		return false;
	if (!Line_To_Buffer())
		return false;
	if (!Buffer_To_List())
		return false;

	//ok
	return true;
}

bool Text_File::Seek_First()
{
	char c;
	while (true)
	{
		c = fgetc(fp);
		if (c == EOF)
			return false;

		else if (!isspace(c)) //not space
		{
			if (c == '#') //commented line, get next
			{
				Throw_Line();
				return Seek_First();
			}

			//else, we now have first word
			//(push back first char)
			ungetc (c, fp);
			return true;
		}
	}
}

bool Text_File::Line_To_Buffer()
{
	//make sure to realloc buffer if too small
	int text_read=0; //no text already read

	while (true)
	{
		//this should always work, but just to be safe
		if (!(fgets( (&buffer[text_read]) , (buffer_size-text_read) , fp)))
			return false;

		text_read = strlen(buffer);

		//if EOF or read a newline, everything ok
		if (feof(fp) || buffer[text_read-1] == '\n')
			return true;
		
		//else: I guess the buffer was too small...
		printlog(1, "WARNING: Text_File buffer was too small, resizing\n");
		buffer_size += INITIAL_BUFFER_SIZE;
		buffer = (char*) realloc (buffer, buffer_size);
	}
}

bool Text_File::Buffer_To_List()
{
	char *buffer_ptr = buffer; //position buffer pointer to start of buffer

	while (true)
	{
		//seek for char not space (if not end of buffer)
		while (*buffer_ptr != '\0' && isspace(*buffer_ptr))
			++buffer_ptr;

		//reached end (end of buffer, comment or newline)
		if (*buffer_ptr == '\0' || *buffer_ptr == '#' || *buffer_ptr == '\n')
			break;

		//
		//ok got start of new word
		//

		//there are two kinds of "words"
		if (*buffer_ptr == '\"') //quotation: "word" begins after " and ends at " (or \0)
		{
			//go one step more (don't want " in word)
			++buffer_ptr;

			//wery unusual error (line ends after quotation mark)
			if (*buffer_ptr == '\0')
			{
				printlog(0, "WARNING: Text_File line ended just after quotation mark (not counted)...\n");
				break;
			}

			//ok
			Append_To_List(buffer_ptr);

			//find next " or end of line
			while (*buffer_ptr!='\"' && *buffer_ptr!='\0')
				++buffer_ptr;

			if (*buffer_ptr=='\0') //end of line before end of quote
				printlog(0, "WARNING: Text_File reached end of line before end of quote...\n");
			else
			{
				*buffer_ptr = '\0'; //make this end (instead of ")
				++buffer_ptr; //jump over this "local" end
			}
		}
		else //normal: word begins after space and ends at space (or \0)
		{
			Append_To_List(buffer_ptr);

			//look for end of word (space or end of buffer)
			++buffer_ptr;
			while (*buffer_ptr != '\0' && !isspace(*buffer_ptr))
				++buffer_ptr;

			//if word ended by end of buffer, do nothing. else
			if (*buffer_ptr != '\0')
			{
				*buffer_ptr = '\0'; //mark as end of word
				++buffer_ptr; //jump over local end
			}
		}
	}

	//in case no words were read from buffer
	if (word_count==0)
		return false;
	
	return true;
}

bool Text_File::Throw_Line ()
{
	char c;
	while (true)
	{
		c = fgetc(fp);
		if (c == EOF)
			return false;
		else if (c == '\n')
			return true;
	}
}

void Text_File::Append_To_List(char *word)
{
	++word_count;

	if (word_count > list_size)
	{
		printlog(1, "WARNING: Text_File word list was too small, resizing\n");
		list_size+=INITIAL_LIST_SIZE;
		words = (char**) realloc(words, list_size*sizeof(char**));
	}

	words[word_count-1] = word; //point to word in buffer
}

void Text_File::Clear_List()
{
	//this isn't necessary (since word count is set to 0)
	for (int i=0; i<word_count; ++i)
		words[i]=NULL;
	//

	word_count = 0;
}

