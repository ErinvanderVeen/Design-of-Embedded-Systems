INCLUDES:=-I $$CLEAN_HOME/lib/Platform -I $$CLEAN_HOME/lib/Generics -I $$CLEAN_HOME/lib/StdLib
FLAGS:=-l -no-pie -l -static -O
EXE:=Clock.out

XENO_CONFIG := /usr/xenomai/bin/xeno-config
CFLAGS := $(shell $(XENO_CONFIG)   --posix --alchemy --cflags)
LDFLAGS := -l -Wl,@/usr/xenomai/lib/cobalt.wrappers -l -Wl,@/usr/xenomai/lib/modechk.wrappers -l /usr/xenomai/lib/xenomai/bootstrap.o -l -Wl,--wrap=main -l -Wl,--dynamic-list=/usr/xenomai/lib/dynlist.ld -l -L/usr/xenomai/lib -l -lalchemy -l -lcopperplate -l -lrt -l -lcobalt -l -lmodechk -l -lpthread 

all: xenomai.o
	clm $(INCLUDES) $(LDFLAGS) -l xenomai.o ex09 -o $(EXE)

xenomai.o: xenomai.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	$(RM) -r "Clean System Files" $(EXE) xenomai.o

run: all
	sudo ./$(EXE)
