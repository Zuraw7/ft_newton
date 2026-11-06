#ifndef GRAPHICS_MESH_HPP
#define GRAPHICS_MESH_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>

#include "graphics/buffers/IndexBuffer.hpp"
#include "graphics/buffers/VertexArray.hpp"
#include "graphics/buffers/VertexBuffer.hpp"

namespace newton::graphics
{
	/**
	 * @brief Single vertex as stored in a @ref VertexBuffer.
	 *
	 * The layout must match the attribute setup in @ref Mesh and the shader inputs
	 * (location 0 position, 1 normal, 2 texCoords).
	 */
	struct Vertex
	{
			glm::vec3 position;  ///< Position in model space.
			glm::vec3 normal;    ///< Unit normal in model space.
			glm::vec2 texCoords; ///< Texture coordinates.
	};

	/**
	 * @brief Indexed triangle mesh uploaded to the GPU.
	 *
	 * Owns its @ref VertexArray, @ref VertexBuffer and @ref IndexBuffer. Vertex data is not kept on the CPU,
	 * only the index count needed for drawing. Requires a current OpenGL context and must be destroyed
	 * before it. Non-copyable, movable.
	 */
	class Mesh
	{
		public:
			// TODO: Create geometry::MeshData object and pass it to constructor
			explicit Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
			Mesh(Mesh &&other) noexcept = default;
			Mesh &operator=(Mesh &&other) noexcept = default;
			~Mesh() = default;

			Mesh(const Mesh &other) = delete;
			Mesh &operator=(const Mesh &other) = delete;

			void draw() const;

		private:
			void setupMesh();

			VertexArray m_VAO;  ///< Attribute state. Declared first, so it is bound before the buffers are created.
			VertexBuffer m_VBO; ///< Vertex data.
			IndexBuffer m_EBO;  ///< Triangle indices.

			int m_indexCount; ///< Number of indices passed to glDrawElements.
	};

} // namespace newton::graphics

#endif // GRAPHICS_MESH_HPP
