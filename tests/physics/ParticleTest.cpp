#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "newton/math/Vec3.hpp"
#include "newton/physics/Particle.hpp"

namespace newton::physics
{
	namespace
	{
		using math::Vec3;

		std::string toString(const Vec3 &vec);
		testing::AssertionResult vec3Near(const Vec3 &actual, const Vec3 &expected, float eps = 1e-5f);
		Particle undampedParticle();

		const Vec3 GRAVITY(0.0f, -10.0f, 0.0f); // Round value, so expected results are easy to compute

	} // unnamed namespace

	namespace
	{
		// Defaults and mass

		TEST(Particle, DefaultIsAtRestWithUnitMass)
		{
			const Particle p;

			EXPECT_TRUE(vec3Near(p.position(), Vec3()));
			EXPECT_TRUE(vec3Near(p.velocity(), Vec3()));
			EXPECT_TRUE(vec3Near(p.acceleration(), Vec3()));
			EXPECT_FLOAT_EQ(p.inverseMass(), 1.0f);
			EXPECT_FLOAT_EQ(p.mass(), 1.0f);
			EXPECT_TRUE(p.hasFiniteMass());
		}

		TEST(Particle, DefaultDampingIsInRange)
		{
			const Particle p;

			EXPECT_GT(p.damping(), 0.0f);
			EXPECT_LE(p.damping(), 1.0f);
		}

		TEST(Particle, SetMassStoresInverse)
		{
			Particle p;

			p.setMass(4.0f);

			EXPECT_FLOAT_EQ(p.inverseMass(), 0.25f);
			EXPECT_FLOAT_EQ(p.mass(), 4.0f);
		}

		TEST(Particle, ZeroInverseMassMakesImmovable)
		{
			Particle p;

			p.setInverseMass(0.0f);

			EXPECT_FALSE(p.hasFiniteMass());
			EXPECT_FLOAT_EQ(p.mass(), std::numeric_limits<float>::max());
		}

		TEST(Particle, SettersStoreValues)
		{
			Particle p;

			p.setPosition({1.0f, 2.0f, 3.0f});
			p.setVelocity({4.0f, 5.0f, 6.0f});
			p.setAcceleration(GRAVITY);
			p.setDamping(0.5f);

			EXPECT_TRUE(vec3Near(p.position(), {1.0f, 2.0f, 3.0f}));
			EXPECT_TRUE(vec3Near(p.velocity(), {4.0f, 5.0f, 6.0f}));
			EXPECT_TRUE(vec3Near(p.acceleration(), GRAVITY));
			EXPECT_FLOAT_EQ(p.damping(), 0.5f);
		}

		// Integration without forces

		TEST(Particle, WithoutForcesMovesUniformly)
		{
			Particle p = undampedParticle();
			p.setVelocity({1.0f, 2.0f, -3.0f});

			p.integrate(0.5f);
			p.integrate(0.5f);

			EXPECT_TRUE(vec3Near(p.position(), {1.0f, 2.0f, -3.0f}));
			EXPECT_TRUE(vec3Near(p.velocity(), {1.0f, 2.0f, -3.0f}));
		}

		TEST(Particle, PositionMovesWithNewVelocity)
		{
			// Semi-implicit Euler: v = 0 + (-10) * 1 = -10, then p = 0 + (-10) * 1 = -10.
			// Explicit Euler would move with the old velocity 0 and leave p at 0.
			Particle p = undampedParticle();
			p.setAcceleration(GRAVITY);

			p.integrate(1.0f);

			EXPECT_TRUE(vec3Near(p.velocity(), {0.0f, -10.0f, 0.0f}));
			EXPECT_TRUE(vec3Near(p.position(), {0.0f, -10.0f, 0.0f}));
		}

		TEST(Particle, VelocityUnderGravityMatchesFormula)
		{
			// v = a * t holds exactly for constant acceleration, whatever the step
			Particle p = undampedParticle();
			p.setAcceleration(GRAVITY);

			for (int i = 0; i < 10; ++i)
				p.integrate(0.1f);

			EXPECT_TRUE(vec3Near(p.velocity(), {0.0f, -10.0f, 0.0f}, 1e-4f));
		}

		TEST(Particle, PositionUnderGravityMatchesSemiImplicitEuler)
		{
			// After n steps: p = a * dt^2 * n(n + 1) / 2 = -10 * 0.01 * 55 = -5.5.
			// The exact value a * t^2 / 2 is -5, semi-implicit Euler overshoots by a * dt * t / 2.
			Particle p = undampedParticle();
			p.setAcceleration(GRAVITY);

			for (int i = 0; i < 10; ++i)
				p.integrate(0.1f);

			EXPECT_TRUE(vec3Near(p.position(), {0.0f, -5.5f, 0.0f}, 1e-4f));
		}

