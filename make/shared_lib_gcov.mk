# requires $(PROJ_NAME_GCOV) be set (defines name of library generated)
# requires shared_lib.mk to be also included

GCOV_SO_NAME = lib$(PROJ_NAME_GCOV).so
GCOV_PROJ_LIB = $(DIST_DIR)/$(GCOV_SO_NAME)

GCOV_PROJ_OBJ = $(PROJ_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_gcov.o)

.PHONY: $(PROJ_NAME_GCOV)

$(PROJ_NAME_GCOV): $(GCOV_PROJ_LIB)

$(GCOV_PROJ_OBJ): $(BUILD_DIR)/%_gcov.o: $(SRC_DIR)/%.c $(PROJ_HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@ -fprofile-arcs -ftest-coverage

$(GCOV_PROJ_LIB): $(GCOV_PROJ_OBJ)
	@mkdir -p $(dir $(PROJ_LIB))
	$(CC) -shared  $(GCOV_PROJ_OBJ) -o $(GCOV_PROJ_LIB) --coverage
