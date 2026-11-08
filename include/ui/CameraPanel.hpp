#ifndef UI_CAMERAPANEL_HPP
#define UI_CAMERAPANEL_HPP

namespace newton::controllers
{
	class CameraController;

} // namespace newton::controllers

namespace newton::ui
{
	float drawCameraPanel(controllers::CameraController &controller, float top);

} // namespace newton::ui

#endif // UI_CAMERAPANEL_HPP
