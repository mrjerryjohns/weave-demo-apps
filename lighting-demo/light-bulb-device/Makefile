#
#    Copyright (c) 2014-2018 Nest Labs, Inc.
#    Copyright (c) 2018 Google LLC
#    All rights reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

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

light-bulb-device: $(OBJ_FILES)
	@echo "LD " $@
	@g++ $(CFLAGS) -std=c++11 -g $^ -o $@ $(LDFLAGS)

all: light-bulb-device

clean:
	rm -rf light-bulb-device
	rm -rf .build
	rm -rf .depend
