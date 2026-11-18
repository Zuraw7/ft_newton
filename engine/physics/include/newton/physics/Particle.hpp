#ifndef NEWTON_PHYSICS_PARTICLE_HPP
#define NEWTON_PHYSICS_PARTICLE_HPP

#include "newton/math/Vec3.hpp"

namespace newton::physics
{
	/**
	 * @brief Point mass without rotation, the simplest simulated object.
	 *
	 * Moved by @ref integrate() once per physics step. Two sources of
	 * acceleration are combined there:
	 * - constant acceleration (@ref setAcceleration()), usually gravity, the
	 *   same for any mass and never cleared,
	 * - forces (@ref addForce()), divided by the mass and cleared after every
	 *   step.
	 *
	 * Mass is stored as its inverse, so an immovable particle has inverse mass
	 * 0 instead of an infinite mass.
	 */
	class Particle
	{
		public:
			Particle() = default;
			~Particle() = default;

			const math::Vec3 &position() const;
			const math::Vec3 &velocity() const;
			const math::Vec3 &acceleration() const;
			float damping() const;
			float mass() const;
			float inverseMass() const;

			void setPosition(const math::Vec3 &position);
			void setVelocity(const math::Vec3 &velocity);
			void setAcceleration(const math::Vec3 &acceleration);
			void setDamping(float damping);
			void setMass(float mass);
			void setInverseMass(float inverseMass);

			void addForce(const math::Vec3 &force);
			void clearAccumulator();

			bool hasFiniteMass() const;

			void integrate(float dt);

		private:
			math::Vec3 m_position;     ///< Position in world space, in meters.
			math::Vec3 m_velocity;     ///< Velocity in world space, in m/s.
			math::Vec3 m_acceleration; ///< Constant acceleration in m/s^2, usually gravity.
			math::Vec3 m_forceAccum;   ///< Sum of forces added since the last step, in newtons.

			float m_damping = 0.995f;   ///< Fraction of velocity kept per second, in [0, 1]. 1 means no damping.
			float m_inverseMass = 1.0f; ///< 1 / mass, in 1/kg. 0 means infinite mass (immovable).
	};

} // namespace newton::physics

#endif // NEWTON_PHYSICS_PARTICLE_HPP
