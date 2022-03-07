#pragma once
#include "../Script.h"
#include "ui_RenderrerMainWindow.h"
#include "../Renderers/ConcreteRenderers/BezierCurveRenderer.h"


class BezierScript final : public Script
{
public:
	BezierScript(Ui::RenderrerMainWindowClass& ui, BezierCurveRenderer* bezier_renderer, Shape* bezierShape): shape_(
		bezierShape)
	{
		x_rot_line_edit_ = ui.XRotationValue;
		y_rot_line_edit_ = ui.YRotationValue;

		std::array<glm::vec3, 5>* cp = bezier_renderer->getControlPointsPtr();
		bool* nu = bezier_renderer->getNeedUpdatePtr();

		ui.P1XSlider->bindSliderAndPointAxis(&(*cp)[0].x, nu);
		ui.P1YSlider->bindSliderAndPointAxis(&(*cp)[0].y, nu);
		ui.P1ZSlider->bindSliderAndPointAxis(&(*cp)[0].z, nu);

		ui.P2XSlider->bindSliderAndPointAxis(&(*cp)[1].x, nu);
		ui.P2YSlider->bindSliderAndPointAxis(&(*cp)[1].y, nu);
		ui.P2ZSlider->bindSliderAndPointAxis(&(*cp)[1].z, nu);

		ui.P3XSlider->bindSliderAndPointAxis(&(*cp)[2].x, nu);
		ui.P3YSlider->bindSliderAndPointAxis(&(*cp)[2].y, nu);
		ui.P3ZSlider->bindSliderAndPointAxis(&(*cp)[2].z, nu);

		ui.P4XSlider->bindSliderAndPointAxis(&(*cp)[3].x, nu);
		ui.P4YSlider->bindSliderAndPointAxis(&(*cp)[3].y, nu);
		ui.P4ZSlider->bindSliderAndPointAxis(&(*cp)[3].z, nu);

		ui.P5XSlider->bindSliderAndPointAxis(&(*cp)[4].x, nu);
		ui.P5YSlider->bindSliderAndPointAxis(&(*cp)[4].y, nu);
		ui.P5ZSlider->bindSliderAndPointAxis(&(*cp)[4].z, nu);
	}

	void updateScript(float dt) override
	{
		shape_->setRotationDegrees({x_rot_line_edit_->text().toFloat(), y_rot_line_edit_->text().toFloat(), 0});
	}

private:
	QLineEdit* x_rot_line_edit_;
	QLineEdit* y_rot_line_edit_;
	Shape* shape_;
};
