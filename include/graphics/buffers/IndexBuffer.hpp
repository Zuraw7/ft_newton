#ifndef GRAPHICS_BUFFERS_INDEXBUFFER_HPP
#define GRAPHICS_BUFFERS_INDEXBUFFER_HPP

#include <cstddef>
#include <vector>

namespace newton::graphics
{
	/**
	 * @brief Owner of an OpenGL index buffer object (EBO) with static unsigned int indices.
	 *
	 * The GL_ELEMENT_ARRAY_BUFFER binding is part of the VAO state, so the buffer must be bound
	 * while the target VAO is bound. Requires a current OpenGL context. Non-copyable, movable.
	 */
	class IndexBuffer
	{
		public:
			explicit IndexBuffer(const unsigned int *indices, std::size_t count);
			explicit IndexBuffer(const std::vector<unsigned int> &indices);
			~IndexBuffer();

			IndexBuffer(IndexBuffer &&other) noexcept;
			IndexBuffer &operator=(IndexBuffer &&other) noexcept;

			IndexBuffer() = delete;
			IndexBuffer(const IndexBuffer &) = delete;
			IndexBuffer &operator=(const IndexBuffer &) = delete;

			void bind() const;
			void unbind() const;

		private:
			unsigned int m_id; ///< OpenGL buffer name, 0 after being moved from.
	};

} // namespace newton::graphics

#endif // GRAPHICS_BUFFERS_INDEXBUFFER_HPP
