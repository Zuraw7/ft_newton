#include "physics/Collision.hpp"

#include <cmath>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <optional>
#include <variant>

#include "physics/Collider.hpp"
#include "physics/RigidBody.hpp"

namespace
{
	using newton::physics::Collider;
	using newton::physics::Contact;
	using newton::physics::RigidBody;

	constexpr float cEpsilon = 1e-6f;
	constexpr glm::vec3 cPlaneNormal = {0.0f, 1.0f, 0.0f};

	std::optional<Contact> sphereSphereTest(const glm::vec3 &posA, const Collider::Sphere &sphereA,
	                                        const glm::vec3 &posB, const Collider::Sphere &sphereB);
	std::optional<Contact> spherePlaneTest(const glm::vec3 &spherePos, const Collider::Sphere &sphereCol,
	                                       const glm::vec3 &planePos, const glm::quat &planeRot);

	/**
	 * @brief Calls the test matching the shapes of two bodies, for `std::visit` over both colliders.
	 *
	 * Pairs without a test yet fall to the template and never collide.
	 */
	struct CollisionDispatcher
	{
			const RigidBody &a;
			const RigidBody &b;

			std::optional<Contact> operator()(const Collider::Sphere &sphereA, const Collider::Sphere &sphereB) const
			{
				return sphereSphereTest(a.position, sphereA, b.position, sphereB);
			}

			std::optional<Contact> operator()(const Collider::Sphere &sphere, const Collider::Plane &) const
			{
				return spherePlaneTest(a.position, sphere, b.position, b.rotation);
			}

			// Reversed pair: the sphere is body b, so positions are swapped with the shapes and the normal flipped
			std::optional<Contact> operator()(const Collider::Plane &, const Collider::Sphere &sphere) const
			{
				std::optional<Contact> contact = spherePlaneTest(b.position, sphere, a.position, a.rotation);
				if (contact)
					contact->normal = -contact->normal;
				return contact;
			}

			template <typename ShapeA, typename ShapeB>
			std::optional<Contact> operator()(const ShapeA &, const ShapeB &) const
			{
				return std::nullopt;
			}
	};

} // unnamed namespace

namespace newton::physics
{
	/**
	 * @brief Tests whether two bodies overlap, using the test for their pair of shapes.
	 *
	 * @note The body indices in the returned contact are always 0 and must be set by the caller.
	 *
	 * @return Contact with the normal from A to B, or empty if the bodies do not overlap.
	 */
	std::optional<Contact> detectCollision(const RigidBody &a, const RigidBody &b)
	{
		return std::visit(CollisionDispatcher{a, b}, a.collider.shape, b.collider.shape);
	}

} // namespace newton::physics

namespace
{
	/**
	 * @brief Tests two spheres for overlap.
	 *
	 * Touching spheres (distance equal to the sum of radii) count as a contact with zero penetration.
	 * When the centers are at the same point, the normal is +Y, because no direction can be derived from them.
	 *
	 * @return Contact with the normal from A to B, or empty if the spheres do not overlap. Body indices are not set.
	 */
	std::optional<Contact> sphereSphereTest(const glm::vec3 &posA, const Collider::Sphere &sphereA,
	                                        const glm::vec3 &posB, const Collider::Sphere &sphereB)
	{
		const glm::vec3 aToB = posB - posA;
		const float radiusSum = sphereA.radius + sphereB.radius;
		const float distanceSquared = glm::dot(aToB, aToB);

		if (distanceSquared > radiusSum * radiusSum)
			return std::nullopt;

		const float distance = std::sqrt(distanceSquared);
		Contact contact;
		contact.normal = distance > cEpsilon ? aToB / distance : glm::vec3(0.0f, 1.0f, 0.0f);
		contact.penetration = radiusSum - distance;
		return contact;
	}

	/**
	 * @brief Tests a sphere against an infinite plane.
	 *
	 * The plane has one side: a sphere whose center is behind it still collides, with a penetration greater than
	 * its radius, so it is pushed back to the side the normal points to.
	 *
	 * @param planePos Any point on the plane.
	 * @param planeRot Rotation of the plane body, turning the local +Y axis into the plane normal.
	 * @return Contact with the normal from the sphere to the plane (the plane normal reversed), or empty if they do
	 *         not overlap. Body indices are not set.
	 */
	std::optional<Contact> spherePlaneTest(const glm::vec3 &spherePos, const Collider::Sphere &sphereCol,
	                                       const glm::vec3 &planePos, const glm::quat &planeRot)
	{
		const glm::vec3 planeToSphere = spherePos - planePos;
		const glm::vec3 normal = planeRot * cPlaneNormal;
		const float signedDistance = glm::dot(normal, planeToSphere);
		if (signedDistance > sphereCol.radius)
			return std::nullopt;

		Contact contact;
		contact.normal = -normal;
		contact.penetration = sphereCol.radius - signedDistance;
		return contact;
	}

} // unnamed namespace
