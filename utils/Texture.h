#pragma once

struct Texture
{
	Texture() = default;

	Texture(Texture&& text)
	{
		width = text.width;
		height = text.height;
		nComp = text.nComp;
		data = std::move(text.data);
	}

	glm::vec3 sampleTexture(const glm::vec2& tex_coord) const
	{
		int t_x = tex_coord.x * width;
		int t_y = tex_coord.y * height;

		const int x = glm::clamp(t_x, 0, width - 1);
		const int y = glm::clamp(t_y, 0, height - 1);


		return {
			data.get()[(width * y + x) * nComp] / 255.,
			data.get()[(width * y + x) * nComp + 1] / 255.,
			data.get()[(width * y + x) * nComp + 2] / 255.
		};
	}

	glm::vec3 sampleTexturerRepeat(float scale,const glm::vec2& tex_coord) const
	{
		int t_x = scale * tex_coord.x * width;
		int t_y = scale * tex_coord.y * height;

		const int x = t_x % (width - 1);
		const int y = t_y % (height - 1);

		return {
			data.get()[(width * y + x) * nComp] / 255.,
			data.get()[(width * y + x) * nComp + 1] / 255.,
			data.get()[(width * y + x) * nComp + 2] / 255.
		};
	}

	int width, height, nComp;
	std::unique_ptr<unsigned char> data;
};
