#ifndef GEOMETRY_MESHDATA_HPP
#define GEOMETRY_MESHDATA_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>

namespace newton::geometry
{
	/**
	 * @brief Single vertex of a mesh.
	 *
	 * graphics::Mesh describes this layout to OpenGL as attributes 0 position, 1 normal, 2 texCoords.
	 * Adding or changing a field requires updating graphics::Mesh::setupMesh() and the shader inputs.
	 */
	struct Vertex
	{
			glm::vec3 position;  ///< Position in model space.
			glm::vec3 normal;    ///< Unit normal in model space.
			glm::vec2 texCoords; ///< Texture coordinates.
	};

	/**
	 * @brief Indexed triangle mesh on the CPU.
	 */
	struct MeshData
	{
			std::vector<Vertex> vertices;      ///< Vertex data.
			std::vector<unsigned int> indices; ///< Triangle indices, counter-clockwise for front faces.
	};

} // namespace newton::geometry

#endif // GEOMETRY_MESHDATA_HPP
