include(GoogleTest)

add_executable(newton_tests
    math/Vec3Test.cpp
    physics/ParticleTest.cpp
)

set_project_warnings(newton_tests)

target_link_libraries(newton_tests PRIVATE
    newton_math
    newton_physics
    GTest::gtest_main
)

# Registers every TEST() as a separate ctest test
gtest_discover_tests(newton_tests)
