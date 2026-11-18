#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "newton/math/Vec3.hpp"

namespace newton::math
{
	namespace
	{
		std::string toString(const Vec3 &vec);
		testing::AssertionResult vec3Eq(const Vec3 &actual, const Vec3 &expected);
		testing::AssertionResult vec3Near(const Vec3 &actual, const Vec3 &expected, float eps = 1e-5f);

		const Vec3 X(1.0f, 0.0f, 0.0f);
		const Vec3 Y(0.0f, 1.0f, 0.0f);
		const Vec3 Z(0.0f, 0.0f, 1.0f);

	} // unnamed namespace

	namespace
	{
		// Constructors

		TEST(Vec3, DefaultConstructorGivesZero)
		{
			Vec3 vec;
			EXPECT_FLOAT_EQ(vec.x, 0.0f);
			EXPECT_FLOAT_EQ(vec.y, 0.0f);
			EXPECT_FLOAT_EQ(vec.z, 0.0f);
		}

		TEST(Vec3, ConstructorSetsComponentsInOrder)
		{
			Vec3 vec(1.0f, 2.0f, 3.0f);
			EXPECT_FLOAT_EQ(vec.x, 1.0f);
			EXPECT_FLOAT_EQ(vec.y, 2.0f);
			EXPECT_FLOAT_EQ(vec.z, 3.0f);
		}

		// Addition and subtraction

		TEST(Vec3, AdditionAddsComponentWise)
		{
			EXPECT_TRUE(vec3Eq(Vec3(3.0f, 4.0f, 5.0f) + Vec3(2.0f, 3.0f, 4.0f), {5.0f, 7.0f, 9.0f}));
			EXPECT_TRUE(vec3Eq(Vec3(1.0f, -2.0f, 3.0f) + Vec3(-4.0f, 5.0f, -6.0f), {-3.0f, 3.0f, -3.0f}));
			EXPECT_TRUE(vec3Eq(Vec3(0.5f, 0.25f, -1.5f) + Vec3(0.25f, 0.5f, 1.5f), {0.75f, 0.75f, 0.0f}));
		}

		TEST(Vec3, AdditionIsCommutative)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);
			const Vec3 b(4.0f, 5.0f, 6.0f);

