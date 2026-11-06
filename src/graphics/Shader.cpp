#include "graphics/Shader.hpp"

#include <glad/gl.h>

#include <glm/gtc/type_ptr.hpp>

#include <cstddef>
#include <fstream>
#include <memory>
#include <sstream>

#include "common/Logger.hpp"
#include "graphics/GLError.hpp"

namespace
{
	std::string parseShaderFile(const std::string &path);
	bool checkShaderCompilationStatus(unsigned int shader, const std::string &path);
	bool checkLinkingProgramStatus(unsigned int program);

	const char *cClassName = "Shader";
	newton::common::Logger &logger = newton::common::Logger::getInstance();

} // unnamed namespace

namespace newton::graphics
{
	common::Logger &Shader::m_logger = common::Logger::getInstance();

	/**
	 * @brief Loads, compiles and links a shader program.
	 *
	 * On failure the cause is logged and everything created so far is released.
	 *
	 * @param vertex Path to the vertex shader source, relative to the working directory.
	 * @param fragment Path to the fragment shader source, relative to the working directory.
	 * @return The shader, or nullptr on failure.
	 */
	std::unique_ptr<Shader> Shader::create(const std::string &vertex, const std::string &fragment)
	{
		unsigned int vertexShader = compileShader(vertex, GL_VERTEX_SHADER);
		unsigned int fragmentShader = compileShader(fragment, GL_FRAGMENT_SHADER);

		if (vertexShader == 0 || fragmentShader == 0) {
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return nullptr;
		}

		unsigned int id = glCreateProgram();
		if (id == 0) {
			m_logger.printf(common::LogLevel::ERROR, "[%s] Failed to create shader program\n", cClassName);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			return nullptr;
		}

		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);

		glLinkProgram(id);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		if (!checkLinkingProgramStatus(id)) {
			m_logger.printf(common::LogLevel::ERROR, "[%s] Failed to link program from: %s, %s\n", cClassName,
			                vertex.c_str(), fragment.c_str());
			glDeleteProgram(id);
			return nullptr;
		}

		graphics::checkGLError("Shader::create");

		return std::unique_ptr<Shader>(new Shader(id));
	}

	/**
	 * @brief Takes ownership of an already linked program. Called by @ref create().
	 */
	Shader::Shader(unsigned int id)
		: m_id(id)
	{}

	/**
	 * @brief Deletes the program.
	 */
	Shader::~Shader()
	{
		if (m_id)
			glDeleteProgram(m_id);
	}

	/**
	 * @brief Makes this program the one used for drawing and for the uniform setters.
	 */
	void Shader::bind() const
	{
		glUseProgram(m_id);
	}

	/**
	 * @brief Unbinds any program, not only this one.
	 */
	void Shader::unbind() const
	{
		glUseProgram(0);
	}

	/**
	 * @return OpenGL program name.
	 */
	unsigned int Shader::getId() const
	{
		return m_id;
	}

	/**
	 * @brief Sets an int uniform. The shader must be bound.
	 */
	void Shader::setInt(const std::string &name, int value)
	{
		const int location = findUniformLoc(name);
		glUniform1i(location, value);
	}

	/**
	 * @brief Sets a float uniform. The shader must be bound.
	 */
	void Shader::setFloat(const std::string &name, float value)
	{
		const int location = findUniformLoc(name);
		glUniform1f(location, value);
	}

	/**
	 * @brief Sets a vec3 uniform. The shader must be bound.
	 */
	void Shader::setVec3(const std::string &name, const glm::vec3 &vec)
	{
		const int location = findUniformLoc(name);
		glUniform3fv(location, 1, glm::value_ptr(vec));
	}

	/**
	 * @brief Sets a mat4 uniform. The shader must be bound.
	 */
	void Shader::setMat4(const std::string &name, const glm::mat4 &matrix)
	{
		const int location = findUniformLoc(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	/**
	 * @brief Loads and compiles a single shader stage.
	 *
	 * @param path Path to the shader source.
	 * @param type Shader stage, GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
	 * @return The compiled shader, or 0 on failure.
	 */
	unsigned int Shader::compileShader(const std::string &path, unsigned int type)
	{
		const std::string source = parseShaderFile(path);
		if (source.empty())
			return 0;

		unsigned int shader = glCreateShader(type);
		if (shader == 0) {
			m_logger.printf(common::LogLevel::ERROR, "[%s] Failed to create shader: %s\n", cClassName, path.c_str());
			return 0;
		}

		const char *src = source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		if (!checkShaderCompilationStatus(shader, path)) {
			glDeleteShader(shader);
			return 0;
		}

		return shader;
	}

	/**
	 * @brief Returns a uniform location, queried from OpenGL once and then taken from @ref m_uniformCache.
	 *
	 * A missing uniform is logged only on the first lookup.
	 *
	 * @return The location, or -1 if the uniform does not exist.
	 */
	int Shader::findUniformLoc(const std::string &name)
	{
		int location;
		const auto it = m_uniformCache.find(name);
		if (it != m_uniformCache.end()) {
			location = it->second;
		} else {
			location = glGetUniformLocation(m_id, name.c_str());
			m_uniformCache[name] = location;

			if (location == -1)
				m_logger.printf(common::LogLevel::WARN,
				                "[%s] Uniform '%s' not found in program [%u] (missing or unused)\n", cClassName,
				                name.c_str(), m_id);
		}
		return location;
	}

} // namespace newton::graphics

namespace
{
	/**
	 * @brief Reads a whole shader file.
	 * @return File contents, or an empty string if the file cannot be opened or is empty.
	 */
	std::string parseShaderFile(const std::string &path)
	{
		std::ifstream file(path.c_str());

		if (!file.is_open()) {
			logger.printf(newton::common::LogLevel::ERROR, "[%s] Error opening file: %s\n", cClassName, path.c_str());
			return "";
		}

		std::stringstream ss;
		ss << file.rdbuf();

		std::string source = ss.str();

		if (source.empty())
			logger.printf(newton::common::LogLevel::ERROR, "[%s] Shader file is empty: %s\n", cClassName, path.c_str());

		return source;
	}

	/**
	 * @brief Checks the compile status and logs the compiler output on failure.
	 * @param path Source path, printed in the log.
	 * @return True if the shader compiled.
	 */
	bool checkShaderCompilationStatus(unsigned int shader, const std::string &path)
	{
		int success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success) {
			int length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			std::string infoLog(static_cast<std::size_t>(length), '\0');

			glGetShaderInfoLog(shader, length, nullptr, infoLog.data());
			logger.printf(newton::common::LogLevel::ERROR, "[%s] Shader compilation error : %s : %s\n", cClassName,
			              path.c_str(), infoLog.c_str());

			return false;
		}

		return true;
	}

	/**
	 * @brief Checks the link status and logs the linker output on failure.
	 * @return True if the program linked.
	 */
	bool checkLinkingProgramStatus(unsigned int program)
	{
		int success = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &success);

		if (!success) {
			int length = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

			std::string infoLog(static_cast<std::size_t>(length), '\0');

			glGetProgramInfoLog(program, length, nullptr, infoLog.data());
			logger.printf(newton::common::LogLevel::ERROR, "[%s] Failed to link shader program [%u], %s\n", cClassName,
			              program, infoLog.c_str());

			return false;
		}

		return true;
	}

} // unnamed namespace
