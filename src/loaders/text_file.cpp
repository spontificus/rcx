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
	char *word;
	while ((word=Word_From_Buffer()))
		Append_Word(word);

	if (word_count==0)
		return false;
	
	return true;

	/*char *saveptr1, *saveptr2;
	//two loops: first one to handle quotations
	char *sup, *sub;
	//each second token is a quotation
	bool quoted = false;

	sup = strtok_r(buffer, "\"\n", &saveptr1); //divide with quote (+remove newline)

	//this should not happen anyway
	if (!sup)
		return false;

	while (sup)
	{
		if (quoted) //this is quoted
		{
			Append_Word(sup);

			quoted = false; //next time is normal
		}
		else //normal list of words
		{
			//divide sup with spaces
			sub = strtok_r(sup, " \f\n\r\t\v", &saveptr2);

			//this should not happen
			if (!sub)
				return false;

			while (sub)
			{
				if (sub[0] == '#') //comment starting
					return true; //stop reading, assume we got wanted words (we have)

				Append_Word(sub);

				//get next
				sub = strtok_r(NULL, " \f\n\r\t\v", &saveptr2);
			}

			quoted = true; //next time is quote
		}

		//get next
		sup = strtok_r(NULL, "\"\n", &saveptr1);
	}
	

	return true;*/
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

char *Text_File::Word_From_Buffer()
{
	//seek for char not space (or end of buffer)

	//is at end?
	if (*buffer_ptr == '\0')
		return NULL;

	while (isspace(*buffer_ptr))
	{
		++buffer_ptr;

		//reached end
		if (buffer_ptr == '\0')
			return NULL;
	}


	//first char of new word
	char *word_start = buffer_ptr;

	if (*word_start == '#') //commented: end of line
		return NULL;


	//seek to end of word

	//printf("%c\n", *word_start);
	//if quotation, end is " or \0
	if (*word_start == '\"')
	{
		do
		{
			++buffer_ptr;
		//printf("quoted %c\n", *buffer_ptr);
		}
		while (*buffer_ptr!='\"' && *buffer_ptr!='\0');
		if (*buffer_ptr!='\0')
			++buffer_ptr;
	}
	else //normal: space or \0
	{
		do
		{
			++buffer_ptr;
		}
		while (!isspace(*buffer_ptr) && *buffer_ptr!='\0');
	}

	//if reading ended by end of buffer, do nothing more
	//if reading ended by space, modify space to be \0 and jump over it
	//(this will mark the end of word when processed by Append_Word() )
	if (*buffer_ptr!='\0')
	{
		*buffer_ptr='\0';
		++buffer_ptr;
		//printf("> \"%c\"\n", *buffer_ptr);
	}

	return word_start;
}


void Text_File::Append_Word(char *word)
{
	++word_count;

	if (word_count > list_size)
	{
		printlog(1, "WARNING: Text_File word list was too small, resizing\n");
		list_size+=INITIAL_LIST_SIZE;
		words = (char**) realloc(words, list_size*sizeof(char**));
	}

	words[word_count-1] = (char*) calloc(strlen(word)+1, sizeof(char));
	strcpy(words[word_count-1], word);
}

void Text_File::Free_Words()
{
	for (int i=0; i<word_count; ++i)
		free (words[i]);

	word_count = 0;
}

