INCLUDE  := -Iinclude/
INCLUDE_DIR := include
INCLUDE_FILE := $(wildcard $(INCLUDE_DIR)/*.h)
SRCDIR = src
SRC      := $(wildcard $(SRCDIR)/*.cc)         

.PHONY:format

format:
	clang-format \
	-style=\
	"{BasedOnStyle: Google,BreakBeforeBraces: Stroustrup, IndentWidth: 4, IndentCaseLabels: false, TabWidth: 4, UseTab: ForIndentation, ColumnLimit: 0}"\
	 -i $(SRC) $(wildcard $(INCLUDE_DIR)/*.cc) $(wildcard $(INCLUDE_DIR)/*.h)
