#!/bin/bash

PRO_FILE=`ls -1 | grep ".pro$" | head -n1`
BUILD_FILE='BUILD'

if [[ "$#" > '1' || -z "$PRO_FILE" ]]; then
	echo "Usage: $0 [--next]"
	echo -e "This utility must be started next to the .pro file\n"
	exit 1
fi

if [ ! -f "$BUILD_FILE" ]; then
	echo -n "0" > $BUILD_FILE
fi

BUILD_NUMBER=`cat $BUILD_FILE`
NEXT_BUILD_NUMBER=`expr $BUILD_NUMBER + 1`

if [[ "$#" == '1' && "$1" == '--next' ]]; then
	echo -n "$NEXT_BUILD_NUMBER" | tee $BUILD_FILE
else
	echo -n "$BUILD_NUMBER"
fi

exit 0