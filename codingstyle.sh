#!/bin/bash

RED="\033[31m"
CLIGNOTE="\033[31;5m"
GREEN="\033[0m\033[32m"
YELLOW="\033[0m\033[33m"
NORMAL="\033[0m"

echo -e "\n//////////////////////////////// BRACES_TEST ///////////////////////////////////"
touch ls_file
find ldso include -name *.c -o -name *.h > ls_file
while IFS=' ' read -r file; do
    nb_line=$(wc -l $file | cut -d' ' -f1)
    is_ok=1
    for i in $(seq 1 $nb_line); do


        line=$(head -$i $file | tail -1 | sed "s/ //g")


        if [[ $line == *$'}'* ]] || [[ $line == *$'{'* ]]; then
            size=$(echo $line | wc -c)
            if [ $size -gt 3 ]; then
                echo "line:$line size:$size"
                echo -e "$RED$file$NORMAL line $i: braces must be on their own line $NORMAL"
                echo "$line"
                is_ok=0
            fi
        fi
    done

    if [ $is_ok -eq 1 ]; then
        echo -e "$YELLOW$file $GREEN [OK] $NORMAL"
    fi
done < ls_file

rm ls_file

echo -e "\n//////////////////////////////// INDENT_TEST ///////////////////////////////////"
touch ls_file
find ldso include -name *.c -o -name *.h > ls_file
while IFS=' ' read -r file; do
    nb_line=$(wc -l $file | cut -d' ' -f1)
    is_ok=1
    for i in $(seq 1 $nb_line); do

        nb_space=$(head -$i $file | tail -1 | awk -F'[^ ]' '{print length($1)}')
        if [ $(($nb_space % 4 )) -ne 0 ]; then
            echo -e "$RED$file$NORMAL line $i: indented by $nb_space spaces $NORMAL"
            echo "$line"
            is_ok=0
        fi
    done

    if [ $is_ok -eq 1 ]; then
        echo -e "$YELLOW$file $GREEN [OK] $NORMAL"
    fi
done < ls_file

rm ls_file


echo -e "\n//////////////////////////////// 80COL_TEST ///////////////////////////////////"
touch ls_file
find ldso src -name *.c -o -name *.h > ls_file
while IFS=' ' read -r file; do
    nb_line=$(wc -l $file | cut -d' ' -f1)
    is_ok=1
    for i in $(seq 1 $nb_line); do
        size=$(head -$i $file | tail -1 | wc -c)
        if [ $size -gt 80 ]; then
            echo -e "$RED$file$NORMAL line $i: size is $CLIGNOTE $size $NORMAL"
            echo "$(head -$i $file | tail -1)"
            is_ok=0
        fi
    done
    if [ $is_ok -eq 1 ]; then
        echo -e "$YELLOW$file $GREEN [OK] $NORMAL"
    fi
done < ls_file

rm ls_file

echo -e "\n//////////////////////////////// BLANK_TEST ///////////////////////////////////"
touch ls_file
find ldso include -name *.c -o -name *.h > ls_file
while IFS=' ' read -r file; do
    nb_line=$(wc -l $file | cut -d' ' -f1)
    is_ok=1
    for i in $(seq 1 $nb_line); do
        line=$(head -$i $file | tail -1)
        t_IFS=$IFS
        IFS=''
        size=$(echo $line | wc -c)
        IFS=$t_IFS
        size=$((size - 2))
        end_char="${line:size:1}"
        if [ "$end_char" == " " ]; then
            echo -e "$RED$file$NORMAL line $i: end char is blank$NORMAL"
            echo "$line"
            is_ok=0
        fi
    done

    if [ "$(head -1 $file)" == "" ]; then
        echo -e "$RED$file$NORMAL first line: blank"
        is_ok=0
    fi
    if [ "$(tail -1 $file)" == "" ]; then
        echo -e "$RED$file$NORMAL last line: blank"
        is_ok=0
    fi
    if [ $is_ok -eq 1 ]; then
        echo -e "$YELLOW$file $GREEN [OK] $NORMAL"
    fi
done < ls_file

rm ls_file

echo -e "\n/////////////////////////////////// STAT //////////////////////////////////////"
echo ""
echo -e "\nnb_line per file: \n$(wc -l $(find ldso include -name *.c -o -name *.h))"
echo -e "\n//////////////////////////////////// END //////////////////////////////////////"