			EXPECT_TRUE(vec3Eq(a + b, b + a));
		}

		TEST(Vec3, SubtractionSubtractsComponentWise)
		{
			const Vec3 a(5.0f, 7.0f, 9.0f);
			const Vec3 b(1.0f, 2.0f, 3.0f);

			EXPECT_TRUE(vec3Eq(a - b, {4.0f, 5.0f, 6.0f}));
			EXPECT_TRUE(vec3Eq(b - a, {-4.0f, -5.0f, -6.0f}));
		}

		TEST(Vec3, AddAssignModifiesLeftOperandOnly)
		{
			Vec3 a(3.0f, 4.0f, 5.0f);
			Vec3 b(2.0f, 3.0f, 4.0f);

			a += b;

			EXPECT_TRUE(vec3Eq(a, {5.0f, 7.0f, 9.0f}));
			EXPECT_TRUE(vec3Eq(b, {2.0f, 3.0f, 4.0f}));
		}

		TEST(Vec3, SubtractAssignModifiesLeftOperandOnly)
		{
			Vec3 a(5.0f, 7.0f, 9.0f);
			Vec3 b(1.0f, 2.0f, 3.0f);

			a -= b;

			EXPECT_TRUE(vec3Eq(a, {4.0f, 5.0f, 6.0f}));
			EXPECT_TRUE(vec3Eq(b, {1.0f, 2.0f, 3.0f}));
		}

		TEST(Vec3, AdditionDoesNotModifyOperands)
		{
			Vec3 a(1.0f, 2.0f, 3.0f);
			Vec3 b(4.0f, 5.0f, 6.0f);

			static_cast<void>(a + b);
			static_cast<void>(a - b);

			EXPECT_TRUE(vec3Eq(a, {1.0f, 2.0f, 3.0f}));
			EXPECT_TRUE(vec3Eq(b, {4.0f, 5.0f, 6.0f}));
		}

		TEST(Vec3, AddingZeroGivesSameVector)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);

			EXPECT_TRUE(vec3Eq(a + Vec3(), a));
			EXPECT_TRUE(vec3Eq(a - Vec3(), a));
		}

		TEST(Vec3, SubtractingItselfGivesZero)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);

			EXPECT_TRUE(vec3Eq(a - a, Vec3()));
		}

		TEST(Vec3, SubtractionUndoesAddition)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);
			const Vec3 b(4.0f, 5.0f, 6.0f);

			EXPECT_TRUE(vec3Eq((a + b) - b, a));
		}

		// Scalar multiplication and division

		TEST(Vec3, MultiplicationScalesComponents)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);

			EXPECT_TRUE(vec3Eq(a * 2.0f, {2.0f, 4.0f, 6.0f}));
			EXPECT_TRUE(vec3Eq(a * 0.0f, Vec3()));
			EXPECT_TRUE(vec3Eq(a * -1.0f, -a));
		}

		TEST(Vec3, ScalarOnTheLeftGivesSameResult)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);

			EXPECT_TRUE(vec3Eq(2.0f * a, a * 2.0f));
		}

		TEST(Vec3, MultiplyAssignScalesInPlace)
		{
			Vec3 a(1.0f, 2.0f, 3.0f);

			a *= 2.0f;

			EXPECT_TRUE(vec3Eq(a, {2.0f, 4.0f, 6.0f}));
		}

		TEST(Vec3, DivisionDividesComponents)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);

			EXPECT_TRUE(vec3Eq(a / 2.0f, {0.5f, 1.0f, 1.5f}));
		}

		TEST(Vec3, DivideAssignDividesInPlace)
		{
			Vec3 a(1.0f, 2.0f, 3.0f);

			a /= 2.0f;

			EXPECT_TRUE(vec3Eq(a, {0.5f, 1.0f, 1.5f}));
		}

		TEST(Vec3, DivisionUndoesMultiplication)
		{
			const Vec3 a(0.1f, -0.2f, 0.3f);

			EXPECT_TRUE(vec3Near((a * 3.0f) / 3.0f, a));
		}

		// Negation and invert

		TEST(Vec3, NegationFlipsComponentsWithoutModifying)
		{
			const Vec3 a(1.0f, -2.0f, 3.0f);

			EXPECT_TRUE(vec3Eq(-a, {-1.0f, 2.0f, -3.0f}));
			EXPECT_TRUE(vec3Eq(a, {1.0f, -2.0f, 3.0f}));
		}

		TEST(Vec3, InvertFlipsComponentsInPlace)
		{
			Vec3 a(1.0f, -2.0f, 3.0f);

			a.invert();

			EXPECT_TRUE(vec3Eq(a, {-1.0f, 2.0f, -3.0f}));
		}

		TEST(Vec3, DoubleInvertGivesOriginal)
		{
			Vec3 a(1.0f, -2.0f, 3.0f);

			a.invert();
			a.invert();

			EXPECT_TRUE(vec3Eq(a, {1.0f, -2.0f, 3.0f}));
		}

		// Compound operators return a reference

		TEST(Vec3, CompoundOperatorsReturnSameObject)
		{
			Vec3 a(1.0f, 2.0f, 3.0f);
			const Vec3 b(4.0f, 5.0f, 6.0f);

			EXPECT_EQ(&(a += b), &a);
			EXPECT_EQ(&(a -= b), &a);
			EXPECT_EQ(&(a *= 2.0f), &a);
			EXPECT_EQ(&(a /= 2.0f), &a);
		}

		TEST(Vec3, CompoundOperatorsCanBeChained)
		{
			Vec3 a(1.0f, 2.0f, 3.0f);
			const Vec3 b(4.0f, 5.0f, 6.0f);

			(a += b) += b;

			EXPECT_TRUE(vec3Eq(a, {9.0f, 12.0f, 15.0f}));
		}

		// Magnitude

		TEST(Vec3, MagnitudeIsLength)
		{
			EXPECT_FLOAT_EQ(Vec3(3.0f, 4.0f, 0.0f).magnitude(), 5.0f);
			EXPECT_FLOAT_EQ(Vec3(2.0f, 3.0f, 6.0f).magnitude(), 7.0f);
		}

		TEST(Vec3, MagnitudeIgnoresSign)
		{
			EXPECT_FLOAT_EQ(Vec3(-3.0f, -4.0f, 0.0f).magnitude(), 5.0f);
		}

		TEST(Vec3, MagnitudeOfZeroIsZero)
		{
			EXPECT_FLOAT_EQ(Vec3().magnitude(), 0.0f);
		}

		TEST(Vec3, SquareMagnitudeIsSquaredLength)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);

			EXPECT_FLOAT_EQ(a.squareMagnitude(), 14.0f);
			EXPECT_NEAR(a.squareMagnitude(), a.magnitude() * a.magnitude(), 1e-5f);
		}

		TEST(Vec3, MagnitudeOfDifferenceIsDistance)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);
			const Vec3 b(4.0f, 5.0f, 6.0f);

			EXPECT_NEAR((b - a).magnitude(), std::sqrt(27.0f), 1e-5f);
			EXPECT_NEAR((a - b).magnitude(), std::sqrt(27.0f), 1e-5f);
		}

		// Normalization

		TEST(Vec3, NormalizedKeepsDirection)
		{
			EXPECT_TRUE(vec3Near(Vec3(3.0f, 4.0f, 0.0f).normalized(), {0.6f, 0.8f, 0.0f}));
			EXPECT_TRUE(vec3Near(Vec3(0.0f, 0.0f, -5.0f).normalized(), {0.0f, 0.0f, -1.0f}));
		}

		TEST(Vec3, NormalizedHasUnitLength)
		{
			const Vec3 vectors[] = {
				{1.0f, 2.0f, 3.0f},
				{-7.0f, 0.5f, 2.0f},
				{0.001f, 0.0f, 0.0f},
				{100.0f, -200.0f, 300.0f},
			};

			for (const Vec3 &vec : vectors)
				EXPECT_NEAR(vec.normalized().magnitude(), 1.0f, 1e-5f) << "for " << toString(vec);
		}

		TEST(Vec3, NormalizedDoesNotModifyOriginal)
		{
			const Vec3 a(3.0f, 4.0f, 0.0f);

			static_cast<void>(a.normalized());

			EXPECT_TRUE(vec3Eq(a, {3.0f, 4.0f, 0.0f}));
		}

		TEST(Vec3, NormalizeMatchesNormalized)
		{
			Vec3 a(1.0f, 2.0f, 3.0f);
			const Vec3 expected = a.normalized();

			a.normalize();

			EXPECT_TRUE(vec3Eq(a, expected));
		}

		TEST(Vec3, NormalizingZeroGivesZero)
		{
			EXPECT_TRUE(vec3Eq(Vec3().normalized(), Vec3()));
		}

		TEST(Vec3, NormalizingUnitVectorKeepsIt)
		{
			EXPECT_TRUE(vec3Near(X.normalized(), X));
		}

		// Dot product

		TEST(Vec3, DotSumsComponentProducts)
		{
			EXPECT_FLOAT_EQ(dot(Vec3(1.0f, 2.0f, 3.0f), Vec3(4.0f, 5.0f, 6.0f)), 32.0f);
		}

		TEST(Vec3, DotIsCommutative)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);
			const Vec3 b(4.0f, 5.0f, 6.0f);

			EXPECT_FLOAT_EQ(dot(a, b), dot(b, a));
		}

		TEST(Vec3, DotOfPerpendicularIsZero)
		{
			EXPECT_FLOAT_EQ(dot(X, Y), 0.0f);
			EXPECT_FLOAT_EQ(dot(X, Z), 0.0f);
			EXPECT_FLOAT_EQ(dot(Y, Z), 0.0f);
		}

		TEST(Vec3, DotOfUnitVectorsIsCosine)
		{
			EXPECT_FLOAT_EQ(dot(X, X), 1.0f);
			EXPECT_FLOAT_EQ(dot(X, -X), -1.0f);
		}

		TEST(Vec3, DotSignShowsAngle)
		{
			EXPECT_GT(dot(Vec3(1.0f, 1.0f, 0.0f), X), 0.0f);
			EXPECT_LT(dot(Vec3(-1.0f, 1.0f, 0.0f), X), 0.0f);
		}

		TEST(Vec3, DotWithItselfIsSquareMagnitude)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);

			EXPECT_FLOAT_EQ(dot(a, a), a.squareMagnitude());
		}

		TEST(Vec3, DotWithUnitVectorIsProjection)
		{
			EXPECT_FLOAT_EQ(dot(Vec3(3.0f, 4.0f, 0.0f), X), 3.0f);
		}

		TEST(Vec3, DotWithZeroIsZero)
		{
			EXPECT_FLOAT_EQ(dot(Vec3(1.0f, 2.0f, 3.0f), Vec3()), 0.0f);
		}

		// Cross product

		TEST(Vec3, CrossOfAxesIsRightHanded)
		{
			EXPECT_TRUE(vec3Eq(cross(X, Y), Z));
			EXPECT_TRUE(vec3Eq(cross(Y, Z), X));
			EXPECT_TRUE(vec3Eq(cross(Z, X), Y));
		}

		TEST(Vec3, CrossOfSwappedAxesIsNegated)
		{
			EXPECT_TRUE(vec3Eq(cross(Y, X), -Z));
		}

		TEST(Vec3, CrossGivesExpectedValue)
		{
			EXPECT_TRUE(vec3Eq(cross(Vec3(1.0f, 2.0f, 3.0f), Vec3(4.0f, 5.0f, 6.0f)), {-3.0f, 6.0f, -3.0f}));
		}

		TEST(Vec3, CrossIsAnticommutative)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);
			const Vec3 b(4.0f, 5.0f, 6.0f);

			EXPECT_TRUE(vec3Eq(cross(a, b), -cross(b, a)));
		}

		TEST(Vec3, CrossOfParallelIsZero)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);

			EXPECT_TRUE(vec3Eq(cross(a, a), Vec3()));
			EXPECT_TRUE(vec3Eq(cross(a, a * 2.0f), Vec3()));
		}

		TEST(Vec3, CrossIsPerpendicularToBoth)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);
			const Vec3 b(4.0f, 5.0f, 6.0f);
			const Vec3 c = cross(a, b);

			EXPECT_NEAR(dot(c, a), 0.0f, 1e-5f);
			EXPECT_NEAR(dot(c, b), 0.0f, 1e-5f);
		}

		TEST(Vec3, CrossIsNotNormalized)
		{
			EXPECT_FLOAT_EQ(cross(X, Y * 2.0f).magnitude(), 2.0f);
		}

		TEST(Vec3, CrossAndDotSatisfyLagrangeIdentity)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);
			const Vec3 b(4.0f, 5.0f, 6.0f);
			const float d = dot(a, b);

			EXPECT_FLOAT_EQ(cross(a, b).squareMagnitude() + d * d, a.squareMagnitude() * b.squareMagnitude());
		}

		// Component-wise product

		TEST(Vec3, CwiseProductMultipliesComponents)
		{
			EXPECT_TRUE(vec3Eq(cwiseProduct(Vec3(1.0f, 2.0f, 3.0f), Vec3(4.0f, 5.0f, 6.0f)), {4.0f, 10.0f, 18.0f}));
		}

		TEST(Vec3, CwiseProductIsCommutative)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);
			const Vec3 b(4.0f, 5.0f, 6.0f);

			EXPECT_TRUE(vec3Eq(cwiseProduct(a, b), cwiseProduct(b, a)));
		}

		TEST(Vec3, CwiseProductWithOnesGivesSameVector)
		{
			const Vec3 a(1.0f, 2.0f, 3.0f);

			EXPECT_TRUE(vec3Eq(cwiseProduct(a, {1.0f, 1.0f, 1.0f}), a));
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

		testing::AssertionResult vec3Eq(const Vec3 &actual, const Vec3 &expected)
		{
			if (actual.x == expected.x && actual.y == expected.y && actual.z == expected.z) {
				return testing::AssertionSuccess();
			}
			return testing::AssertionFailure() << "Expected " << toString(expected) << "\nGot " << toString(actual);
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

	} // unnamed namespace

} // namespace newton::math
