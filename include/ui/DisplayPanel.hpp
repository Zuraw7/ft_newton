#ifndef UI_DISPLAYPANEL_HPP
#define UI_DISPLAYPANEL_HPP

namespace newton::graphics
{
	class Context;
	class GridRenderer;

} // namespace newton::graphics

namespace newton::ui
{
	void drawDisplayPanel(graphics::Context &context, graphics::GridRenderer &gridRenderer);

} // namespace newton::ui

#endif // UI_DISPLAYPANEL_HPP
