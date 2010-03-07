#!/bin/sh
#simple script to generate makefile compatible list of prerequisites


for FILE in *cpp */*cpp
do
	DIR=$(dirname $FILE)
	NAME=$(basename $FILE ".cpp")

	#in case file is in root of src dir, don't print "/./"
	if [ $DIR == "." ]
	then
		echo -n "build/$NAME.o:"
	else
		echo -n "build/$DIR/$NAME.o:"
	fi

	echo -n " $FILE"

	for HEADER in $(grep '^#include.*"' $FILE |cut -d'"' -f2)
	do
		echo -n " $DIR/$HEADER"
	done
	echo ""
done
