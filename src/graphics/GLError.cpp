#include "graphics/GLError.hpp"

#include <glad/gl.h>

#include "common/Logger.hpp"

namespace
{
	newton::common::Logger &logger = newton::common::Logger::getInstance();
	const char *glErrorToString(unsigned int code);

} // unnamed namespace

namespace newton::graphics
{
	/**
	 * @brief Logs and clears all pending OpenGL errors.
	 * @param where Label of the checked place, printed in the log.
	 * @return True if any error was logged.
	 */
	bool checkGLError(const char *where)
	{
		bool hadError = false;
		unsigned int code;

		while ((code = glGetError()) != GL_NO_ERROR) {
			logger.printf(common::LogLevel::ERROR, "[GL] %s: %s (0x%X)\n", where, glErrorToString(code), code);

			hadError = true;
		}

		return hadError;
	}

} // namespace newton::graphics

namespace
{
	const char *glErrorToString(unsigned int code)
	{
		switch (code) {
			case GL_INVALID_ENUM : return "GL_INVALID_ENUM";
			case GL_INVALID_VALUE : return "GL_INVALID_VALUE";
			case GL_INVALID_OPERATION : return "GL_INVALID_OPERATION";
			case GL_INVALID_FRAMEBUFFER_OPERATION : return "GL_INVALID_FRAMEBUFFER_OPERATION";
			case GL_OUT_OF_MEMORY : return "GL_OUT_OF_MEMORY";
		}

		return "UNKNOWN";
	}

} // unnamed namespace
