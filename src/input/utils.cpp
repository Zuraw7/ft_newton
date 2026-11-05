#include <GLFW/glfw3.h>
#include <string>

namespace newton::input
{
	std::string keyToString(int key)
	{
		// 0-9
		if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
			return std::string(1, static_cast<char>('0' + key - GLFW_KEY_0));
		}

		// A-Z
		if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
			return std::string(1, static_cast<char>('A' + key - GLFW_KEY_A));
		}

		// F1-F25
		if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F25) {
			return "F" + std::to_string(key - GLFW_KEY_F1 + 1);
		}

		// Keypad keys
		if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9) {
			return "KP_" + std::to_string(key - GLFW_KEY_KP_0);
		}

		// Other
		switch (key) {
			case GLFW_KEY_SPACE : return "SPACE";
			case GLFW_KEY_APOSTROPHE : return "APOSTROPHE";
			case GLFW_KEY_COMMA : return "COMMA";
			case GLFW_KEY_MINUS : return "MINUS";
			case GLFW_KEY_PERIOD : return "PERIOD";
			case GLFW_KEY_SLASH : return "SLASH";
			case GLFW_KEY_SEMICOLON : return "SEMICOLON";
			case GLFW_KEY_EQUAL : return "EQUAL";

			case GLFW_KEY_LEFT_BRACKET : return "LEFT_BRACKET";
			case GLFW_KEY_BACKSLASH : return "BACKSLASH";
			case GLFW_KEY_RIGHT_BRACKET : return "RIGHT_BRACKET";
			case GLFW_KEY_GRAVE_ACCENT : return "GRAVE_ACCENT";

			case GLFW_KEY_WORLD_1 : return "WORLD_1";
			case GLFW_KEY_WORLD_2 : return "WORLD_2";

			case GLFW_KEY_ESCAPE : return "ESCAPE";
			case GLFW_KEY_ENTER : return "ENTER";
			case GLFW_KEY_TAB : return "TAB";
			case GLFW_KEY_BACKSPACE : return "BACKSPACE";
			case GLFW_KEY_INSERT : return "INSERT";
			case GLFW_KEY_DELETE : return "DELETE";

			case GLFW_KEY_RIGHT : return "RIGHT";
			case GLFW_KEY_LEFT : return "LEFT";
			case GLFW_KEY_DOWN : return "DOWN";
			case GLFW_KEY_UP : return "UP";

			case GLFW_KEY_PAGE_UP : return "PAGE_UP";
			case GLFW_KEY_PAGE_DOWN : return "PAGE_DOWN";
			case GLFW_KEY_HOME : return "HOME";
			case GLFW_KEY_END : return "END";

			case GLFW_KEY_CAPS_LOCK : return "CAPS_LOCK";
			case GLFW_KEY_SCROLL_LOCK : return "SCROLL_LOCK";
			case GLFW_KEY_NUM_LOCK : return "NUM_LOCK";
			case GLFW_KEY_PRINT_SCREEN : return "PRINT_SCREEN";
			case GLFW_KEY_PAUSE : return "PAUSE";

			case GLFW_KEY_LEFT_SHIFT : return "LEFT_SHIFT";
			case GLFW_KEY_LEFT_CONTROL : return "LEFT_CONTROL";
			case GLFW_KEY_LEFT_ALT : return "LEFT_ALT";
			case GLFW_KEY_LEFT_SUPER : return "LEFT_SUPER";

			case GLFW_KEY_RIGHT_SHIFT : return "RIGHT_SHIFT";
			case GLFW_KEY_RIGHT_CONTROL : return "RIGHT_CONTROL";
			case GLFW_KEY_RIGHT_ALT : return "RIGHT_ALT";
			case GLFW_KEY_RIGHT_SUPER : return "RIGHT_SUPER";

			case GLFW_KEY_MENU : return "MENU";

			case GLFW_KEY_KP_DECIMAL : return "KP_DECIMAL";
			case GLFW_KEY_KP_DIVIDE : return "KP_DIVIDE";
			case GLFW_KEY_KP_MULTIPLY : return "KP_MULTIPLY";
			case GLFW_KEY_KP_SUBTRACT : return "KP_SUBTRACT";
			case GLFW_KEY_KP_ADD : return "KP_ADD";
			case GLFW_KEY_KP_ENTER : return "KP_ENTER";
			case GLFW_KEY_KP_EQUAL : return "KP_EQUAL";

			default : break;
		}

		return "UNKNOWN";
	}

	std::string buttonToString(int button)
	{
		switch (button) {
			case GLFW_MOUSE_BUTTON_LEFT : return "LEFT";
			case GLFW_MOUSE_BUTTON_RIGHT : return "RIGHT";
			case GLFW_MOUSE_BUTTON_MIDDLE : return "MIDDLE";
		}

		if (button >= GLFW_MOUSE_BUTTON_1 && button <= GLFW_MOUSE_BUTTON_LAST)
			return "BUTTON_" + std::to_string(button + 1);

		return "UNKNOWN";
	}

} // namespace newton::input
