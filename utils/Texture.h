#pragma once

struct Texture
{
	Texture()=default;

	Texture(Texture&& text)
	{
		width=text.width;
		height=text.height;
		nComp=text.nComp;
		data = std::move(text.data);
	}

	int width,height,nComp;
	std::unique_ptr<unsigned char> data;
};