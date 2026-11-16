#ifndef NEWTON_UI_DISPLAYPANEL_HPP
#define NEWTON_UI_DISPLAYPANEL_HPP

namespace newton::graphics
{
	class Context;
	class GridRenderer;

} // namespace newton::graphics

namespace newton::ui
{
	float drawDisplayPanel(graphics::Context &context, graphics::GridRenderer &gridRenderer, float top);

} // namespace newton::ui

#endif // NEWTON_UI_DISPLAYPANEL_HPP
