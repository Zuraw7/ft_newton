#include "newton/math/Vec3.hpp"

#include <cmath>

namespace newton::math
{
	/**
	 * @brief Creates the zero vector.
	 */
	Vec3::Vec3()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
	{}

	/**
	 * @brief Creates a vector from given components.
	 */
	Vec3::Vec3(float px, float py, float pz)
		: x(px)
		, y(py)
		, z(pz)
	{}

	/**
	 * @brief Flips the sign of every component, so the vector points the opposite way.
	 */
	void Vec3::invert()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	/**
	 * @brief Scales the vector to length 1, keeping its direction.
	 *
	 * The zero vector has no direction and is left unchanged.
	 */
	void Vec3::normalize()
	{
		const float d = magnitude();
		if (d > 0)
			(*this) *= 1.0f / d;
	}

	/**
	 * @return Copy of this vector scaled to length 1, or the zero vector if this vector is zero.
	 * @see normalize()
	 */
	Vec3 Vec3::normalized() const
	{
		Vec3 result = *this;
		result.normalize();
		return result;
	}

	/**
	 * @return Length of this vector, never negative.
	 */
	float Vec3::magnitude() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	/**
	 * @return Squared length of this vector.
	 */
	float Vec3::squareMagnitude() const
	{
		return x * x + y * y + z * z;
	}

	/**
	 * @brief Multiplies every component by @p scalar.
	 */
	Vec3 &Vec3::operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;

		return *this;
	}

	/**
	 * @return Copy of this vector with every component multiplied by @p scalar.
	 */
	Vec3 Vec3::operator*(float scalar) const
	{
		Vec3 scaled = *this;
		scaled *= scalar;
		return scaled;
	}

	/**
	 * @brief Divides every component by @p scalar, which must not be zero.
	 */
	Vec3 &Vec3::operator/=(float scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;

		return *this;
	}

	/**
	 * @return Copy of this vector with every component divided by @p scalar, which must not be zero.
	 */
	Vec3 Vec3::operator/(float scalar) const
	{
		Vec3 divided = *this;
		divided /= scalar;
		return divided;
	}

	/**
	 * @return Copy of this vector pointing the opposite way.
	 * @see invert()
	 */
	Vec3 Vec3::operator-() const
	{
		Vec3 inverted = *this;
		inverted.invert();
		return inverted;
	}

	/**
	 * @brief Adds @p other component by component.
	 */
	Vec3 &Vec3::operator+=(const Vec3 &other)
	{
		x += other.x;
		y += other.y;
		z += other.z;

		return *this;
	}

	/**
	 * @return Component-wise sum of this vector and @p other.
	 */
	Vec3 Vec3::operator+(const Vec3 &other) const
	{
		Vec3 added = *this;
		added += other;
		return added;
	}

	/**
	 * @brief Subtracts @p other component by component.
	 */
	Vec3 &Vec3::operator-=(const Vec3 &other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;

		return *this;
	}

	/**
	 * @return Component-wise difference of this vector and @p other. For two points it is the vector
	 * from @p other to this point.
	 */
	Vec3 Vec3::operator-(const Vec3 &other) const
	{
		Vec3 subtracted = *this;
		subtracted -= other;
		return subtracted;
	}

} // namespace newton::math

namespace newton::math
{
	/**
	 * @relates Vec3
	 * @return Scalar (dot) product of @p a and @p b, equal to |a| |b| cos θ.
	 *
	 * Positive when the vectors point roughly the same way, zero when they are perpendicular, negative when
	 * they point roughly opposite ways.
	 */
	float dot(const Vec3 &a, const Vec3 &b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	/**
	 * @relates Vec3
	 * @return Vector (cross) product of @p a and @p b: perpendicular to both.
	 *
	 * The result is not normalized. Its direction follows the right-hand rule, so the order of arguments
	 * matters: `cross(a, b) == -cross(b, a)`. Parallel vectors give the zero vector.
	 */
	Vec3 cross(const Vec3 &a, const Vec3 &b)
	{
		return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}

	/**
	 * @relates Vec3
	 * @return Component-wise product of @p a and @p b.
	 */
	Vec3 cwiseProduct(const Vec3 &a, const Vec3 &b)
	{
		return Vec3(a.x * b.x, a.y * b.y, a.z * b.z);
	}

	/**
	 * @relates Vec3
	 * @return Copy of @p vec with every component multiplied by @p scalar.
	 */
	Vec3 operator*(float scalar, const Vec3 &vec)
	{
		return vec * scalar;
	}

} // namespace newton::math
