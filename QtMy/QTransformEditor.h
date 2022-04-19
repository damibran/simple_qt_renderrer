#pragma once
#include <QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include "../utils/Transform.h"

class QTransformEditor : public QWidget
{
public slots:
	void onSelectorUpdate(int i)
	{
		glm::vec3 values;
		if (i == 0) // Scale
		{
			values = shape_transform_->getScale();
			XSlider->setMinimum(scaleMin);
			YSlider->setMinimum(scaleMin);
			ZSlider->setMinimum(scaleMin);

			XSlider->setMaximum(scaleMax);
			YSlider->setMaximum(scaleMax);
			ZSlider->setMaximum(scaleMax);
		}
		else if (i == 1) // Pos
		{
			values = shape_transform_->getPos();
			XSlider->setMinimum(posMin);
			YSlider->setMinimum(posMin);
			ZSlider->setMinimum(posMin);

			XSlider->setMaximum(posMax);
			YSlider->setMaximum(posMax);
			ZSlider->setMaximum(posMax);
		}
		else // Rot
		{
			values = shape_transform_->getRotationDegrees();
			XSlider->setMinimum(rotMin);
			YSlider->setMinimum(rotMin);
			ZSlider->setMinimum(rotMin);

			XSlider->setMaximum(rotMax);
			YSlider->setMaximum(rotMax);
			ZSlider->setMaximum(rotMax);
		}

		XSlider->setValue(static_cast<int>(values.x));
		YSlider->setValue(static_cast<int>(values.y));
		ZSlider->setValue(static_cast<int>(values.z));
	}

	void onSliderUpdate(int x)
	{
		int i = TransformSelector->currentIndex();

		glm::vec3 values(XSlider->value(), YSlider->value(), ZSlider->value());

		if (i == 0) // Scale
		{
			shape_transform_->setScale(values);
		}
		else if (i == 1) // Pos
		{
			shape_transform_->setPos(values);
		}
		else // Rot
		{
			shape_transform_->setRotationDegrees(values);
		}
	}

public:
	QVBoxLayout* verticalLayout;
	QComboBox* TransformSelector;
	QSlider* XSlider;
	QSlider* YSlider;
	QSlider* ZSlider;

	Transform* shape_transform_;

	const int posMin = -50;
	const int posMax = - posMin;

	const int scaleMin = 0;
	const int scaleMax = 30;

	const int rotMin = -45;
	const int rotMax = -rotMin;

	QTransformEditor(QWidget* parent): QWidget(parent)
	{
		setMinimumSize(120, 50);
		verticalLayout = new QVBoxLayout(this);
		verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
		verticalLayout->setContentsMargins(0, 0, 0, 0);

		TransformSelector = new QComboBox(this);
		TransformSelector->addItem(QString("Scale"));
		TransformSelector->addItem(QString("Position"));
		TransformSelector->addItem(QString("Rotation"));
		TransformSelector->setObjectName(QString::fromUtf8("TransformSelector"));

		verticalLayout->addWidget(TransformSelector);

		XSlider = new QSlider(this);
		XSlider->setObjectName(QString::fromUtf8("XSlider"));
		XSlider->setOrientation(Qt::Horizontal);

		verticalLayout->addWidget(XSlider);

		YSlider = new QSlider(this);
		YSlider->setObjectName(QString::fromUtf8("YSlider"));
		YSlider->setOrientation(Qt::Horizontal);

		verticalLayout->addWidget(YSlider);

		ZSlider = new QSlider(this);
		ZSlider->setObjectName(QString::fromUtf8("ZSlider"));
		ZSlider->setOrientation(Qt::Horizontal);

		verticalLayout->addWidget(ZSlider);

		connect(TransformSelector, &QComboBox::currentIndexChanged, this, &QTransformEditor::onSelectorUpdate);
		connect(XSlider, &QSlider::valueChanged, this, &QTransformEditor::onSliderUpdate);
		connect(YSlider, &QSlider::valueChanged, this, &QTransformEditor::onSliderUpdate);
		connect(ZSlider, &QSlider::valueChanged, this, &QTransformEditor::onSliderUpdate);
	}

	void bindWidgetToShape(Transform* transform)
	{
		shape_transform_ = transform;
		onSelectorUpdate(0);
	}
};
