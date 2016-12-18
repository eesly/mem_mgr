DEBUG=0
STATIC=0
CC=gcc
CXX=g++

CPPFLAGS+= -D_TEST_
LDFLAGS+=

ifeq ($(DEBUG), 1)
	CPPFLAGS += -g3 -DDEBUG
else
	CPPFLAGS += -g3 -O3 -Os -DNDEBUG
endif

INCPATH += -I./

CPPFLAGS+= $(INCPATH)

ifeq ($(STATIC), 1)
	LDFLAGS += -static-libgcc
endif

LDFLAGS+= -lrt -pthread

MEM_OBJ += CMemMgr.o CMemOperator.o CMemLeakChecker.o CUtils.o
MEM_OBJ += CMemStatistics.o

TEST = test

.PHONY:all
all: $(TEST)

$(TEST): $(MEM_OBJ) main.o
	$(CXX) $(CPPFLAGS) -o $(TEST) $^ $(LDFLAGS)


.PHONY:clean
clean:
	rm -rf $(MEM_OBJ)
	rm -rf $(TEST) main.o
