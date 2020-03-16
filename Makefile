CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic
TARGETS = $(pathsubst %.c,%,$(wildcard *.c))

define make_target
$(1): $(1).o
	$(CC) $(CFLAGS) $$^ -o $$@
endef

.PHONY: all clean
all: $(TARGETS)

ifdef DEV_INFO
DEV_FLAGS = -save-temps
endif

$(foreach TARGET,$(TARGETS),$(eval $(call make_target,$(TARGET))))

%.o: %.c
	$(CC) $(CFLAGS) $(DEV_FLAGS) -MMD -c $< -o $@

clean:
	rm -f $(TARGETS) *.o *.d *.i *.s

-include $(TARGETS:=.d)
