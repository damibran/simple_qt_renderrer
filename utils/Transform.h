#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
public:
	void translate(const glm::vec3& v)
	{
		position_ = glm::translate(glm::mat4(1), v) * glm::vec4(position_, 1);
	}

	void setPos(const glm::vec3& v)
	{
		position_ = v;
	}

	[[nodiscard]] glm::vec3 getPos() const
	{
		return position_;
	}

	void setRotationDegrees(const glm::vec3 v)
	{
		rotation_ = v;
	}

	glm::vec3 getRotationDegrees() const
	{
		return rotation_;
	}

	void setPitch(float v)
	{
		rotation_.x = v;
	}

	void scale(const glm::vec3& factor)
	{
		scaling_ *= factor;
	}

	void setScale(const glm::vec3& v)
	{
		scaling_ = v;
	}

	[[nodiscard]] glm::mat4 getRotationMatrix() const
	{
		glm::mat4 m(1.);
		m = glm::rotate(m, glm::radians(rotation_.x), { 1, 0, 0 });
		m = glm::rotate(m, glm::radians(rotation_.y), { 0, 1, 0 });
		m = glm::rotate(m, glm::radians(rotation_.z), { 0, 0, 1 });
		return m;
	}

	glm::mat4 getFullModelMatrix() const
	{
		return glm::translate(glm::mat4(1), position_) * getRotationMatrix() *
			glm::scale(glm::mat4(1), scaling_);
	}

private:
	glm::vec3 position_ = glm::vec3(0.0f);
	glm::vec3 rotation_ = glm::vec3(0.f); // pitch, yaw, roll in degrees
	glm::vec3 scaling_ = glm::vec3(1.0f);
};