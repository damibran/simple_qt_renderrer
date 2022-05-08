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

		const int x = glm::clamp(static_cast<int>(tex_coord.x * width), 0, width);
		const int y = glm::clamp(static_cast<int>(tex_coord.y * height), 0, height);


		return {
			data.get()[(width * y + x) * nComp] / 255.,
			data.get()[(width * y + x) * nComp + 1] / 255.,
			data.get()[(width * y + x) * nComp + 2] / 255.
		};
	}

	int width, height, nComp;
	std::unique_ptr<unsigned char> data;
};
