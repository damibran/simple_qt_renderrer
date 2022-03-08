#pragma once
#include <QSlider>

class QAxisSlider final : public QSlider
{
public:
	explicit QAxisSlider(QWidget* parent = nullptr) : QSlider(parent)	{
	}

	void bindSliderAndPointAxis(float* v,bool* nu)
	{
		axis_value_ = v;
		*v = static_cast<float>(value());
		bezier_need_update_ = nu;
	}

protected:
	void sliderChange(SliderChange change) override
	{
		QSlider::sliderChange(change);
		if (change == SliderValueChange && axis_value_!=nullptr)
		{
			*axis_value_ = static_cast<float>(value());
			*bezier_need_update_ = true;
		}
	}
private:
	float* axis_value_=nullptr;
	bool* bezier_need_update_ = nullptr;
};
