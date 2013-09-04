#!/bin/sh

if [ $# != 1 ]
then
    echo usage: $0 \<path to directory with build\>
    exit 1
fi

MSS_BUILD="$(readlink -f $1)"
echo $MSS_BUILD

# Add path to libraries in LD_LIBRARY_PATH
LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$MSS_BUILD/lib:$MSS_BUILD/tests/lib"

export LD_LIBRARY_PATH

# Change current directory
cd $MSS_BUILD/tests/bin

./fulltest

exit 0
