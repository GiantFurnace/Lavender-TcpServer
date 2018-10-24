#author:chenzhengqiang
#generate date:2018/10/22 15:24:45


INCLUDE_DIR:=./include
SOURCE_DIR:=./src
LIB_DIR:=./lib

SUFFIX:=cpp
vpath %.h $(INCLUDE_DIR)
vpath %.$(SUFFIX) $(SOURCE_DIR)

TARGET:=lavender
CC0:=g++
CC1:=g++
#define the optimize level of compiler
OLEVEL=0
LDCONFIG:=-lpthread -lcactus -lnana

CFLAGS=-O$(OLEVEL) -pg -g -W -Wall -Wextra -Wconversion -Wshadow
OBJS:=lavender_samplecode tcpserver error
OBJS:=$(foreach obj,$(OBJS),$(obj).o)

INSTALL_DIR:=/usr/local/bin
CONFIG_PATH:=
SERVICE:=
CONFIG_INSTALL_PATH:=
TAR_NAME=$(TARGET)-$(shell date +%Y%m%d)

.PHONEY:clean
.PHONEY:install
.PHONEY:test
.PHONEY:tar

all:$(TARGET)
$(TARGET):$(OBJS)
	$(CC1) -o $@ $^ -L$(LIB_DIR) $(LDCONFIG)
$(OBJS):%.o:%.$(SUFFIX)
	$(CC0) -o $@ -c $< -I$(INCLUDE_DIR) $(CFLAGS)

clean:
	-rm -f *.o *.a *.so *.log *core* $(TARGET) *.tar.gz *.cppe *.out

install:
	-mv $(TARGET) $(INSTALL_DIR)
	-cp -f $(SERVICE) /etc/init.d/$(TARGET)
	-rm -rf $(CONFIG_INSTALL_PATH)
	-mkdir $(CONFIG_INSTALL_PATH)
	-cp -f $(CONFIG_PATH)/* $(CONFIG_INSTALL_PATH)

test:
	./$(TARGET)
tar:
	tar -cvzf $(TAR_NAME).tar.gz .
