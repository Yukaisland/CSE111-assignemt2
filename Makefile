#######################################################################
#
# Copyright (C) 2018-2021 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

#######################################################################
#
# Do not modify this file. 
#
# If you modify it and your code relies on those modifications, your code
# will not compile in the automated test harness and will be unable to
# execute any tests.
#
# If you want to use additional standard libraries, modify Makefile.libs
#
#######################################################################

include Makefile.libs

CFLAGS = --coverage -Wall -Iinclude -Llib -lbounds
CC = gcc -g

# Assignment No.
a = 2

SRC=$(wildcard src/*.c)

.PHONY: all tidy clean test

all: 
	-@make -s bounds
	@$(MAKE) -s tidy

tidy:
	@rm -f *.gcda 2>/dev/null
	@rm -f src/*.gcda src/*.gcno coverage.info 2>/dev/null

bounds: $(SRC) 
	@rm -f bounds 2>/dev/null
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) 2>&1 | tee make.out
	@mv *.gcno src
	@./bounds 
	@mv *.gcda src
	@lcov -q --capture --directory src --output-file coverage.info --no-external 2>&1 | tee coverage.out
	@genhtml coverage.info --output-directory coverage
	@$(MAKE) -s tidy

clean: tidy
	@rm -f bounds make.out

test: clean bounds

grade: clean 
	@./grade.sh $a
	@$(MAKE) -s tidy

submit: clean
	@echo ""
	@echo "#### Only these file are submitted:"
	@echo ""
	@tar czvf ~/CSE111-Assignment$(a).tar.gz \
		Makefile.libs src/*
	@echo ""
	@echo "##### Confirming submission is good..."
	@./confirm.sh $(a)
	@echo "#### If that failed, fix the problem and try again."
	@echo ""
	@echo "Finally, don't forget to upload ~/CSE111-Assignment$(a).tar.gz to Canvas!"
	@echo ""
