#pragma once

struct RGBA {
	int r;
	int g;
	int b;
	int a;
};

struct HSVA {
	int h;
	int s;
	int v;
	int a;
};

class Color
{
public:
	float r = 1;
	float g = 1;
	float b = 1;
	float a = 1;

	Color() {}
	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

	bool operator==(const Color& o) const;

	Color operator+(const Color& o) const {
		Color temp = *this;
		temp.r += o.r;
		temp.g += o.g;
		temp.b += o.b;
		temp.a += o.a;
		return temp;
	}
	Color operator*(const float& f) const {
		Color temp = *this;
		temp.r *= f;
		temp.g *= f;
		temp.b *= f;
		temp.a *= f;
		return temp;
	}

	static Color convertFromHSVA(HSVA hsva) {
		return convertFromHSVA(hsva.h, hsva.s, hsva.v, hsva.a);
	}
	static Color convertFromHSVA(int h, int s, int v, int a);
};

