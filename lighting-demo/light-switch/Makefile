include ../Defines.mak

VPATH += ../common
VPATH += ../schema/zigbee/cluster/general

WEAVE_SRC_FILES := $(shell find . -type f -name '*.cpp')
WEAVE_SRC_FILES += $(shell find ../common -type f -name '*.cpp')
WEAVE_SRC_FILES += $(shell find ../schema -type f -name '*.cpp')

OBJ_FILES := $(patsubst ../common/%, %, $(WEAVE_SRC_FILES))
OBJ_FILES := $(patsubst ../schema/zigbee/cluster/general/%, %, $(OBJ_FILES))
OBJ_FILES := $(patsubst %.cpp, %.o, $(OBJ_FILES))
OBJ_FILES := $(patsubst ./%.o, %.o, $(OBJ_FILES))
OBJ_FILES := $(patsubst %.o, .build/%.o, $(OBJ_FILES))

CFLAGS := $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS := $(shell pkg-config --libs gtk+-3.0)

CFLAGS += $(shell pkg-config --cflags jsoncpp)
LDFLAGS += $(shell pkg-config --libs jsoncpp)

CFLAGS += -Wno-deprecated-declarations

CFLAGS += -I$(WEAVE_BASE)/usr/local/include -DWEAVE_WITH_OPENSSL=1 -I. -I../common -I../schema
LDFLAGS += -L$(WEAVE_BASE)/usr/local/lib -lWeave -lssl -lcrypto -lz -lnlfaultinjection

.build/%.o: ./%.cpp
	@echo "    CXX: " $<
	@mkdir -p $(@D)
	@g++ $(CFLAGS) -std=c++11 -g -c -o $@ $<

light-switch: $(OBJ_FILES)
	@echo "LD: " $@
	@g++ $(CFLAGS) -std=c++11 -g $^ -o $@ $(LDFLAGS)

all: light-switch

clean:
	rm -rf light-switch
	rm -rf .build
	rm -rf .depend
