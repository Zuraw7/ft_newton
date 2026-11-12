#include "physics/Collision.hpp"

#include <cmath>
#include <cstddef>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <variant>
#include <vector>

#include "physics/Collider.hpp"
#include "physics/RigidBody.hpp"

namespace
{
	using newton::physics::Collider;
	using newton::physics::Contact;
	using newton::physics::RigidBody;

	constexpr float cEpsilon = 1e-6f;
	constexpr glm::vec3 cPlaneNormal = {0.0f, 1.0f, 0.0f};

	void sphereSphereTest(const glm::vec3 &posA, const Collider::Sphere &sphereA, const glm::vec3 &posB,
	                      const Collider::Sphere &sphereB, std::vector<Contact> &contacts);
	void spherePlaneTest(const glm::vec3 &spherePos, const Collider::Sphere &sphereCol, const glm::vec3 &planePos,
	                     const glm::quat &planeRot, std::vector<Contact> &contacts);
	void boxPlaneTest(const glm::vec3 &boxPos, const Collider::Box &boxCol, const glm::quat &boxRot,
	                  const glm::vec3 &planePos, const glm::quat &planeRot, std::vector<Contact> &contacts);

	void reverseNormals(std::size_t first, std::vector<Contact> &contacts);

	/**
	 * @brief Calls the test matching the shapes of two bodies, for `std::visit` over both colliders.
	 *
	 * Pairs without a test yet fall to the template and never collide.
	 */
	struct CollisionDispatcher
	{
			const RigidBody &a;
			const RigidBody &b;
			std::vector<Contact> &contacts;

			void operator()(const Collider::Sphere &sphereA, const Collider::Sphere &sphereB) const
			{
				sphereSphereTest(a.position, sphereA, b.position, sphereB, contacts);
			}

			void operator()(const Collider::Sphere &sphere, const Collider::Plane &) const
			{
				spherePlaneTest(a.position, sphere, b.position, b.rotation, contacts);
			}

			// Reversed pair: the sphere is body b, so positions are swapped with the shapes and the normals reversed
			void operator()(const Collider::Plane &, const Collider::Sphere &sphere) const
			{
				const std::size_t first = contacts.size();
				spherePlaneTest(b.position, sphere, a.position, a.rotation, contacts);

				reverseNormals(first, contacts);
			}

			void operator()(const Collider::Box &box, const Collider::Plane &) const
			{
				boxPlaneTest(a.position, box, a.rotation, b.position, b.rotation, contacts);
			}

			void operator()(const Collider::Plane &, const Collider::Box &box) const
			{
				const std::size_t first = contacts.size();
				boxPlaneTest(b.position, box, b.rotation, a.position, a.rotation, contacts);

				reverseNormals(first, contacts);
			}

			template <typename ShapeA, typename ShapeB>
			void operator()(const ShapeA &, const ShapeB &) const
			{}
	};

} // unnamed namespace

namespace newton::physics
{
	/**
	 * @brief Tests whether two bodies overlap, using the test for their pair of shapes.
	 *
	 * Appends the found contacts, with the normal from A to B, to the end of the vector. Adds nothing if the bodies
	 * do not overlap. Contacts already in the vector are not changed.
	 *
	 * @note The body indices in the added contacts are always 0 and must be set by the caller.
	 *
	 * @param a First body.
	 * @param b Second body.
	 * @param contacts Vector the contacts are appended to.
	 */
	void detectCollision(const RigidBody &a, const RigidBody &b, std::vector<Contact> &contacts)
	{
		std::visit(CollisionDispatcher{a, b, contacts}, a.collider.shape, b.collider.shape);
	}

} // namespace newton::physics

namespace
{
	/**
	 * @brief Tests two spheres for overlap.
	 *
	 * Touching spheres (distance equal to the sum of radii) count as a contact with zero penetration.
	 * When the centers are at the same point, the normal is +Y, because no direction can be derived from them.
	 * The contact point is in the middle of the overlap, on the line between the centers, so it also fits spheres
	 * of different sizes.
	 *
	 * @param contacts Vector one contact is appended to, with the normal from A to B, if the spheres overlap.
	 *        Body indices are not set.
	 */
	void sphereSphereTest(const glm::vec3 &posA, const Collider::Sphere &sphereA, const glm::vec3 &posB,
	                      const Collider::Sphere &sphereB, std::vector<Contact> &contacts)
	{
		const glm::vec3 aToB = posB - posA;
		const float radiusSum = sphereA.radius + sphereB.radius;
		const float distanceSquared = glm::dot(aToB, aToB);

		if (distanceSquared > radiusSum * radiusSum)
			return;

		const float distance = std::sqrt(distanceSquared);
		Contact contact;
		contact.normal = distance > cEpsilon ? aToB / distance : glm::vec3(0.0f, 1.0f, 0.0f);
		contact.penetration = radiusSum - distance;
		contact.point = posA + contact.normal * (sphereA.radius - contact.penetration / 2);
		contacts.push_back(contact);
	}

