#!/bin/sh
REF_OUT=".bash.out"
TEST_OUT=".myfind.out"

BLUE=$'\e[0;33m'
ORANGE=$'\e[1;34m'
GREEN=$'\e[0;32m'
RED=$'\e[0;31m'
NC=$'\e[0m'


PASSED=0
TOTAL=0

fastdir()
{

    mkdir delete 2>/dev/null 
    touch delete/a 2>/dev/null 
    touch delete/v 2>/dev/null 
    touch delete/okok 2>/dev/null 
    mkdir delete/ouiiiii 2>/dev/null 
    touch delete/ouiiiii/okok 2>/dev/null 

}

finde()
{
    TOTAL=$((TOTAL + 1))
    fastdir
#   find $1 2>/dev/null > "$REF_OUT"
    find $1 2>/dev/null > "$REF_OUT"
    findres=$(echo $?)

    fastdir
#   ./myfind $1 2>/dev/null > "$TEST_OUT"
   ./myfind $1 2>/dev/null > "$TEST_OUT"
   myfindres=$(echo $?)

   if [ $findres -ne $myfindres ]; then
       if [ $findres -eq 1 ]; then
           echo "[${ORANGE}X${NC}] ./myfind $1 | find returned an error but not myfind"
       else
           echo "[${ORANGE}X${NC}] ./myfind $1 | myfind returned an error but not find"
       fi
       return 0
   else

       COLOR=$GREEN
       if [ $findres -eq 1 ]; then
           COLOR=$BLUE
        fi

       #diff -u "$REF_OUT" "$TEST_OUT"
       DIFF=$(diff -u "$REF_OUT" "$TEST_OUT")

       if [ "$DIFF" != "" ]; then
           echo "[${RED}X${NC}] ./myfind $1"
       else
           echo "[${COLOR}V${NC}] ./myfind $1"
           PASSED=$((PASSED + 1))
       fi
   fi
}


echo
echo " === Delete === "

finde "delete -delete"
finde 'delete -name "*ok*" -delete'
finde 'delete/ouiiiii -print -delete'
finde 'delete -delete -delete'

echo
if [ $TOTAL -eq $PASSED ]; then
    echo "Delete tests passed : " ${GREEN}$PASSED"/"$TOTAL${NC}
else
    echo "Delete tests passed : " ${RED}$PASSED"/"$TOTAL${NC}
fi
