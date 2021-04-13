CXX=c++
RM=rm -f
CPPFLAGS=$(shell pkg-config --cflags ncursesw) -std=c++17 -g -Wall -Werror -Wextra
LDFLAGS=-g
LDLIBS=$(shell pkg-config --libs ncursesw)

SRCS=$(shell find $(SRC) -name '*.cc')
OBJS=$(subst .cc,.o,$(SRCS))

all: snake

snake: $(OBJS)
	$(CXX) $(LDFLAGS) -o snake $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) .depend

include .depend
