CC = gcc # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity

SRCDIR = src
BUILDDIR = build
TARGET = bin/AeroQual

SRCEXT = c
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall

FLAG = -pthread -lpthread -fno-builtin-exit -D _BSD_SOURCE -l sqlite3
LIB = -L/usr/local/lib
INC := -I/usr/local/include

$(TARGET): $(OBJECTS)
	@mkdir -p bin
	@mkdir -p measurement

	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB) $(FLAG)"; $(CC) $^ -o $(TARGET) $(LIB) $(FLAG)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BUILDDIR)/sqlite

	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC)  -c -o $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)
	@echo " $(RM) -r bin measurment"; $(RM) -r bin measurement




.PHONY: clean
# # Tests
# tester:
#   $(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester
#
# # Spikes
# ticket:
#   $(CC) $(CFLAGS) spikes/ticket.cpp $(INC) $(LIB) -o bin/ticket
