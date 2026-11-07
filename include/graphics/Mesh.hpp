#ifndef GRAPHICS_MESH_HPP
#define GRAPHICS_MESH_HPP

#include <vector>

#include "geometry/MeshData.hpp"
#include "graphics/buffers/IndexBuffer.hpp"
#include "graphics/buffers/VertexArray.hpp"
#include "graphics/buffers/VertexBuffer.hpp"

namespace newton::graphics
{
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
			explicit Mesh(const std::vector<geometry::Vertex> &vertices, const std::vector<unsigned int> &indices);
			explicit Mesh(const geometry::MeshData &data);
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
