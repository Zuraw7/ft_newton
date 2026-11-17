#ifndef NEWTON_MATH_VEC3_HPP
#define NEWTON_MATH_VEC3_HPP

namespace newton::math
{
	/**
	 * @brief Three-component vector of floats, used for both points and directions.
	 *
	 * Operations that need a length (normalization) leave the zero vector unchanged, because it has no
	 * direction. Products of two vectors are free functions: @ref dot(), @ref cross(), @ref cwiseProduct().
	 */
	struct Vec3
	{
			Vec3();
			Vec3(float px, float py, float pz);

			void invert();

			void normalize();
			Vec3 normalized() const;

			float magnitude() const;
			float squareMagnitude() const;

			Vec3 &operator*=(float scalar);
			Vec3 operator*(float scalar) const;
			Vec3 &operator/=(float scalar);
			Vec3 operator/(float scalar) const;
			Vec3 operator-() const;
			Vec3 &operator+=(const Vec3 &other);
			Vec3 operator+(const Vec3 &other) const;
			Vec3 &operator-=(const Vec3 &other);
			Vec3 operator-(const Vec3 &other) const;

			float x;
			float y;
			float z;
	};

	float dot(const Vec3 &a, const Vec3 &b);
	Vec3 cross(const Vec3 &a, const Vec3 &b);
	Vec3 cwiseProduct(const Vec3 &a, const Vec3 &b);

	Vec3 operator*(float scalar, const Vec3 &vec);

} // namespace newton::math

#endif // NEWTON_MATH_VEC3_HPP
