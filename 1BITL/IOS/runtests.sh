#!/bin/bash

###
#IOS, project 1
#File: runtests.sh
#Author: Katerina Zaklova, xzaklo00@stud.fit.vutbr.cz
###

export LC_ALL=C
export IFS=$(echo -en "\n\b")

v=0;
t=0;
r=0;
s=0;
c=0;
exitcode=0;

HELPMSG="Usage: $0 [-vtrsc] TEST_DIR [REGEX]

    -v  validate tree
    -t  run tests
    -r  report results
    -s  synchronize expected results
    -c  clear generated files

    It is mandatory to supply at least one option."

#Clear generated files
function cleaner() {
	tree=$(find "$directory" -type d | sort | grep -E "$regex")
	orig=`pwd`
	for path in $tree; do
		cd $path || { echo "cannot change directory" >&2; exitcode=1; }
		for item in stdout-captured stderr-captured status-captured stdout-delta stderr-delta status-delta; do
			if [ -f $item ]; then
				rm -f "$item" || { echo "cannot remove file $path/$item" >&2; exitcode=1; }
			fi
		done
		cd $orig || { echo "cannot change directory" >&2; exitcode=2; }
	done
}

#Synchronize expected results
function synchro() {
	tree=$(find "$directory" -type d | sort | grep -E "$regex")
	orig=`pwd`
	for path in $tree; do
		cd $path || { echo "cannot change directory" >&2; exitcode=1; }
		for item in stdout-captured stderr-captured status-captured; do
			if [ -f $item ];then
				new=$(echo "$item" | sed -r 's/captured/expected/')
				mv -f "$item" "$new" || { echo "cannot move file $path/$item" >&2; exitcode=1; }
			fi
		done
		cd $orig || { echo "cannot change directory" >&2; exitcode=2; }
	done
}


function validate() {
	tree=$(find "$directory" -type d | sort | grep -E "$regex")
	dir=`pwd`
	for path in $tree; do
		cd $path || { echo "cannot change directory" >&2; exitcode=1; }
		file_flag=0;
		dir_flag=0;
		cmd_flag=0;
		for item in `ls`; do
#Only valid filenames
			if [ -f $item ]; then
				file_flag=1
				if [ $item != "stdout-expected" ] && [ $item != "stderr-expected" ] && [ $item != "status-expected" ] &&
				   [ $item != "stdout-captured" ] && [ $item != "stderr-captured" ] && [ $item != "status-captured" ] &&
				   [ $item != "stdout-delta" ] && [ $item != "stderr-delta" ] && [ $item != "status-delta" ] && [ $item != "stdin-given" ]&&
				   [ $item != "cmd-given" ]; then
					echo "$0 -v: wrong filename" >&2
					exitcode=1
					continue
				fi
			elif [ -d $item ]; then
				dir_flag=1
			fi
#File and directory in directory
			if [ $file_flag -eq 1 ] && [ $dir_flag -eq 1 ] ; then
				echo "$0 -v: directories and other files mixed" >&2
				exitcode=1
				continue
			fi
#symlinks
			if [ -h $item ]; then
				echo "$0 -v: a symlink found" >&2
				exitcode=1
				continue
			fi
#hardlinks
			if [ ! -d $item ]; then
				hard=$(ls -l $item | awk '{print $2}')
				#echo $path"/"$item": "$hard
				if [ "$hard" != "1" ]; then
				echo "$0 -v: a multiple hardlink found" >&2
				exitcode=1
				continue
				fi
			fi
#stdin-given - readable
			if [ -f $item ] && [ $item == "stdin-given" ]; then
				if [ ! -r $item ]; then
					echo "$0 -v: stdin-given is not readable" >&2
					exitcode=1
					continue
				fi
			fi
#stdout|stderr|status-expected|captured|delta writeable
			if [ -f $item ]; then
				if [ $item == "stdout-expected" ] || [ $item == "stderr-expected" ] || [ $item == "status-expected" ] ||
				   [ $item == "stdout-captured" ] || [ $item == "stderr-captured" ] || [ $item == "status-captured" ] ||
				   [ $item == "stdout-delta" ] || [ $item == "stderr-delta" ] || [ $item == "status-delta" ]; then
					if [ ! -w $item ]; then
						echo "$0 -v: (stdout/stderr/status)-(expected/captured/delta) not writeable" >&2
						exitcode=1
						continue
					fi
				fi
			fi
#status-expected|captured - digits and endl
			if [ -f $item ]; then
				if [ $item == "status-expected" ] || [ $item == "status-captured" ]; then
					#cat $item
					if [ `cat $item | grep -Ec "^[[:digit:]]+$"` != "1" ] || [ `cat $item | wc -l` -ne 1 ]; then
						echo "$0 -v: status-(expected/captured) wrong format in $path/$item" >&2
						exitcode=1
						continue
					fi
				fi
			fi
if [ $item != "cmd-given" ]; then
	continue
fi
#cmd-given - executable
			if [ $dir_flag -eq 0 ]; then
				if [ $cmd_flag -eq 0 ]; then
					if [ $item == "cmd-given" ]; then
						if [ -x $item ]; then
						cmd_flag=1
						else
							echo "$0 -v: cmd-given not executable" >&2
							exitcode=1
							continue
						fi
					else
						echo "./$0 -v: cmd-given not found" >&2
						exitcode=1
						continue
					fi
				fi
			else
				if [ $cmd_flag -eq 1 ]; then
					echo "./$0 -v: cmd-given and folder mixed" >&2
					exitcode=1
					continue
				fi
			fi
		done
		cd $dir || { echo "cannot change directory" >&2; exitcode=2; }
	done
}

