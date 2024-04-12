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
finde()
{
    TOTAL=$((TOTAL + 1))

#   find $1 2>/dev/null > "$REF_OUT"
    find $1 2>/dev/null > "$REF_OUT"
    findres=$(echo $?)

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
echo " === SIMPLE === "

finde "src"
finde "./"
finde ".."
finde "././"
finde "src/"
finde "src/*.c"
finde "*ak*e*"
finde "src/ -name *.o"
finde ". -name -print"
finde "-print"

echo
echo " === And Or === "

finde "tests/test_folder/* -name \"*i*.c\" -o -name \"*.a\" -a -name \"*01*\""
finde "tests/test_folder/ -name \"*o*\" -a -name \"f*\""
finde "tests/test_folder/ -name \"*o*\" -print -a -name \"f*\""
finde "tests/test_folder/ -name \"*.c\" -a -name \"*file*\" -o -name \"*0*\""
finde "tests/test_folder/ -name \"*.c\" -o -name \"*file*\" -a -name \"*0*\""

echo
echo " === Parenthesis === "
finde "tests/test_folder/ ( ( -name *.c ) -o ( -name *.a ) ) -a -name file*"
finde "tests/test_folder/ ( ( -name *.c ) -o -name *.a ) ( -name file* ) "
finde "tests/test_folder ( ( -name *.c -o -name *.a ) -o -name file* ) -a ( -name \"*s*\" )"
finde "tests/test_folder/ ( -name *.c -o -name *.a ) -a -name file*"

echo
echo " === Errors === "
finde "./././././././ -o"
finde "-o -print"
finde "-a -o -a"
finde " -name *.c -o"
finde " -name *.c -a"

finde "-type ff"

finde ". -print -perm 888"
finde ". -print -perm }122"

finde ". -user x"
finde ". -user students"
finde ". -group x"
finde ". -group quentin.nedelec"

echo
echo " === Type === "
finde "src/* -type f"
finde "src/* -type d"
finde "-type s"
finde "-type l,f,d"
finde "-type f,l"
finde "-type s,c"
finde "-type p,d,f"
finde "-type d,p,f,l"
finde "-type d,d"
finde "-type p,f,d,p"

echo
echo " === Newer === "
#touch ~/a.tmp
#touch ~/b.tmp
finde "/home/quentin.nedelec/*.tmp -newer /home/quentin.nedelec/a.tmp"
finde "/home/quentin.nedelec/*.tmp -newer /home/quentin.nedelec/b.tmp"

#rm ~/a.tmp
#rm ~/b.tmp

echo
echo " === Perm === "
finde "tests/test_folder/ -perm 644"
finde "tests/test_folder/ -perm 444"
finde "tests/test_folder/ -perm /644"
finde "tests/test_folder/ -perm /234"
finde "tests/test_folder/ -perm -644"
finde "tests/test_folder/ -perm -777"
finde "tests/test_folder/ -perm -070"
finde "tests/test_folder/ -perm /001"
finde "tests/test_folder/ -perm -000"
finde "tests/test_folder/ -perm /777"

echo
echo " === User & Group === "
finde ". -user quentin.nedelec"
finde ". -group students"
finde ". -group users"

echo
echo " === Not === "
finde " ! -name e*.c"
finde "tests/test_folder/ ! ( ! ( -name *.c ) -o ( -name *.a ) ) -a ! -name file*"
finde '-name e*.c ! -name e*s.c'
finde " ( ! ( -type d ) ! ( ! ( -type f ) ) )"
finde " ( -type p ) -o ! ( -type d ) -a ( -name src ) "
finde '( -type f -a ( -name "*.c" ) ) -o ( ! ( -type f ) -a ( -name src ) )'
finde " ! ! ! ! -name *.c"

echo
echo " === Tricky Parenthesis Errors === "
finde " ( -a )"
finde "-o ( -a )"
finde ") ("
finde "( )"
finde "( ) ( )"
finde "( ( -name \"*.c\" -o ) )"
finde "( ( -o -name \"*.c\" ) )"
finde "( ( -name o )"
finde "!"
finde " ! ( -name -o ! )"

echo
echo " === Options === "
finde "-d"
finde '-d  -name "*.c"'
finde "-d -name Makefile"
finde "-d -d -name Makefile"
finde "-H -L -P -L tests/ -type l"
finde "-L tests/"
finde "-H tests/"
finde "-P -L -H -L -P"
finde "-L"
finde "-L ../"
finde "-L tests/test_folder/foo/*"
finde '-L tests/test_folder/foo/* -newer tests/test_folder/foo/link3'
#finde '-H tests/test_folder/foo/* -newer tests/test_folder/foo/link'
finde '-L tests/test_folder/foo/* -newer tests/test_folder/foo/link'

echo
echo " === Exec === "
finde "-exec echo -- {} -- ;"
finde "tests/test_folder/* -exec echo \( coucou {} \) ; -name fo*"
finde "Makefile -exec echo a -exec echo b ;"
#finde "tests/test_folder/* -exec echo ( coucou {} ) ; -name fo* -print"
finde "tests/test_folder/* -exec wfuioeh ;"
finde "-exec ;"
finde "-exec echo ( {{}} ) ;"
finde "-exec . . . .  .. . . . . . .. . . . . . . .. . .. . . !!!! -a ;"
finde "Makefile ( -exec echo miam ) ) ; )"
finde "-exec echo {{}} {{}} ;"
finde "-exec echo {{{{{}}}}}}}{}{{{ ;"

echo
if [ $TOTAL -eq $PASSED ]; then
    echo "Test passed : " ${GREEN}$PASSED"/"$TOTAL${NC}
else
    echo "Test passed : " ${RED}$PASSED"/"$TOTAL${NC}
fi
