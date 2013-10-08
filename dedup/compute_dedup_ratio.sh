#/!bin/bash

if [ $# -ne 1 ] ;then
	echo "Usage command [dir]"
	exit 1
fi

CACHE_DIR=/mnt/supercache

DIR=$1

if [ ! -d "$DIR" ] ;then 
	echo "invalid dir!"
	exit 1
fi

total_bytes=$(./total_bytes_of_dir.sh "$DIR")

bytes_after_dedup=$(./total_bytes_of_dir.sh "$CACHE_DIR")

echo "Total bytes #$total_bytes"

echo "Bytes after dedup #$bytes_after_dedup"

let "deduped_bytes=$total_bytes-$bytes_after_dedup"
echo "Deduped bytes #$deduped_bytes"

let "dedup_ratio=$deduped_bytes*100/$total_bytes"

echo "Dedup_ratio # $dedup_ratio%"

exit 0