function runtests() {
	testvar=0
	tree=$(find "$directory" -type d | sort | grep -E "$regex")
	original=`pwd`
	for path in $tree; do
		status=0
		cd $original || { echo "cannot change directory" >&2; exitcode=2; }
		[ -z "$path" ] && continue;
		cd $path || { echo "cannot change directory, possible cmd-given inside" >&2; exitcode=1; }
#regex inferno!
		dirforsed=`sed -r 's/\//\\\\\//g' <<< "$directory"`
		if [ $? -ne 0 ];then
			echo "sed failed">&2
			exitcode=2
		fi
		TEST=$(echo $path | sed "s/$dirforsed\///")
		if [ $? -ne 0 ];then
			echo "sed failed">&2
			exitcode=2
		fi
#cmd-given & executable
		if [ -e cmd-given ];then
			if [ -x cmd-given ]; then
#execute script
				if [ -r stdin-given ]; then
					./cmd-given <"stdin-given" >"stdout-captured" 2>"stderr-captured"
					echo "$?" >"status-captured"
				else
					./cmd-given </dev/null >"stdout-captured" 2>"stderr-captured"
					echo "$?" >"status-captured"
				fi
#diff
				if [ -r stdout-expected ] && [ -r stdout-captured ]; then
					diff -up stdout-expected stdout-captured >"stdout-delta"
				else
					status=1
					exitcode=1
				fi
				if [ -r stderr-expected ] && [ -r stderr-captured ]; then
					diff -up stderr-expected stderr-captured >"stderr-delta"
				else
					status=1
					exitcode=1
				fi
				if [ -r status-expected ] && [ -r status-captured ]; then
					diff -up status-expected status-captured >"status-delta"
				else
					status=1
					exitcode=1
				fi
			else 
				status=1
			fi
#OK & FAILED
			if [ -s stdout-delta ] || [ -s stderr-delta ] || [ -s status-delta ] || [ $status -eq 1 ]; then
				RESULT="FAILED"
				testvar=1
			else
				RESULT="OK"
			fi
			if [ $RESULT == "OK" ]; then
				if [ -t 2 ]; then
					echo -e "$TEST: \033[1;32mOK\033[0m" >&2
				else
					echo "$TEST: OK" >&2
				fi
			elif [ $RESULT == "FAILED" ]; then
				if [ -t 2 ]; then
					echo -e "$TEST: ""\033[1;31mFAILED\033[0m" >&2
				else
					echo "$TEST: FAILED" >&2
				fi
			fi
		fi
	done
	cd $original || { echo "cannot change directory" >&2; exitcode=2; }
#exitcode
	if [ $testvar -eq 0 ]; then
		exitcode=0
	else
		exitcode=1
	fi
}

