#ifndef NEWTON_INPUT_UTILS_HPP
#define NEWTON_INPUT_UTILS_HPP

#include <GLFW/glfw3.h>
#include <string>

namespace newton::input
{
	std::string keyToString(int key);
	std::string buttonToString(int button);

} // namespace newton::input

#endif // NEWTON_INPUT_UTILS_HPP
