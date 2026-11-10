#include "physics/Collider.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <variant>

namespace newton::physics
{
	/**
	 * @brief Fits a collider made for scale 1 to the scale and rotation of an object.
	 *
	 * Always pass the original collider, not one already transformed, otherwise the scale is applied twice.
	 * A sphere is scaled by `scale.x` only, so the scale should be uniform. A box is scaled along each axis and
	 * ignores the rotation. A plane ignores the scale and only its normal is rotated.
	 *
	 * @param collider Collider of the object at scale 1, e.g. radius 0.5 for a unit sphere mesh.
	 * @param rotation Rotation of the object.
	 * @param scale Scale of the object along each axis.
	 * @return The transformed copy.
	 */
	Collider transformCollider(Collider collider, const glm::quat &rotation, const glm::vec3 &scale)
	{
		struct ShapeTransformer
		{
				const glm::quat &rot;
				const glm::vec3 &scl;

				void operator()(Collider::Sphere &sphere) const
				{
					sphere.radius *= scl.x;
				}
				void operator()(Collider::Box &box) const
				{
					box.halfExtents *= scl;
				}
				void operator()(Collider::Plane &plane) const
				{
					plane.normal = glm::normalize(rot * plane.normal);
				}
		};

		ShapeTransformer transformer{rotation, scale};
		std::visit(transformer, collider.shape);

		return collider;
	}

} // namespace newton::physics
