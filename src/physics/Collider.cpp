#include "physics/Collider.hpp"

#include <variant>

namespace newton::physics
{
	/**
	 * @brief Fits a collider made for scale 1 to the scale of an object.
	 *
	 * Always pass the original collider, not one already scaled, otherwise the scale is applied twice.
	 * A sphere is scaled by `scale.x` only, so the scale should be uniform. A box is scaled along each local axis.
	 * A plane is infinite and does not change. The rotation is not applied here, it stays in the body.
	 *
	 * @param collider Collider of the object at scale 1, e.g. radius 0.5 for a unit sphere mesh.
	 * @param scale Scale of the object along each axis.
	 * @return The scaled copy.
	 */
	Collider scaleCollider(Collider collider, const glm::vec3 &scale)
	{
		struct ShapeScaler
		{
				const glm::vec3 &scl;

				void operator()(Collider::Sphere &sphere) const
				{
					sphere.radius *= scl.x;
				}
				void operator()(Collider::Box &box) const
				{
					box.halfExtents *= scl;
				}
				void operator()(Collider::Plane &) const {}
		};

		ShapeScaler scaler{scale};
		std::visit(scaler, collider.shape);

		return collider;
	}

} // namespace newton::physics
