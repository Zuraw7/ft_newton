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
	struct Vertex
	{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 texCoords;
	};

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

			VertexArray m_VAO;
			VertexBuffer m_VBO;
			IndexBuffer m_EBO;

			int m_indexCount;
	};

} // namespace newton::graphics

#endif // GRAPHICS_MESH_HPP