	/**
	 * @brief Tests a sphere against an infinite plane.
	 *
	 * The plane has one side: a sphere whose center is behind it still collides, with a penetration greater than
	 * its radius, so it is pushed back to the side the normal points to.
	 *
	 * @param planePos Any point on the plane.
	 * @param planeRot Rotation of the plane body, turning the local +Y axis into the plane normal.
	 * @param contacts Vector one contact is appended to if they overlap, with the normal from the sphere to the
	 *        plane (the plane normal reversed) and the center of the sphere projected onto the plane as the contact
	 *        point. Body indices are not set.
	 */
	void spherePlaneTest(const glm::vec3 &spherePos, const Collider::Sphere &sphereCol, const glm::vec3 &planePos,
	                     const glm::quat &planeRot, std::vector<Contact> &contacts)
	{
		const glm::vec3 planeToSphere = spherePos - planePos;
		const glm::vec3 normal = planeRot * cPlaneNormal;
		const float signedDistance = glm::dot(normal, planeToSphere);
		if (signedDistance > sphereCol.radius)
			return;

		Contact contact;
		contact.normal = -normal;
		contact.point = spherePos - normal * signedDistance;
		contact.penetration = sphereCol.radius - signedDistance;
		contacts.push_back(contact);
	}

	/**
	 * @brief Tests a box against an infinite plane, one corner at a time.
	 *
	 * The distance of each corner from the plane is the distance of the box center plus the corner offset along
	 * the normal, so the center distance is computed once. Every corner on or behind the plane gives its own
	 * contact, e.g. 4 for a box lying flat, so one pair can give up to 8 contacts.
	 *
	 * @param boxPos Center of the box.
	 * @param boxCol Box collider, with the half extents along the local axes.
	 * @param boxRot Rotation of the box body.
	 * @param planePos Any point on the plane.
	 * @param planeRot Rotation of the plane body, turning the local +Y axis into the plane normal.
	 * @param contacts Vector one contact per sunk corner is appended to, with the normal from the box to the plane.
	 */
	void boxPlaneTest(const glm::vec3 &boxPos, const Collider::Box &boxCol, const glm::quat &boxRot,
	                  const glm::vec3 &planePos, const glm::quat &planeRot, std::vector<Contact> &contacts)
	{
		const glm::vec3 &h = boxCol.halfExtents;
		const glm::vec3 corners[8] = {glm::vec3(-h.x, -h.y, -h.z), glm::vec3(-h.x, -h.y, h.z),
		                              glm::vec3(-h.x, h.y, -h.z),  glm::vec3(-h.x, h.y, h.z),
		                              glm::vec3(h.x, -h.y, -h.z),  glm::vec3(h.x, -h.y, h.z),
		                              glm::vec3(h.x, h.y, -h.z),   glm::vec3(h.x, h.y, h.z)};

		const glm::vec3 normal = planeRot * cPlaneNormal;
		const float centerDistance = glm::dot(normal, boxPos - planePos);

		for (const glm::vec3 &corner : corners) {
			const glm::vec3 offset = boxRot * corner;
			const float penetration = -(centerDistance + glm::dot(normal, offset));
			if (penetration < 0.0f)
				continue;

			Contact contact;
			contact.normal = -normal;
			contact.point = boxPos + offset;
			contact.penetration = penetration;
			contacts.push_back(contact);
		}
	}

	/**
	 * @brief Reverses the normals of the contacts added by a test called for a reversed pair of shapes.
	 *
	 * The tests take the shapes in a fixed order, so for the reversed pair their normals point from B to A.
	 *
	 * @param first Size of the vector before the test, the index of its first added contact.
	 * @param contacts Vector with the contacts, only those from first to the end are changed.
	 */
	void reverseNormals(std::size_t first, std::vector<Contact> &contacts)
	{
		for (std::size_t i = first; i < contacts.size(); i++)
			contacts.at(i).normal = -contacts.at(i).normal;
	}

} // unnamed namespace
