#!/bin/sh

THIS_HOME=$(dirname $0)

do_rm() {
	local ELMT_LIST=$1
	if [ -n "$ELMT_LIST" ]
	then
		echo "REMOVE: [$ELMT_LIST]"
		rm -rf $ELMT_LIST
	fi
}

process_pattern() {
	local PATTERN=$1
	local ELMT_LIST=$(find $THIS_HOME -name "$PATTERN" | tr '\n' ' ')
	do_rm "$ELMT_LIST"
}

while read
do
	process_pattern "$REPLY"
done < $THIS_HOME/.gitignore
