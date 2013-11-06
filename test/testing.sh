#!/bin/sh

if [ $# != 1 ]
then
    echo usage: $0 \<path to directory with build\>
    exit 1
fi

MSS_BUILD="$(readlink -f $1)"

# Add path to libraries in LD_LIBRARY_PATH
LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$MSS_BUILD/lib:$MSS_BUILD/test/lib"

export LD_LIBRARY_PATH

# Change current directory
cd $MSS_BUILD/test/bin

./fulltest

exit 0
