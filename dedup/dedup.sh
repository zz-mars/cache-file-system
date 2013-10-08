#!/bin/bash

DIR=/home/zz/dedup/
DEDUP_UTI=/home/zz/dedup_code/dedup

#echo "@ $@"
#echo "? $?"
#echo "# $#"
#echo "1 $1"

if [ $# -ne 1 ] ;then
	echo "Usage ./dedup.sh [dir]"
	exit 1
fi

DIR=$1

#echo "dedup dir $DIR"

cd $DIR

if [ $? -ne 0 ] ;then
	echo "Please make sure $DIR is an existing directory and you have privillege to into it"
	exit 1
fi

for file in *
do
	if [ -d "$file" ] ;then
		continue
	fi

	$DEDUP_UTI "$file"
done

cd - > /dev/null

exit 0

