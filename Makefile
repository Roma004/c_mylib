include make/common.mk

LIBS = $(sort $(dir $(wildcard */Makefile)))
INCLUDES = $(sort $(dir $(wildcard */include/)))

CFLAGS += $(addprefix -I../,$(INCLUDES))

.PHONY: all $(LIBS)
all: $(LIBS)

$(LIBS):
	@mkdir -p $(BUILD_DIR) $(DIST_DIR)
	$(MAKE) -C $@ all \
		BUILD_DIR="$(shell pwd)/$(BUILD_DIR)/$@" \
		DIST_DIR="$(shell pwd)/$(DIST_DIR)" \
		CFLAGS="$(CFLAGS)"

clean:
	rm -rf $(BUILD_DIR)/*
