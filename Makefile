
CURRENT_DIR = $(shell pwd)


all: build

install_dep: $(CURRENT_DIR)/build/conanbuildinfo.cmake

$(CURRENT_DIR)/build/conanbuildinfo.cmake:
	pip install conan
	mkdir -p $(CURRENT_DIR)/build && \
	cd $(CURRENT_DIR)/build && conan install $(CURRENT_DIR)

build: install_dep
	mkdir -p $(CURRENT_DIR)/build && \
	cd $(CURRENT_DIR)/build && cmake $(CURRENT_DIR) && cmake --build $(CURRENT_DIR)/build