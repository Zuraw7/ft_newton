#ifndef GRAPHICS_BUFFERS_VERTEXARRAY_HPP
#define GRAPHICS_BUFFERS_VERTEXARRAY_HPP

namespace newton::graphics
{
	/**
	 * @brief Owner of an OpenGL vertex array object (VAO).
	 *
	 * The VAO stores the vertex attribute layout, the buffer each attribute reads from and the index buffer
	 * binding, so binding it alone is enough to draw. Requires a current OpenGL context.
	 * Non-copyable, movable.
	 */
	class VertexArray
	{
		public:
			VertexArray();
			~VertexArray();

			VertexArray(VertexArray &&other) noexcept;
			VertexArray &operator=(VertexArray &&other) noexcept;

			VertexArray(const VertexArray &) = delete;
			VertexArray &operator=(const VertexArray &) = delete;

			void bind() const;
			void unbind() const;

		private:
			unsigned int m_id; ///< OpenGL vertex array name, 0 after being moved from.
	};

} // namespace newton::graphics

#endif // GRAPHICS_BUFFERS_VERTEXARRAY_HPP
