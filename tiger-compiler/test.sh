#!/bin/bash
list=`ls ./testfiles | grep ".*tig$"`
for file in $list
do
	echo "Compiling $file"
	(./bin/assemtest ./testfiles/$file &> ./testfiles/$file.log) || true
	echo "Done"
done
