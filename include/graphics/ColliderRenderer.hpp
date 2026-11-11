#ifndef GRAPHICS_COLLIDERRENDERER_HPP
#define GRAPHICS_COLLIDERRENDERER_HPP

#include <glm/ext/quaternion_float.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <string>
#include <vector>

#include "graphics/Camera.hpp"
#include "graphics/Shader.hpp"
#include "graphics/buffers/VertexArray.hpp"
#include "graphics/buffers/VertexBuffer.hpp"

namespace newton::graphics
{
	/**
	 * @brief Draws collider shapes as colored lines, to check that they match the meshes.
	 *
	 * Each shape is a unit line mesh built once (box from -1 to 1, sphere of radius 1 as three circles, plane as
	 * a grid with an arrow along its normal), placed and sized by a model matrix, so any number of colliders shares
	 * three small buffers. Every shape type has its own color.
	 *
	 * Every frame starts with @ref beginFrame(). Hidden by default, so the game starts without debug information.
	 * While hidden, all methods do nothing.
	 *
	 * Created only through @ref create(), so an existing object always holds a valid shader.
	 * Methods call OpenGL directly, so they can be used only after a successful @ref Context::create().
	 * Non-copyable.
	 */
	class ColliderRenderer
	{
		public:
			static std::unique_ptr<ColliderRenderer> create(const std::string &vertex, const std::string &fragment);
			~ColliderRenderer() = default;

			ColliderRenderer(const ColliderRenderer &) = delete;
			ColliderRenderer &operator=(const ColliderRenderer &) = delete;

			void visible(bool enabled);
			bool isVisible() const;

			void beginFrame(Camera &camera);
			void drawBox(const glm::vec3 &position, const glm::vec3 &halfExtents, const glm::quat &rotation);
			void drawSphere(const glm::vec3 &position, float radius);
			void drawPlane(const glm::vec3 &position, const glm::quat &rotation);

		private:
			/**
			 * @brief Line segments stored on the GPU, drawn as `GL_LINES` without indices.
			 *
			 * Each pair of vertices is one segment, so shared corners are repeated.
			 */
			struct LineMesh
			{
					explicit LineMesh(const std::vector<glm::vec3> &vertices);

					void draw() const;

					VertexArray VAO;         ///< Attribute state. Declared first, so it is bound before the buffer.
					VertexBuffer VBO;        ///< Vertex positions.
					std::size_t vertexCount; ///< Number of vertices, twice the number of segments.
			};

			explicit ColliderRenderer(std::unique_ptr<Shader> shader);

			std::unique_ptr<Shader> m_shader; ///< Shader drawing the lines in one color. Never null.

			bool m_visible; ///< False makes every method do nothing.

			LineMesh m_box;    ///< Box from -1 to 1 on each axis.
			LineMesh m_sphere; ///< Three circles of radius 1 in the XY, XZ and YZ planes.
			LineMesh m_plane;  ///< Grid from -1 to 1 on the XZ plane with an arrow along +Y.
	};

} // namespace newton::graphics

#endif // GRAPHICS_COLLIDERRENDERER_HPP
