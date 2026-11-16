#ifndef NEWTON_UI_CAMERAPANEL_HPP
#define NEWTON_UI_CAMERAPANEL_HPP

namespace newton::controllers
{
	class CameraController;

} // namespace newton::controllers

namespace newton::ui
{
	float drawCameraPanel(controllers::CameraController &controller, float top);

} // namespace newton::ui

#endif // NEWTON_UI_CAMERAPANEL_HPP
