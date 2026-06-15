# Makefile wrapper for CMake project
# Targets: all, test, benchmark, demo, clean, install, format

BUILD_DIR := build
BUILD_TYPE ?= Release

.PHONY: all test benchmark demo clean install format help

all: $(BUILD_DIR)
	@cmake --build $(BUILD_DIR)

$(BUILD_DIR):
	@cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

test: $(BUILD_DIR)
	@cmake --build $(BUILD_DIR) --target test_correctness
	@$(BUILD_DIR)/test_correctness

benchmark: $(BUILD_DIR)
	@cmake --build $(BUILD_DIR) --target benchmark
	@$(BUILD_DIR)/benchmark

demo: $(BUILD_DIR)
	@cmake --build $(BUILD_DIR) --target demo
	@$(BUILD_DIR)/demo

clean:
# Windows (PowerShell/CMD) and Linux/macOS compatible cleanup
	@if exist $(BUILD_DIR) (rmdir /s /q $(BUILD_DIR) 2>nul || cmd /c "rmdir /s /q $(BUILD_DIR)")
	@if exist CMakeCache.txt del CMakeCache.txt 2>nul
	@if exist CMakeFiles rmdir /s /q CMakeFiles 2>nul

install: $(BUILD_DIR)
	@cmake --install $(BUILD_DIR)

format:
	@if command -v clang-format > /dev/null; then \
		find src tests examples include -type f \( -name "*.c" -o -name "*.h" \) -print0 | xargs -0 clang-format -i; \
		echo "Code formatted successfully."; \
	else \
		echo "Error: clang-format is not installed."; \
	fi

help:
	@echo "Available targets:"
	@echo "  all        - Build library and all executables (default)"
	@echo "  test       - Run correctness tests"
	@echo "  benchmark  - Run performance benchmarks"
	@echo "  demo       - Run demonstration"
	@echo "  clean      - Remove build directory"
	@echo "  install    - Install library and header"
	@echo "  format     - Format code with clang-format"
	@echo ""
	@echo "Build type: $(BUILD_TYPE) (use BUILD_TYPE=Debug for debug build)"