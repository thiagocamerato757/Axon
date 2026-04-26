CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -D_DEFAULT_SOURCE -I./src -I./src/core -I./src/tests -I.
LDFLAGS = -lm

# ─────────────────────────────────────────
#  Sources
# ─────────────────────────────────────────

SRC_CORE = src/core/tensor.c src/core/random.c
OBJ_CORE = $(SRC_CORE:.c=.o)

# discover all test files in src/tests/
TEST_SRCS = $(wildcard src/tests/t_*.c)

# one binary per test file
# ex: src/tests/t_tensor.c → bin/t_tensor
TEST_BINS = $(patsubst src/tests/%.c, bin/%, $(TEST_SRCS))

# ─────────────────────────────────────────
#  Targets
# ─────────────────────────────────────────

.PHONY: all tests clean rebuild

all: $(TEST_BINS)

# compile all tests and run them one by one
tests: $(TEST_BINS)
	@echo ""
	@echo "╔══════════════════════════════════════╗"
	@echo "║         RUNNING ALL TESTS            ║"
	@echo "╚══════════════════════════════════════╝"
	@failed=0; \
	for bin in $(TEST_BINS); do \
		echo "\n--- $$bin ---"; \
		./$$bin; \
		if [ $$? -ne 0 ]; then failed=$$((failed + 1)); fi; \
	done; \
	echo ""; \
	if [ $$failed -eq 0 ]; then \
		echo "All test suites passed."; \
	else \
		echo "$$failed suite(s) failed."; \
		exit 1; \
	fi

# run a single test by name
# usage: make test NAME=t_tensor
test: bin/$(NAME)
	@./bin/$(NAME)

# ─────────────────────────────────────────
#  Build rules
# ─────────────────────────────────────────

# ensure bin/ directory exists
bin:
	mkdir -p bin

# compile each test binary linking against core objects
bin/%: src/tests/%.c $(OBJ_CORE) | bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# compile core object files
src/core/%.o: src/core/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ─────────────────────────────────────────
#  Util
# ─────────────────────────────────────────

clean:
	rm -f src/core/*.o bin/*

rebuild: clean all