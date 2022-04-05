#pragma once
#include <QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>

class QTransformEditor : public QWidget
{
public slots:
	void onSelectorUpdate(int i)
	{
		glm::vec3 values;
		if (i == 0) // Scale
		{
			values = shape_transform_->getScale();
		}
		else if (i == 1) // Pos
		{
			values = shape_transform_->getPos();
		}
		else // Rot
		{
			values = shape_transform_->getRotationDegrees();
		}

		XSlider->setValue(values.x);
		YSlider->setValue(values.y);
		ZSlider->setValue(values.z);
	}

public:
	QVBoxLayout* verticalLayout;
	QLabel* ObjNameLabel;
	QComboBox* TransformSelector;
	QSlider* XSlider;
	QSlider* YSlider;
	QSlider* ZSlider;

	Transform* shape_transform_;

	QTransformEditor(QWidget* parent): QWidget(parent)
	{
		setMinimumSize(120, 50);
		verticalLayout = new QVBoxLayout(this);
		verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
		verticalLayout->setContentsMargins(0, 0, 0, 0);
		ObjNameLabel = new QLabel(this);
		ObjNameLabel->setObjectName(QString::fromUtf8("ObjNameLabel"));

		verticalLayout->addWidget(ObjNameLabel);

		TransformSelector = new QComboBox(this);
		TransformSelector->addItem(QString("Scale"));
		TransformSelector->addItem(QString("Position"));
		TransformSelector->addItem(QString("Rotation"));
		TransformSelector->setObjectName(QString::fromUtf8("TransformSelector"));

		verticalLayout->addWidget(TransformSelector);

		XSlider = new QSlider(this);
		XSlider->setObjectName(QString::fromUtf8("XSlider"));
		XSlider->setMinimum(-90);
		XSlider->setMaximum(90);
		XSlider->setOrientation(Qt::Horizontal);

		verticalLayout->addWidget(XSlider);

		YSlider = new QSlider(this);
		YSlider->setObjectName(QString::fromUtf8("YSlider"));
		YSlider->setMinimum(-90);
		YSlider->setMaximum(90);
		YSlider->setOrientation(Qt::Horizontal);

		verticalLayout->addWidget(YSlider);

		ZSlider = new QSlider(this);
		ZSlider->setObjectName(QString::fromUtf8("ZSlider"));
		ZSlider->setMinimum(-90);
		ZSlider->setMaximum(90);
		ZSlider->setOrientation(Qt::Horizontal);

		verticalLayout->addWidget(ZSlider);

		connect(TransformSelector, &QComboBox::currentIndexChanged, this, &QTransformEditor::onSelectorUpdate);
	}

	void bindWidgetToShape(Transform* transform)
	{
		shape_transform_ = transform;
	}
};