		TEST(Particle, SmallerStepIsCloserToExactPosition)
		{
			const float exact = 0.5f * GRAVITY.y * 1.0f * 1.0f; // -5 after one second

			Particle coarse = undampedParticle();
			Particle fine = undampedParticle();
			coarse.setAcceleration(GRAVITY);
			fine.setAcceleration(GRAVITY);

			for (int i = 0; i < 10; ++i)
				coarse.integrate(0.1f);
			for (int i = 0; i < 100; ++i)
				fine.integrate(0.01f);

			EXPECT_LT(std::abs(fine.position().y - exact), std::abs(coarse.position().y - exact));
			EXPECT_NEAR(fine.position().y, -5.05f, 1e-3f);
		}

		TEST(Particle, ConstantAccelerationIgnoresMass)
		{
			Particle light = undampedParticle();
			Particle heavy = undampedParticle();
			light.setMass(1.0f);
			heavy.setMass(10.0f);
			light.setAcceleration(GRAVITY);
			heavy.setAcceleration(GRAVITY);

			light.integrate(0.5f);
			heavy.integrate(0.5f);

			EXPECT_TRUE(vec3Near(light.velocity(), heavy.velocity()));
			EXPECT_TRUE(vec3Near(light.position(), heavy.position()));
		}

		// Infinite mass

		TEST(Particle, ImmovableIgnoresVelocityAndGravity)
		{
			Particle p = undampedParticle();
			p.setInverseMass(0.0f);
			p.setPosition({1.0f, 2.0f, 3.0f});
			p.setVelocity({5.0f, 0.0f, 0.0f});
			p.setAcceleration(GRAVITY);

			p.integrate(1.0f);

			EXPECT_TRUE(vec3Near(p.position(), {1.0f, 2.0f, 3.0f}));
			EXPECT_TRUE(vec3Near(p.velocity(), {5.0f, 0.0f, 0.0f}));
		}

		TEST(Particle, ForcesOnImmovableDoNotPileUp)
		{
			Particle p = undampedParticle();
			p.setInverseMass(0.0f);

			p.addForce({100.0f, 0.0f, 0.0f});
			p.integrate(1.0f);

			// Made movable again, the old force must not act
			p.setMass(1.0f);
			p.integrate(1.0f);

			EXPECT_TRUE(vec3Near(p.velocity(), Vec3()));
		}

		// Forces

		TEST(Particle, ForceIsDividedByMass)
		{
			// a = F / m = (4, 0, 0) / 2 = (2, 0, 0), v = a * 1, p = v * 1
			Particle p = undampedParticle();
			p.setMass(2.0f);

			p.addForce({4.0f, 0.0f, 0.0f});
			p.integrate(1.0f);

			EXPECT_TRUE(vec3Near(p.velocity(), {2.0f, 0.0f, 0.0f}));
			EXPECT_TRUE(vec3Near(p.position(), {2.0f, 0.0f, 0.0f}));
		}

		TEST(Particle, SameForceMovesHeavierLess)
		{
			Particle light = undampedParticle();
			Particle heavy = undampedParticle();
			light.setMass(1.0f);
			heavy.setMass(4.0f);

			light.addForce({8.0f, 0.0f, 0.0f});
			heavy.addForce({8.0f, 0.0f, 0.0f});
			light.integrate(1.0f);
			heavy.integrate(1.0f);

			EXPECT_TRUE(vec3Near(light.velocity(), {8.0f, 0.0f, 0.0f}));
			EXPECT_TRUE(vec3Near(heavy.velocity(), {2.0f, 0.0f, 0.0f}));
		}

		TEST(Particle, ForcesAddUp)
		{
			Particle p = undampedParticle();

			p.addForce({1.0f, 0.0f, 0.0f});
			p.addForce({0.0f, 2.0f, 0.0f});
			p.addForce({0.0f, 0.0f, -3.0f});
			p.integrate(1.0f);

			EXPECT_TRUE(vec3Near(p.velocity(), {1.0f, 2.0f, -3.0f}));
		}

		TEST(Particle, ForcesAndGravityCombine)
		{
			// Upward force equal to the weight: m * 10 = 2 * 10 = 20 N cancels gravity
			Particle p = undampedParticle();
			p.setMass(2.0f);
			p.setAcceleration(GRAVITY);

			p.addForce({0.0f, 20.0f, 0.0f});
			p.integrate(1.0f);

			EXPECT_TRUE(vec3Near(p.velocity(), Vec3()));
			EXPECT_TRUE(vec3Near(p.position(), Vec3()));
		}

