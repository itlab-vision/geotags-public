# ------------------------------------
# Files
# ------------------------------------
STAGED_CPP := $(shell git diff --cached --name-only --diff-filter=ACM \
    | grep -E '\.(cpp|cc|h|hpp)$$' \
    | grep -vE '^(3rdparty|build)/')

FOLDERS := include samples scripts src tests
C_FILES := $(shell \
	for d in $(FOLDERS); do \
		if [ -d "$$d" ]; then \
			find "$$d" -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.h" -o -name "*.hpp" \); \
		fi; \
	done)


# ------------------------------------
# Helpers
# ------------------------------------
define log
    $(info [$1] $2)
endef

define report_format
	CHANGED=0; UNCHANGED=0; \
	for f in $(1); do \
		before=$$(md5sum $$f); \
		clang-format -i $$f; \
		after=$$(md5sum $$f); \
		if [ "$$before" != "$$after" ]; then \
			if [ $$CHANGED -eq 0 ]; then \
				echo "[CHANGED]:"; \
			fi; \
			echo "  $$f"; \
			CHANGED=$$((CHANGED+1)); \
		else \
			UNCHANGED=$$((UNCHANGED+1)); \
		fi; \
	done; \
	if [ $$UNCHANGED -gt 0 ]; then \
		echo "[UNCHANGED]: $$UNCHANGED files"; \
	fi
endef

define report_lint
	FAILED=0; PASSED=0; \
	for f in $(1); do \
		OUT=$$(cpplint --filter=-runtime/references,-build/c++11 $$f 2>&1 | grep -v '^Done processing'); \
		if [ -n "$$OUT" ]; then \
			if [ $$FAILED -eq 0 ]; then \
				echo "[FAILED]:"; \
			fi; \
			echo "  $$f"; \
			echo "$$OUT"; \
			FAILED=$$((FAILED+1)); \
		else \
			PASSED=$$((PASSED+1)); \
		fi; \
	done; \
	if [ $$PASSED -gt 0 ]; then \
		echo "[PASSED]: $$PASSED files"; \
	fi
endef

# ------------------------------------
# Targets
# ------------------------------------

fmt-st-c:
	$(call log,TASK,Formatting staged C++ files)
	@if [ -n "$(STAGED_CPP)" ]; then \
		$(call report_format,$(STAGED_CPP)); \
	else \
		echo "  (no staged C++ files)"; \
	fi

lint-st-c:
	$(call log,TASK,Linting staged C++ files)
	@if [ -n "$(STAGED_CPP)" ]; then \
		$(call report_lint,$(STAGED_CPP)); \
	else \
		echo "  (no staged C++ files)"; \
	fi

fmt+lint-st-c:
	@$(MAKE) -s fmt-st-c
	@echo ""
	@$(MAKE) -s lint-st-c

fmt-c:
	$(call log,TASK,Formatting all C++ files)
	@if [ -n "$(C_FILES)" ]; then \
		$(call report_format,$(C_FILES)); \
	else \
		echo "  (no C++ files)"; \
	fi

lint-c:
	$(call log,TASK,Linting all C++ files)
	@if [ -n "$(C_FILES)" ]; then \
		$(call report_lint,$(C_FILES)); \
	else \
		echo "  (no C++ files)"; \
	fi

fmt+lint-c:
	@$(MAKE) -s fmt-c
	@echo ""
	@$(MAKE) -s lint-c
