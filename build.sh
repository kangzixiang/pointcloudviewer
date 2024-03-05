#!/bin/bash

CURRENT_DIR=$(cd $(dirname $0); pwd)
FILE=${CURRENT_DIR}/products

if [ ! -d "$FILE" ]; then
    echo "$FILE not exist"
    mkdir ${FILE}
fi

parseArguments()
{
	echo Build ARGS: $@
	while true
	do
		case "$1" in
		--clean)
			rm -rf CMakeCache.txt  CMakeFiles/
			;;
		--clean-all)
			rm -rf CMakeCache.txt  CMakeFiles/ ../bin/
			;;
		-h|--help)
			echo -e "\033[32m==================================\033[0m"
			echo "Usage"
			echo "    build.sh [options]"
			echo
			echo "Options"
			echo "    --clean                                                 "
			echo "    --clean-all       = Clean all tmp files before building."
			echo
			echo "Example"
			echo "    ./build.sh --clean"
			echo -e "\033[32m==================================\033[0m"
			exit -1
			;;
		*)
			if [ -z "$1" ]; then
				break
			fi
			;;
		esac
		shift
	done
}

cd products

parseArguments $@

cmake ..

make -j16