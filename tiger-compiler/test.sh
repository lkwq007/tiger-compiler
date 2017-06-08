#!/bin/bash
list=`ls ./testfiles | grep ".*tig$"`
for file in $list
do
	echo "Compiling $file"
	(./bin/assemirtest ./testfiles/$file &> ./testfiles/$file.log) || true
	(./bin/assemtest ./testfiles/$file &> ./testfiles/$file.pdot) || true
	dot ./testfiles/$file.pdot -T svg -o ./testfiles/$file.svg || true
	echo "Done"
done