function report() {
	testvar=0
	tree=$(find "$directory" -type d | sort | grep -E "$regex")
	original=`pwd`
	for path in $tree; do
		status=0
		cd $path || { echo "cannot change directory, possible cmd-given inside" >&2; exitcode=1; }
		if [ -e cmd-given ]; then
			dirforsed=`sed -r 's/\//\\\\\//g' <<< "$directory"`
			if [ $? -ne 0 ];then
				echo "sed failed">&2
				exitcode=2
			fi
			TEST=$(echo $path | sed "s/$dirforsed\///")
			if [ $? -ne 0 ];then
				echo "sed failed">&2
				exitcode=2
			fi
#diff
			if [ -r stdout-expected ] && [ -r stdout-captured ]; then
				diff -up stdout-expected stdout-captured >"stdout-delta"
			else
				status=1
				exitcode=1
			fi
			if [ -r stderr-expected ] && [ -r stderr-captured ]; then
				diff -up stderr-expected stderr-captured >"stderr-delta"
			else
				status=1
				exitcode=1
			fi
			if [ -r status-expected ] && [ -r status-captured ]; then
				diff -up status-expected status-captured >"status-delta"
			else
				status=1
				exitcode=1
			fi
#OK & FAILED
			if [ -s stdout-delta ] || [ -s stderr-delta ] || [ -s status-delta ] || [ $status -eq 1 ]; then
				RESULT="FAILED"
				testvar=1
			else
				RESULT="OK"
			fi
			if [ $RESULT == "OK" ]; then
				if [ -t 1 ]; then
					echo -e "$TEST: \033[1;32mOK\033[0m"
				else
					echo "$TEST: OK"
				fi
			elif [ $RESULT == "FAILED" ]; then
				if [ -t 1 ]; then
					echo -e "$TEST: ""\033[1;31mFAILED\033[0m"
				else
					echo "$TEST: FAILED"
				fi
			fi
		fi
	cd $original || { echo "cannot change directory" >&2; exitcode=2; }
	done
#exitcode
	if [ $testvar -eq 0 ]; then
		exitcode=0
	else
		exitcode=1
	fi
}

# get options
options=$#
while getopts ":vtrsc" opt; do
	case $opt in
		v) v=1
		;;
		t) t=1
		;;
		r) r=1
		;;
		s) s=1
		;;
		c) c=1
		;;
		\?) echo "$HELPMSG" >&2
		exit 2
		;;
	esac
done

# get the other options
shift $(($OPTIND-1))
if [ $# -eq $options ]; then
	echo "$HELPMSG" >&2
	exit 2
fi
# 1 param - directory
if [ $# -eq 1 ]; then
	directory=$1;
	regex=.*;
# 2 params - directory and regex
elif [ $# -eq 2 ]; then
	directory=$1;
	regex=$2;
# too many params
else
	echo "$HELPMSG" >&2
	exit 2
fi

if [ ! -d "$directory" ];then
	echo "\$TESTDIR not found" >&2
	exit 2
fi

if [ ! -d "$directory" ];then
	exit 2
fi

if [ "$v" -eq 1 ]; then
	validate;
fi
if [ "$t" -eq 1 ]; then
	runtests;
fi
if [ "$r" -eq 1 ]; then
	report;
fi
if [ "$s" -eq 1 ];then
	synchro;
fi
if [ "$c" -eq 1 ];then
	cleaner;
fi

if [ $exitcode -eq 0 ]; then
	exit 0;
elif [ $exitcode -eq 1 ]; then
	exit 1;
else
	exit 2;
fi
