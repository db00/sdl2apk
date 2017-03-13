# source $HOME/alchemy-ubuntu-v0.5a/alchemy-setup && alc-on
ALCHEMY_HOME:=$(HOME)/alchemy-ubuntu-v0.5a
ASC:=$(ALCHEMY_HOME)/bin/asc.jar 
PATH:=$(ALCHEMY_HOME)/achacks:$(ALCHEMY_HOME)/bin:$(PATH)
#SWFBRIDGE:=$(ALCHEMY_HOME)/bin/swfbridge
FLEX_HOME:=$(HOME)/flex_sdk_4.5
FLASHPLAYER:=$(FLEX_HOME)/bin/flashplayerdebugger
MXMLC:=$(FLEX_HOME)/bin/mxmlc
SWF:=test.swf
AS3_PATH:=.
AS3:=$(AS3_PATH)/test.as
SWC:=z.swc
SRC:=zip.c bytearray.c inflate.c crc32.c uncompr.c adler32.c inffast.c inftrees.c zutil.c
OBJS:= $(patsubst %.c,%.o,$(SRC))
CFLAGS:=-D as_api -O3 -Wall
CC:=$(ALCHEMY_HOME)/achacks/gcc

all: $(SWF)
#	echo $(shell ls /)
	$(FLASHPLAYER) $(SWF)
$(SWF): $(SWC) $(AS3)
	$(MXMLC) -source-path=$(AS3_PATH) -library-path+=$(SWC),md5.swc $(AS3) -o $(SWF)
$(SWC): $(OBJS)
	ALCHEMY_HOME=$(ALCHEMY_HOME) ASC=$(ASC) $(CC) $(OBJS) $(CFLAGS) -swc -o $(SWC)
.c.o:
	ALCHEMY_HOME=$(ALCHEMY_HOME) $(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.swc *.o *.swf *.achacks.* a.out *.dSYM
