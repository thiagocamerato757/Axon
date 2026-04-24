CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -D_DEFAULT_SOURCE -I./src -I./src/core -I.
LDFLAGS = -lm

SRC_CORE = src/core/tensor.c src/core/random.c
SRC_TEST = src/tests/t_tensor.c

OBJ = $(SRC_CORE:.c=.o) $(SRC_TEST:.c=.o)

TARGET = test_tensor

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# regra para core
src/core/%.o: src/core/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# regra para tests
src/tests/%.o: src/tests/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f src/core/*.o src/tests/*.o $(TARGET)

rebuild: clean all