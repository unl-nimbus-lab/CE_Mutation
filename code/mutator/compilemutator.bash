#!/bin/bash
DIRECTORY="mutants"

if [ -d "$DIRECTORY" ]; then
    cd mutants
	rm -rf *
	cd ..
else
	mkdir mutants
fi

docker exec mutationcontainer make
if [ $? -eq 0 ]; then
    echo OK
	./mutator -seed=3 -number=0 -category=PET b747cl.c
else
	echo " "
    echo FAIL!!!
	echo " "
fi
