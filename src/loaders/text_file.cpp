//NOTE: it might seem strange to use C text processing in C++ code,
//but it is simply more suitable in this case

Text_File::Text_File (const char *name)
{
	open = false; //default until attempting opening
	word_count = 0; //no words read yet

	Open (name);
	//allocate buffer anyway, if reopening
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
		Free_Words();
	}
}

bool Text_File::Read_Line ()
{
	//remove the old words
	Free_Words();

	//the following actions goes false if end of file
	if (!Seek_First())
		return false;
	if (!Line_To_Buffer())
		return false;
	if (!Buffer_To_Words())
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

bool Text_File::Buffer_To_Words()
{
	buffer_ptr = buffer; //position buffer pointer to start of buffer
	while (Word_From_Buffer());

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

bool Text_File::Word_From_Buffer()
{
	//is at end?
	if (*buffer_ptr == '\0')
		return false;

	//seek for char not space (or end of buffer)
	while (isspace(*buffer_ptr))
	{
		++buffer_ptr;

		//reached end
		if (*buffer_ptr == '\0')
			return false;
	}

	//maybe this is a comment (then end of line)
	if (*buffer_ptr == '#')
		return false;

	//pointer to first char of new word
	char *word_start;
	//count how many characters
	size_t c_count=0;

	//there a two ways of locating the word
	if (*buffer_ptr == '\"') //quotation: "word" begins after " and ends at " (or \0)
	{
		//go one step more (don't want " in word)
		++buffer_ptr;
		word_start = buffer_ptr;

		//find next " or end of line
		while (*buffer_ptr!='\"' && *buffer_ptr!='\0')
		{
			++buffer_ptr;
			++c_count;
		}

		if (*buffer_ptr=='\0') //end of line before end of quote
			printlog(0, "WARNING: Text_File reached end of line before end of quote...\n");
		else
			++buffer_ptr; //so not to get " at nedt read
	}
	else //normal: word begins after space and ends at space (or \0)
	{
		word_start = buffer_ptr;
		do
		{
			++buffer_ptr;
			++c_count;
		}
		while (!isspace(*buffer_ptr) && *buffer_ptr!='\0');

		++buffer_ptr; //make sure ignore what know to be space at next time
	}

	//ok
	Append_Word(word_start, c_count);
	return true;;
}


void Text_File::Append_Word(char *word, size_t count)
{
	++word_count;

	if (word_count > list_size)
	{
		printlog(1, "WARNING: Text_File word list was too small, resizing\n");
		list_size+=INITIAL_LIST_SIZE;
		words = (char**) realloc(words, list_size*sizeof(char**));
	}

	words[word_count-1] = (char*) calloc(count+1, sizeof(char));
	memcpy(words[word_count-1], word, sizeof(char)*count);
	words[word_count-1][count]='\0';
}

void Text_File::Free_Words()
{
	for (int i=0; i<word_count; ++i)
		free (words[i]);

	word_count = 0;
}

