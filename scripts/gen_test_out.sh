# generates test output based on your machine.
# You should run this file, before starting to work on the luamemprofiler code.
#

TEST=cicle
lua tests/$TEST.lua > tests/out/$TEST.txt

TEST=dupfinalizer
lua tests/$TEST.lua > tests/out/$TEST.txt

TEST=eval
lua tests/$TEST.lua > tests/out/$TEST.txt

TEST=function
lua tests/$TEST.lua > tests/out/$TEST.txt

TEST=string
lua tests/$TEST.lua > tests/out/$TEST.txt

TEST=table
lua tests/$TEST.lua > tests/out/$TEST.txt

TEST=wfc
lua tests/$TEST.lua > tests/out/$TEST.txt

