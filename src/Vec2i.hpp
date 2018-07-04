#ifndef RENDERER_VEC2I_HPP
#define RENDERER_VEC2I_HPP


struct Vec2i
{
public:
	int x;
	int y;


	explicit Vec2i(int x = 0, int y = 0)
			: x(x)
			, y(y)
	{}


	Vec2i(const Vec2i& other) = default;

	Vec2i& operator=(const Vec2i& other) = default;
};


#endif
