# requires $(PROJ_NAME) be set (defines name of library generated)

SO_NAME = lib$(PROJ_NAME).so
PROJ_LIB = $(DIST_DIR)/$(SO_NAME)

PROJ_FILES = $(wildcard $(SRC_DIR)/*.c)
PROJ_HEADERS = $(wildcard $(INCLUDE_DIR)/$(PROJ_NAME)/*.h)
PROJ_OBJ = $(PROJ_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all $(PROJ_NAME)

all: $(PROJ_NAME)

$(PROJ_NAME): $(PROJ_LIB)

$(PROJ_OBJ): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(PROJ_HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(PROJ_LIB): $(PROJ_OBJ)
	@mkdir -p $(dir $(PROJ_LIB))
	$(CC) -shared  $(PROJ_OBJ) -o $(PROJ_LIB)
