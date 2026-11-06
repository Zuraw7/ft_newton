NAME       := ft_newton
BUILD_TYPE ?= Release
BUILD_DIR  := build/$(shell echo $(BUILD_TYPE) | tr '[:upper:]' '[:lower:]')

all: $(NAME)

configure:
	cmake -S . -B $(BUILD_DIR) -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

$(NAME): configure
	cmake --build $(BUILD_DIR) -j$$(nproc)
	cp $(BUILD_DIR)/$(NAME) $(NAME)

docs: configure
	cmake --build $(BUILD_DIR) --target docs

clean:
	rm -rf build

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all configure $(NAME) docs clean fclean re
