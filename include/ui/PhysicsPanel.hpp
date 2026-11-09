#ifndef UI_PHYSICSPANEL_HPP
#define UI_PHYSICSPANEL_HPP

namespace newton::physics
{
	class PhysicsWorld;

} // namespace newton::physics

namespace newton::ui
{
	float drawPhysicsPanel(physics::PhysicsWorld &world, float top);

} // namespace newton::ui

#endif // UI_PHYSICSPANEL_HPP
