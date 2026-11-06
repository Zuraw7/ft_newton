#ifndef GRAPHICS_SHADER_HPP
#define GRAPHICS_SHADER_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "common/Logger.hpp"

namespace newton::graphics
{
	/**
	 * @brief Owner of a linked OpenGL shader program built from a vertex and a fragment shader file.
	 *
	 * Created only through @ref create(), so an existing object always holds a valid program.
	 * Uniform setters apply to the currently used program, so call @ref bind() first.
	 * Requires a current OpenGL context. Non-copyable and non-movable.
	 */
	class Shader
	{
		public:
			static std::unique_ptr<Shader> create(const std::string &vertex, const std::string &fragment);
			~Shader();

			Shader(const Shader &other) = delete;
			Shader(Shader &&other) = delete;

			Shader &operator=(const Shader &other) = delete;
			Shader &operator=(Shader &&other) = delete;

			void bind() const;
			void unbind() const;

			unsigned int getId() const;

			void setInt(const std::string &name, int value);
			void setFloat(const std::string &name, float value);
			void setVec3(const std::string &name, const glm::vec3 &vec);
			void setMat4(const std::string &name, const glm::mat4 &matrix);

		private:
			explicit Shader(unsigned int id);

			static unsigned int compileShader(const std::string &path, unsigned int type);
			int findUniformLoc(const std::string &name);

			unsigned int m_id; ///< OpenGL program name.

			std::unordered_map<std::string, int> m_uniformCache; ///< Uniform locations by name, -1 for missing ones.

			static common::Logger &m_logger;
	};

} // namespace newton::graphics

#endif // GRAPHICS_SHADER_HPP