		TEST(Particle, ForcesAreClearedAfterStep)
		{
			Particle p = undampedParticle();

			p.addForce({2.0f, 0.0f, 0.0f});
			p.integrate(1.0f);
			p.integrate(1.0f); // No force any more, velocity stays

			EXPECT_TRUE(vec3Near(p.velocity(), {2.0f, 0.0f, 0.0f}));
		}

		TEST(Particle, ClearAccumulatorRemovesForces)
		{
			Particle p = undampedParticle();

			p.addForce({2.0f, 0.0f, 0.0f});
			p.clearAccumulator();
			p.integrate(1.0f);

			EXPECT_TRUE(vec3Near(p.velocity(), Vec3()));
		}

		// Damping

		TEST(Particle, DampingOfOneKeepsVelocity)
		{
			Particle p = undampedParticle();
			p.setVelocity({3.0f, 0.0f, 0.0f});

			p.integrate(1.0f);

			EXPECT_TRUE(vec3Near(p.velocity(), {3.0f, 0.0f, 0.0f}));
		}

		TEST(Particle, DampingIsFractionKeptPerSecond)
		{
			Particle p;
			p.setDamping(0.5f);
			p.setVelocity({8.0f, 0.0f, 0.0f});

			p.integrate(1.0f);

			EXPECT_TRUE(vec3Near(p.velocity(), {4.0f, 0.0f, 0.0f}));
		}

		TEST(Particle, DampingDoesNotDependOnStepLength)
		{
			Particle oneStep;
			Particle tenSteps;
			oneStep.setDamping(0.5f);
			tenSteps.setDamping(0.5f);
			oneStep.setVelocity({8.0f, 0.0f, 0.0f});
			tenSteps.setVelocity({8.0f, 0.0f, 0.0f});

			oneStep.integrate(1.0f);
			for (int i = 0; i < 10; ++i)
				tenSteps.integrate(0.1f);

			EXPECT_TRUE(vec3Near(oneStep.velocity(), tenSteps.velocity(), 1e-4f));
		}

		TEST(Particle, DampingOfZeroStops)
		{
			Particle p;
			p.setDamping(0.0f);
			p.setVelocity({8.0f, 0.0f, 0.0f});

			p.integrate(0.1f);

			EXPECT_TRUE(vec3Near(p.velocity(), Vec3()));
		}

		// Invalid values: assert in debug builds, ignored in release builds.
		// The DeathTest suffix makes GoogleTest run these first, as it recommends.

		TEST(ParticleDeathTest, NonPositiveMassIsRejected)
		{
			Particle p;

			EXPECT_DEBUG_DEATH(p.setMass(0.0f), "mass must be greater than zero");
			EXPECT_DEBUG_DEATH(p.setMass(-1.0f), "mass must be greater than zero");
			EXPECT_DEBUG_DEATH(p.setMass(std::nanf("")), "mass must be greater than zero");
			EXPECT_FLOAT_EQ(p.inverseMass(), 1.0f);
		}

		TEST(ParticleDeathTest, NegativeInverseMassIsRejected)
		{
			Particle p;

			EXPECT_DEBUG_DEATH(p.setInverseMass(-1.0f), "inverse mass must not be negative");
			EXPECT_FLOAT_EQ(p.inverseMass(), 1.0f);
		}

		TEST(ParticleDeathTest, DampingOutOfRangeIsRejected)
		{
			Particle p;
			const float before = p.damping();

			EXPECT_DEBUG_DEATH(p.setDamping(-0.1f), "damping must be in range");
			EXPECT_DEBUG_DEATH(p.setDamping(1.1f), "damping must be in range");
			EXPECT_FLOAT_EQ(p.damping(), before);
		}

		TEST(ParticleDeathTest, NonPositiveStepIsRejected)
		{
			Particle p;

			EXPECT_DEBUG_DEATH(p.integrate(0.0f), "dt > 0");
		}

	} // unnamed namespace

	namespace
	{
		std::string toString(const Vec3 &vec)
		{
			std::ostringstream oss;
			oss << std::setprecision(9);
			oss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
			return oss.str();
		}

		testing::AssertionResult vec3Near(const Vec3 &actual, const Vec3 &expected, float eps)
		{
			if (std::abs(actual.x - expected.x) <= eps && std::abs(actual.y - expected.y) <= eps &&
			    std::abs(actual.z - expected.z) <= eps) {
				return testing::AssertionSuccess();
			}
			return testing::AssertionFailure()
			       << "Expected " << toString(expected) << "\nGot " << toString(actual) << "\n(eps " << eps << ")";
		}

		/**
		 * Damping of 1 keeps the expected results exact, the default value would change every velocity.
		 */
		Particle undampedParticle()
		{
			Particle p;
			p.setDamping(1.0f);
			return p;
		}

	} // unnamed namespace

} // namespace newton::physics
