#pragma once

struct iRect
{
	iRect() = default;
	iRect( ivec2 Pos, ivec2 Size )
		:pos(Pos), size(Size) { }

	ivec2 pos = ivec2(0);
	ivec2 size = ivec2(1);
};

struct UISprite
{
	iRect rect;
	vec4 color = vec4(1);
	TextureData* texture = nullptr;
};