#ifndef GRAPHICS_BUFFERS_VERTEXBUFFER_HPP
#define GRAPHICS_BUFFERS_VERTEXBUFFER_HPP

#include <cstddef>
#include <vector>

namespace newton::graphics
{
	/**
	 * @brief Owner of an OpenGL vertex buffer object (VBO) with static data.
	 *
	 * Stores raw bytes. Requires a current OpenGL context. Non-copyable, movable.
	 */
	class VertexBuffer
	{
		public:
			explicit VertexBuffer(const void *data, std::size_t bytes);

			/**
			 * @brief Uploads the contents of a vector of any vertex type.
			 * @param vertices Vertex data, copied to the GPU.
			 */
			template <typename T>
			explicit VertexBuffer(const std::vector<T> &vertices)
				: VertexBuffer(vertices.data(), vertices.size() * sizeof(T))
			{}

			~VertexBuffer();

			VertexBuffer(VertexBuffer &&other) noexcept;
			VertexBuffer &operator=(VertexBuffer &&other) noexcept;

			VertexBuffer() = delete;
			VertexBuffer(const VertexBuffer &) = delete;
			VertexBuffer &operator=(const VertexBuffer &) = delete;

			void bind() const;
			void unbind() const;

		private:
			unsigned int m_id; ///< OpenGL buffer name, 0 after being moved from.
	};

} // namespace newton::graphics

#endif // GRAPHICS_BUFFERS_VERTEXBUFFER_HPP
