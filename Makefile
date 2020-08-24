TOPTARGETS := all clean

SUBDIRS = lighting-demo third_party

.PHONY: $(TOPTARGETS) $(SUBDIRS)

all: weave app 

weave:
	@echo "-- Updating submodule..."
	@git submodule update --init
	@echo "-- Making Weave"
	$(MAKE) $@ -C third_party

app:
	$(MAKE) $@ -C lighting-demo	

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
		$(MAKE) -C $@ $(MAKECMDGOALS)
