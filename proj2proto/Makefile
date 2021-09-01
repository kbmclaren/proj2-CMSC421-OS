
SRCS = src/proto.c

BIN = mail_box

CFLAGS = -g -Wall -Wextra  -Wno-unused-parameter -Werror -std=gnu11

LDFLAGS = -g

###################################################################
# You shouldn't need to touch anything below this line...         #
###################################################################
BUILD_DIR = build/make
OBJS = $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(SRCS)))

.PHONY: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

.PRECIOUS: $(BUILD_DIR)/. $(BUILD_DIR)%/.

$(BUILD_DIR)/.:
	mkdir -p $@

$(BUILD_DIR)%/.:
	mkdir -p $@

all: $(BIN)

clean:
	-rm -rf $(BUILD_DIR)
	-rm -f $(BIN)

.SECONDEXPANSION:

$(BUILD_DIR)/%.o: %.c | $$(@D)/.
	$(CC) -c $< -o $@ $(CFLAGS)