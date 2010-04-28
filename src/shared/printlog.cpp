/*
 * RCX Copyright (C) Slinger
 *
 * This program comes with ABSOLUTELY NO WARRANTY!
 *
 * This is free software, and you are welcome to
 * redistribute it under certain conditions.
 *
 * See license.txt and README for more info
 */

#include <stdarg.h>
#include "internal.hpp"
#include "printlog.hpp"

//verbosity indicators
const char *indicator[] = {"=> ", " > ", " * "};

//print log message - if it's below or equal to the current verbosity level
void printlog (int level, const char *text, ...)
{
	if (level <= internal.verbosity)
	{
		if (level==0)
			putchar('\n');

		//print verbosity indicator
		fputs(indicator[level], stdout); //puts adds newline, fputs instead

		//print message
		va_list list;
		va_start (list, text);
		vprintf (text, list);
		va_end (list);

		//put newline
		putchar('\n');
	}
}

