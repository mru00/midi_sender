
TARGET = midi_usb
CC = g++
GCC = gcc -Wall -g
LDFLAGS = `libusb-config --libs`
ALL_CFLAGS = -Wall -g -ggdb `libusb-config --cflags`
RM = rm -f
FLEX = flex
BISON = bison -d --debug --verbose


LEXER = parse.l
PARSER = parse

DEPENDS = dependencies.mk
SRC = main.cpp parse.tab.cpp usb.cpp


OBJ = $(SRC:.cpp=.o)

all: build

clean:
	@echo cleaing...
	@$(RM) *~
	@$(RM) *.o
	@$(RM) parser.tab.*
	@$(RM) parser.output
	@$(RM) dependencies.mk
	@$(RM) lex.yy.c
	@$(RM) $(TARGET)

build: depend $(TARGET)

lex.yy.c: $(LEXER)
	@echo     [FL] $<
	@$(FLEX) $(LEXER)

$(PARSER).tab.cpp: $(PARSER).ypp  lex.yy.c
	@echo [BI] $<
	@$(BISON) $(PARSER).ypp


$(TARGET): $(OBJ)
	@echo [LD] $@
	@$(CC) $(ALL_CFLAGS) $(OBJ) --output $@ $(LDFLAGS)


.cpp.o:  depend $(SRC) midi_events.hpp
	@echo [CC] $<
	@$(CC) $(ALL_CFLAGS) -c $< -o $@

test: build
	./$(TARGET) 2>&1
	cp hexdump_test hexdump_old
	hd a.out > hexdump_test
#	ghex2 a.out &
#killall meld
	meld hexdump_good hexdump_test&

debug: build
	cp nasm ../thegame/
	cd ../thegame && ddd ./nasm &

commit: 
	cvs commit

.PHONY: all build clean depend test commit debug

depend:
	@makedepend -f - $(SRC) > $(DEPENDS) 2> /dev/null
