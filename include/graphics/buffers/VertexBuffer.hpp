#ifndef GRAPHICS_BUFFERS_VERTEXBUFFER_HPP
#define GRAPHICS_BUFFERS_VERTEXBUFFER_HPP

#include <cstddef>
#include <vector>

namespace newton::graphics
{
	class VertexBuffer
	{
		public:
			explicit VertexBuffer(const void *data, std::size_t bytes);
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
			unsigned int m_id;
	};

} // namespace newton::graphics

#endif // GRAPHICS_BUFFERS_VERTEXBUFFER_HPP
