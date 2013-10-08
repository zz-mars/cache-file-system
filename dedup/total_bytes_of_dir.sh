#/!bin/bash

if [ $# -ne 1 ] ;then
	echo "Usage command [dir]"
	exit 1
fi

dir=$1

if [ ! -d "$dir" ] ;then 
	echo "invalid dir!"
	exit 1
fi

total_bytes=0

cd $dir

if [ $? -ne 0 ] ;then
	echo "cd fail!"
	exit 1
fi

for file in *
do
	if [ -d "$file" ] ;then
		continue
	fi
	file_byts=$(stat "$file" | awk 'NR==2{print $2}')
	let "total_bytes+=file_byts"
done

cd - > /dev/null

echo "$total_bytes"

exit 0

