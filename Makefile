PREFIX = $(MIX_APP_PATH)/priv
BUILD = $(MIX_APP_PATH)/obj

NIF = $(PREFIX)/my_app_nif.so

CFLAGS = -std=c++11 -fpic
LDFLAGS = -lpthread -dynamiclib -undefined dynamic_lookup

ERL_CFLAGS = -I$(ERL_EI_INCLUDE_DIR)
ERL_LDFLAGS = -L$(ERL_EI_LIBDIR) -lei

SRC = $(wildcard src/*.cpp)
HEADERS = $(wildcard src/*.h)
OBJ = $(SRC:src/%.cpp=$(BUILD)/%.o)

all: install

install: $(PREFIX) $(BUILD) $(NIF)

$(OBJ): $(HEADERS) Makefile

$(BUILD)/%.o: src/%.cpp
	$(CC) -c $(CFLAGS) $(ERL_CFLAGS) -o $@ $<

$(NIF): $(OBJ)
	$(CC) $(LDFLAGS) $(ERL_LDFLAGS) -o $@ $^

$(PREFIX) $(BUILD):
	mkdir -p $@

clean:
	$(RM) $(NIF) $(OBJ)

.PHONY: all clean install
