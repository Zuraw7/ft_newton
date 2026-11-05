#ifndef INPUTFUNCTIONS_HPP
#define INPUTFUNCTIONS_HPP

#include <GLFW/glfw3.h>
#include <string>

namespace newton::input
{
	void polygonModeManagement(GLFWwindow *window);
	std::string keyToString(int key);

} // namespace newton::input

#endif // INPUTFUNCTIONS_HPP
