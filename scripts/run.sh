#############################################################################
# Author: Pablo Musa                                                        #
# Creation Date: mar 27 2011                                                #
# Last Modification: aug 09 2011                                            #
#                                                                           #
# Script for automated test                                                 #
#############################################################################

TESTS=tests
OUT=$TESTS/out

for i in $TESTS/*.lua
do
  i=`basename $i .lua`
#  echo "lua5.2 $TESTS/$i.lua > tmp.txt"
  lua5.2 $TESTS/$i.lua > tmp.txt
  echo "diff tmp.txt $OUT/$i.txt"
  diff $OUT/$i.txt tmp.txt
done

rm tmp.txt
