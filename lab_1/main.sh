#!/bin/sh

# Group: ПМ-92
# Task version: 8
# Students: Begichev, Shishkin

# Started directory for program
start_dir=$root

# Is argument given?
if [ $# -gt 1 ]; then
    echo -e "Usage: $0 [directory_name]"
    exit
    
# And is this a directory?
elif [ ! -d $1 ]; then
    echo "There is no such directory: $1"
    exit

# Change started directory
# to given argument
else
    start_dir=$1
fi

# Just colors
RED='\033[0;31m'
YELLOW='\033[0;33m'
YELLOW_BOLD='\033[1;33m'
NC='\033[0m'

# The -print0 will use the NULL as a file separator
# instead of a newline, sort -z and the -d $'\0' will use NULL
# as the separator while reading.
find $start_dir -type d -print0 | sort -z | while read -d $'\0' dir; do
    echo -e ${YELLOW_BOLD}"$dir"${NC}

    # Iters are used to count
    # files in the directory
    iters=0

    # If directory is empty, print info and go next
    if [ $(find "$dir" -maxdepth 1 -type f | wc -l) -eq 0 ]; then
        echo " * There is no any file"

    # Else iterate files over directory
    else
        # pipe drops variables after running
        # but with this short is don't
        shopt -s lastpipe 
        find "$dir" -maxdepth 1 -type f -print0 | sort -z | while read -d $'\0' file; do
            # Print filename and used amount of memory in bytes
            echo -e " >"${YELLOW} ${file##*/} ${RED} $(du -b "$file" | cut -f1)B ${NC}

            # Count file in the directory
            ((iters++))
        done

        # Print the number of files in the directory
        echo " * There are $iters files total"
    fi

    # Output seporator (for directories)
    echo ''
done