#ifndef GRAPHICS_BUFFERS_INDEXBUFFER_HPP
#define GRAPHICS_BUFFERS_INDEXBUFFER_HPP

#include <cstddef>
#include <vector>

namespace newton::graphics
{
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
			unsigned int m_id;
	};

} // namespace newton::graphics

#endif // GRAPHICS_BUFFERS_INDEXBUFFER_HPP
