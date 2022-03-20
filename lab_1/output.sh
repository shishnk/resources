#!/bin/bash

IFS=$'\n' # переменная окружения

# Color variables
red='\033[0;31m'
green='\033[0;32m'
yellow='\033[0;33m'
blue='\033[0;34m'
cyan='\033[0;36m'
clear='\033[0m'

start_dir=$root

if [ -d "$1" ]; then
    start_dir=$1
else
    echo -e "${yellow}Папка: ${blue}$1${yellow} не существует.${clear}" && exit
fi

function iterating {
    file_num=0
    echo -e "${yellow}Папка: ${blue}$1${clear}"
    for file in $(find "$1" -maxdepth 1 -type f | sort); do
        echo -e " * ${green}$(basename "$file"): ${red}$(stat -c%s "$file") bytes${clear}"
        file_num=$((file_num + 1))
    done

    echo -e "${cyan}Общее количество файлов: ${red}$file_num${clear}"
    file_num=0
}

for dir in $(find "$start_dir" -type d); do
    iterating "$dir"
done