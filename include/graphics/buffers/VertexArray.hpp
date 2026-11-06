#ifndef GRAPHICS_BUFFERS_VERTEXARRAY_HPP
#define GRAPHICS_BUFFERS_VERTEXARRAY_HPP

namespace newton::graphics
{
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
			unsigned int m_id;
	};

} // namespace newton::graphics

#endif // GRAPHICS_BUFFERS_VERTEXARRAY_HPP
