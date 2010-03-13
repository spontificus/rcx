#!/bin/sh
#simple script to generate makefile compatible list of prerequisites


#remove old
rm prereqs.lst

#generate new
for FILE in *cpp */*cpp
do
	DIR=$(dirname $FILE)
	NAME=$(basename $FILE ".cpp")

	#in case file is in root of src dir, don't print "/./"
	if [ $DIR == "." ]
	then
		echo -n "build/$NAME.o:" >> prereqs.lst
	else
		echo -n "build/$DIR/$NAME.o:" >> prereqs.lst
	fi

	for HEADER in $(grep '^#include.*"' $FILE |cut -d'"' -f2)
	do
		echo -n " $DIR/$HEADER" >> prereqs.lst
	done
	echo "" >> prereqs.lst
done
