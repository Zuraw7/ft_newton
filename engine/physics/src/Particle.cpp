#include "newton/physics/Particle.hpp"

#include <cassert>
#include <cmath>
#include <limits>

#include "newton/math/Vec3.hpp"

namespace newton::physics
{
	const math::Vec3 &Particle::position() const
	{
		return m_position;
	}

	const math::Vec3 &Particle::velocity() const
	{
		return m_velocity;
	}

	const math::Vec3 &Particle::acceleration() const
	{
		return m_acceleration;
	}

	float Particle::damping() const
	{
		return m_damping;
	}

	/**
	 * @return Mass in kilograms, or the largest float for an immovable particle. Physics code should use
	 * @ref inverseMass() instead.
	 */
	float Particle::mass() const
	{
		if (!hasFiniteMass())
			return std::numeric_limits<float>::max();
		return 1.0f / m_inverseMass;
	}

	/**
	 * @return 1 / mass, in 1/kg. 0 for an immovable particle.
	 */
	float Particle::inverseMass() const
	{
		return m_inverseMass;
	}

	void Particle::setPosition(const math::Vec3 &position)
	{
		m_position = position;
	}

	void Particle::setVelocity(const math::Vec3 &velocity)
	{
		m_velocity = velocity;
	}

	/**
	 * @brief Sets the constant acceleration, usually gravity.
	 *
	 * It is applied in every step regardless of the mass and is not cleared. Changing forces go through
	 * @ref addForce().
	 *
	 * @param acceleration Acceleration in m/s^2.
	 */
	void Particle::setAcceleration(const math::Vec3 &acceleration)
	{
		m_acceleration = acceleration;
	}

	/**
	 * @brief Sets how much velocity is kept after one second without forces.
	 *
	 * Works as a simple drag, e.g. 0.99 keeps 99% of the velocity per second. Not needed for stability.
	 *
	 * @param damping Fraction in [0, 1], 1 means no damping.
	 */
	void Particle::setDamping(float damping)
	{
		const bool valid = damping <= 1.0f && damping >= 0.0f;
		assert(valid && "damping must be in range [0, 1]");
		if (!valid)
			return;
		m_damping = damping;
	}

	/**
	 * @brief Sets the mass, stored as its inverse.
	 *
	 * Zero mass would make any force give infinite acceleration, so it is not allowed. An immovable particle
	 * is made with @ref setInverseMass() and 0.
	 *
	 * @param mass Mass in kilograms, must be greater than zero.
	 */
	void Particle::setMass(float mass)
	{
		const bool valid = mass > 0.0f;
		assert(valid && "mass must be greater than zero");
		if (!valid)
			return;
		m_inverseMass = 1.0f / mass;
	}

	/**
	 * @brief Sets 1 / mass directly, the only way to make the particle immovable.
	 *
	 * @param inverseMass Inverse mass in 1/kg, must not be negative. 0 means infinite mass.
	 */
	void Particle::setInverseMass(float inverseMass)
	{
		const bool valid = inverseMass >= 0.0f;
		assert(valid && "inverse mass must not be negative");
		if (!valid)
			return;
		m_inverseMass = inverseMass;
	}

	/**
	 * @brief Adds a force acting during the next step only.
	 *
	 * Forces from many sources add up, @ref integrate() turns their sum into acceleration and clears it.
	 *
	 * @param force Force in newtons.
	 */
	void Particle::addForce(const math::Vec3 &force)
	{
		m_forceAccum += force;
	}

	/**
	 * @brief Removes all forces added since the last step.
	 *
	 * @ref integrate() calls it itself.
	 */
	void Particle::clearAccumulator()
	{
		m_forceAccum = {};
	}

	/**
	 * @return False for an immovable particle (inverse mass 0).
	 */
	bool Particle::hasFiniteMass() const
	{
		return m_inverseMass > 0.0f;
	}

	/**
	 * @brief Moves the particle forward in time by @p dt seconds.
	 *
	 * Uses semi-implicit Euler: velocity is updated first and the position
	 * moves with the new velocity, which keeps energy stable. Damping is
	 * applied as a power of @p dt, so the fraction kept per second does not
	 * depend on the step length. Forces added since the last step are applied
	 * and cleared. A particle with infinite mass does not move.
	 *
	 * @param dt Time step in seconds, must be greater than zero.
	 */
	void Particle::integrate(float dt)
	{
		assert(dt > 0.0f && "dt must be greater than zero");

		// Immovable: ignore gravity too, and drop forces so they do not pile up
		if (!hasFiniteMass()) {
			clearAccumulator();
			return;
		}

		// Constant acceleration (gravity) does not depend on mass, forces do
		const math::Vec3 acceleration = m_acceleration + m_forceAccum * m_inverseMass;

		m_velocity += acceleration * dt;
		m_velocity *= std::pow(m_damping, dt);

		m_position += m_velocity * dt;

		clearAccumulator();
	}

} // namespace newton::physics
