#ifndef UI_STATSOVERLAY_HPP
#define UI_STATSOVERLAY_HPP

#include <cstddef>

namespace newton::ui
{
	void drawStatsOverlay(int fps, std::size_t objectCount, std::size_t physicsObjects);

} // namespace newton::ui

#endif // UI_STATSOVERLAY_HPP
