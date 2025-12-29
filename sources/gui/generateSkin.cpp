#include <cage-core/color.h>
#include <cage-core/image.h>
#include <cage-core/logger.h>
#include <cage-engine/guiSkins.h>

using namespace cage;

namespace
{
	Vec4 interpolateColor(const Vec4 &rgba1, const Vec4 &rgba2, Real factor)
	{
		return Vec4(interpolateColor(Vec3(rgba1), Vec3(rgba2), factor), interpolate(rgba1[3], rgba2[3], factor));
	}

#define interpolateColor interpolate

	Vec4 blend(Vec4 under, Vec4 over)
	{
		return Vec4(interpolate(Vec3(under), Vec3(over), over[3]), under[3]);
	}

	Vec2 tryNormalize(Vec2 dir)
	{
		if (lengthSquared(dir) > 1e-7)
			return normalize(dir);
		return {};
	}

	Real insideRadius(const Vec4 rect, Real gx, Real gy)
	{
		Real w = rect[2] - rect[0];
		Real h = rect[3] - rect[1];
		gx -= rect[0];
		gy -= rect[1];
		if (h > w)
		{
			std::swap(w, h);
			std::swap(gx, gy);
		}
		if (gx > w / 2)
			gx = w - gx;
		if (gy > h / 2)
			gy = h - gy;
		h *= 0.5;
		if (gx > h)
			return saturate(1 - gy / h);
		return saturate(length(h - Vec2(gx, gy)) / h);
	}

	Vec4 colorInside(GuiElementTypeEnum type, uint32 form, const Vec4 rect, Real gx, Real gy)
	{
		switch (type)
		{
			case GuiElementTypeEnum::PanelBase:
			case GuiElementTypeEnum::PanelCaption:
			{
				switch (form)
				{
					case 0:
					case 1:
					case 2:
						return Vec4(39, 37, 68, 255) / 255;
					case 4:
					case 5:
					case 6:
						return Vec4(125, 101, 49, 255) / 255;
					case 3:
					case 7:
						return Vec4(187, 187, 187, 255) / 255;
				}
				break;
			}
			case GuiElementTypeEnum::SeparatorHorizontalLine:
			case GuiElementTypeEnum::SeparatorVerticalLine:
			{
				const Real ir = insideRadius(rect, gx, gy);
				const Real radius = saturate(1 - pow(ir * 2, 4)) * 255;
				switch (form)
				{
					case 0:
					case 1:
					case 2:
						return Vec4(26, 116, 155, radius) / 255;
					case 4:
					case 5:
					case 6:
						return Vec4(205, 188, 133, radius) / 255;
					case 3:
					case 7:
						return Vec4(204, 204, 204, radius) / 255;
				}
				break;
			}
			case GuiElementTypeEnum::SpoilerCaption:
			case GuiElementTypeEnum::Button:
			case GuiElementTypeEnum::InputButtonDecrement:
			case GuiElementTypeEnum::InputButtonIncrement:
			case GuiElementTypeEnum::ComboBoxBase:
			case GuiElementTypeEnum::ComboBoxItemUnchecked:
			case GuiElementTypeEnum::CheckBoxChecked:
			case GuiElementTypeEnum::CheckBoxUnchecked:
			case GuiElementTypeEnum::CheckBoxIndetermined:
			{
				const Vec3 base = Vec3(colorInside(GuiElementTypeEnum::InvalidElement, form, rect, gx, gy));
				const Vec3 a = colorHsvToRgb(colorRgbToHsv(base) + Vec3(0, 0, 0.13));
				const Vec3 b = colorHsvToRgb(colorRgbToHsv(base) + Vec3(0, 0, -0.13));
				const Real f = saturate((gy - rect[1]) / (rect[3] - rect[1]));
				return Vec4(interpolateColor(a, b, f), 1);
			}
			case GuiElementTypeEnum::ComboBoxItemChecked:
			{
				const Vec4 base = colorInside(GuiElementTypeEnum::ComboBoxItemUnchecked, form, rect, gx, gy);
				Vec3 hsv = colorRgbToHsv(Vec3(base));
				hsv[2] = interpolate(hsv[2], 1, 0.4);
				return Vec4(colorHsvToRgb(hsv), base[3]);
			}
			case GuiElementTypeEnum::ScrollbarHorizontalPanel:
			case GuiElementTypeEnum::ScrollbarVerticalPanel:
			{
				const Vec4 base = colorInside(GuiElementTypeEnum::InvalidElement, form, rect, gx, gy);
				Vec3 hsv = colorRgbToHsv(Vec3(base));
				hsv[2] *= 0.5;
				return Vec4(colorHsvToRgb(hsv), base[3]);
			}
			case GuiElementTypeEnum::ScrollbarHorizontalDot:
			case GuiElementTypeEnum::ScrollbarVerticalDot:
			{
				const Vec4 base = colorInside(GuiElementTypeEnum::InvalidElement, form, rect, gx, gy);
				Vec3 hsv = colorRgbToHsv(Vec3(base));
				hsv[2] = interpolate(hsv[2], 1, 0.3);
				return Vec4(colorHsvToRgb(hsv), base[3]);
			}
			case GuiElementTypeEnum::SliderHorizontalDot:
			case GuiElementTypeEnum::SliderVerticalDot:
			{
				const Vec4 base = colorInside(GuiElementTypeEnum::InvalidElement, form, rect, gx, gy);
				Vec3 hsv = colorRgbToHsv(Vec3(base));
				hsv[2] = interpolate(hsv[2], 1, 0.5);
				return Vec4(colorHsvToRgb(hsv), base[3]);
			}
			case GuiElementTypeEnum::Frame:
				return Vec4(Vec3(colorInside(GuiElementTypeEnum::PanelBase, form, rect, gx, gy)), 0);
			case GuiElementTypeEnum::SpoilerBase:
				return Vec4(0);
		}

		switch (form)
		{
			case 0:
				return Vec4(36, 48, 94, 255) / 255;
			case 1:
				return Vec4(65, 81, 143, 255) / 255;
			case 2:
				return Vec4(80, 110, 150, 255) / 255;
			case 3:
				return Vec4(170, 170, 170, 255) / 255;
			case 4:
				return Vec4(171, 139, 116, 255) / 255;
			case 5:
				return Vec4(152, 147, 128, 255) / 255;
			case 6:
				return Vec4(182, 137, 108, 255) / 255;
			case 7:
				return Vec4(170, 170, 170, 255) / 255;
		}

		return Vec4(0, 0, 0, 1);
	}

	Real findEdge(const GuiSkinElementLayout::TextureUvOi &rects, Real gx, Real gy)
	{
		const auto &find = [](Real x1, Real x2, Real x3, Real x4, Real x) -> Real
		{
			const Real mid = (x2 + x3) * 0.5;
			if (x < mid)
				return saturate((x2 - x) / (x2 - x1));
			else
				return saturate((x - x3) / (x4 - x3));
		};
		const Real x = find(rects.outer[0], rects.inner[0], rects.inner[2], rects.outer[2], gx);
		const Real y = find(rects.outer[1], rects.inner[1], rects.inner[3], rects.outer[3], gy);
		return saturate(sqrt(sqr(x) + sqr(y)));
	}

	Vec2 sunDir(Real edge)
	{
		static const Vec2 defaultDir = normalize(Vec2(1, 2));
		const Real offset = smootherstep(smootherstep(edge)) * 1.5 - 1;
		return defaultDir * offset;
	}

	Vec3 borderTransition(const GuiSkinElementLayout::TextureUvOi &rects, Real gx, Real gy, Real edge, Vec3 colorDark, Vec3 colorLight)
	{
		const auto &find = [](Real x1, Real x2, Real x3, Real x4, Real x) -> Real
		{
			if (x < x2)
				return -saturate((x2 - x) / (x2 - x1));
			if (x > x3)
				return saturate((x - x3) / (x4 - x3));
			return 0;
		};
		const Real x = find(rects.outer[0], rects.inner[0], rects.inner[2], rects.outer[2], gx);
		const Real y = -find(rects.outer[1], rects.inner[1], rects.inner[3], rects.outer[3], gy);
		const Vec2 dir = tryNormalize(Vec2(x, y));
		const Real sun = saturate(dot(dir, sunDir(edge)) * 0.5 + 0.5);
		return interpolateColor(colorDark, colorLight, sun);
	}

	std::pair<Vec3, Vec3> borderColors(GuiElementTypeEnum type, uint32 form)
	{
		switch (type)
		{
			case GuiElementTypeEnum::Frame:
			case cage::GuiElementTypeEnum::Header:
			{
				switch (form)
				{
					case 0:
					case 1:
					case 2:
						return { Vec3(30, 52, 71) / 255, Vec3(86, 216, 255) / 255 };
					case 4:
					case 5:
					case 6:
						return { Vec3(151, 121, 86) / 255, Vec3(255, 238, 183) / 255 };
					case 3:
					case 7:
						return { Vec3(204) / 255, Vec3(204) / 255 };
				}
				break;
			}
		}

		switch (form)
		{
			case 0:
			case 1:
			case 2:
				return { Vec3(50, 52, 51) / 255, Vec3(186, 206, 225) / 255 };
			case 4:
			case 5:
			case 6:
				return { Vec3(131, 121, 116) / 255, Vec3(245, 228, 203) / 255 };
			case 3:
			case 7:
				return { Vec3(204) / 255, Vec3(204) / 255 };
		}

		return {};
	}

	Vec4 colorBorder(GuiElementTypeEnum type, uint32 form, const GuiSkinElementLayout::TextureUvOi &rects, Real gx, Real gy, const Vec4 insideColor)
	{
		switch (type)
		{
			case GuiElementTypeEnum::SpoilerBase:
				return Vec4(0);
		}

		const Real edge = findEdge(rects, gx, gy);
		const Real opacity = saturate(1 - pow(edge, 50));
		const Real mixing = saturate(edge * 2 - 0.2);
		const auto baseColors = borderColors(type, form);
		const Vec3 baseColor = borderTransition(rects, gx, gy, edge, baseColors.first, baseColors.second);
		return interpolateColor(insideColor, Vec4(baseColor, opacity), mixing);
	}

	Vec4 colorRadioBox(GuiElementTypeEnum type, uint32 form, const Vec4 rect, Real gx, Real gy)
	{
		const Vec2 center = Vec2(rect[0] + rect[2], rect[1] + rect[3]) * 0.5;
		const Real radius = length(Vec2(gx, gy) - center) * 2 / min(rect[2] - rect[0], rect[3] - rect[1]);

		Vec4 inner = colorInside(GuiElementTypeEnum::Button, form, rect, gx, gy);
		inner[3] *= saturate((0.95 - radius) / 0.05);

		const Vec2 dir = tryNormalize((Vec2(gx, gy) - center) * Vec2(-1, 1));
		const Real sun = saturate(dot(dir, sunDir(radius)) * 0.5 + 0.5);
		const auto borderBases = borderColors(type, form);
		const Vec3 border = interpolateColor(borderBases.first, borderBases.second, sun);

		const Real factor = saturate(1 - abs(radius - 0.8) / 0.2);
		return interpolateColor(inner, Vec4(border, 1), factor);
	}

	Vec4 colorTriangle(GuiElementTypeEnum type, uint32 form, const Vec4 rect, Real gx, Real gy)
	{
		const Real w = rect[2] - rect[0];
		const Real h = rect[3] - rect[1];
		Real x = (gx - rect[0]) / w;
		Real y = (gy - rect[1]) / h;
		if (x < 0 || y < 0 || x > 1 || y > 1)
			return Vec4(0);

		Vec3 color = colorRgbToHsv(Vec3(colorInside(GuiElementTypeEnum::InvalidElement, form, rect, gx, gy)));
		color[2] = interpolate(color[2], 1, 0.7);
		color = colorHsvToRgb(color);

		switch (type)
		{
			case GuiElementTypeEnum::ComboBoxIconShown:
			case GuiElementTypeEnum::SpoilerIconShown:
				break;
			case GuiElementTypeEnum::InputButtonIncrement:
				std::swap(x, y);
				break;
			case GuiElementTypeEnum::ComboBoxIconCollapsed:
			case GuiElementTypeEnum::SpoilerIconCollapsed:
			case GuiElementTypeEnum::InputButtonDecrement:
				x = 1 - x;
				std::swap(x, y);
				break;
		}

		const Real k = (1 - y) * 0.5;
		const Real opacity = saturate((k - abs(x - 0.5)) * 50);
		return Vec4(color, opacity);
	}

	Real sdfSegment(Vec2 p, Vec2 a, Vec2 b)
	{
		Vec2 pa = p - a;
		Vec2 ba = b - a;
		Real h = saturate(dot(pa, ba) / dot(ba, ba));
		return length(pa - ba * h);
	}

	Real sdfCheckMark(Vec2 pos)
	{
		pos[1] = 1 - pos[1];
		Vec2 p = pos - 0.5;
		p *= 1.6;

		// checkmark points
		Vec2 a = Vec2(-0.4, 0);
		Vec2 b = Vec2(-0.05, -0.3);
		Vec2 c = Vec2(0.45, 0.35);

		Real d1 = sdfSegment(p, a, b);
		Real d2 = sdfSegment(p, b, c);

		Real thickness = 0.15;
		return min(d1, d2) - thickness;
	}

	Real sdfQuestionMark(Vec2 pos)
	{
		pos[1] = 1 - pos[1];
		Vec2 p = pos - 0.5;

		//  top arc
		Vec2 arcCenter = Vec2(0, 0.13);
		Real arcRadius = 0.2;
		Vec2 ap = p - arcCenter;

		Real angle = atan2(ap[0], ap[1]).value;
		Real arcMask = max(angle - 3.6, -angle - 0.8); // limits arc angles
		Real dArc = abs(length(ap) - arcRadius);
		dArc = max(dArc, arcMask);

		// stem
		Rads stemAngle = Rads(-0.5);
		Vec2 stemA = arcCenter + Vec2(cos(stemAngle), sin(stemAngle)) * arcRadius;
		Vec2 stemB = Vec2(0.05, -0.1);
		Real dStem = sdfSegment(p, stemA, stemB);

		// dot
		Vec2 dotCenter = Vec2(0, -0.3);
		Real dDot = length(p - dotCenter) - 0.03;

		Real d = min(min(dArc, dStem), dDot);
		return d - 0.05;
	}

	Real sdfDiamond(Vec2 pos)
	{
		Vec2 p = abs(pos - 0.5);
		return p[0] + p[1] - 0.5;
	}

	Vec4 colorCheckMark(GuiElementTypeEnum type, uint32 form, const Vec4 rect, Real gx, Real gy)
	{
		const Vec2 orig = Vec2(rect[0], rect[1]);
		Vec2 pos = (Vec2(gx, gy) - orig) / (Vec2(rect[2], rect[3]) - orig);
		switch (type)
		{
			case GuiElementTypeEnum::CheckBoxChecked:
			case GuiElementTypeEnum::CheckBoxIndetermined:
				pos = (pos - 0.5) * 0.85 + 0.5;
				break;
			case GuiElementTypeEnum::RadioBoxChecked:
			case GuiElementTypeEnum::RadioBoxIndetermined:
				pos = (pos - 0.5) * 1.4 + 0.5;
				break;
		}

		Real sd = 1;
		switch (type)
		{
			case GuiElementTypeEnum::RadioBoxChecked:
			case GuiElementTypeEnum::CheckBoxChecked:
				sd = sdfCheckMark(pos);
				break;
			case GuiElementTypeEnum::RadioBoxIndetermined:
			case GuiElementTypeEnum::CheckBoxIndetermined:
				sd = sdfQuestionMark(pos);
				break;
			case GuiElementTypeEnum::SliderHorizontalDot:
			case GuiElementTypeEnum::SliderVerticalDot:
				sd = sdfDiamond(pos);
				break;
		}

		Vec3 color = colorRgbToHsv(Vec3(colorInside(GuiElementTypeEnum::InvalidElement, form, rect, gx, gy)));
		color[2] = interpolate(color[2], 1, 0.8);
		color = colorHsvToRgb(color);
		return Vec4(color, 1) * saturate(sd * -30 + 0.5);
	}

	Vec4 findColor(GuiElementTypeEnum type, uint32 form, const GuiSkinElementLayout::TextureUvOi &rects, Real gx, Real gy)
	{
		switch (type)
		{
			case GuiElementTypeEnum::RadioBoxChecked:
			case GuiElementTypeEnum::RadioBoxUnchecked:
			case GuiElementTypeEnum::RadioBoxIndetermined:
			{
				const Vec4 color = colorRadioBox(type, form, rects.inner, gx, gy);
				const Vec4 mark = colorCheckMark(type, form, rects.inner, gx, gy);
				return blend(color, mark);
			}
			case GuiElementTypeEnum::ComboBoxIconCollapsed:
			case GuiElementTypeEnum::ComboBoxIconShown:
			case GuiElementTypeEnum::SpoilerIconCollapsed:
			case GuiElementTypeEnum::SpoilerIconShown:
				return colorTriangle(type, form, rects.inner, gx, gy);
			case GuiElementTypeEnum::SliderHorizontalDot:
			case GuiElementTypeEnum::SliderVerticalDot:
				return colorCheckMark(type, form, rects.inner, gx, gy);
		}

		const Vec4 insideColor = colorInside(type, form, rects.inner, gx, gy);
		const Vec4 borderColor = colorBorder(type, form, rects, gx, gy, insideColor);
		const bool inside = gx > rects.inner[0] - 1e-5 && gx < rects.inner[2] && gy > rects.inner[1] - 1e-5 && gy < rects.inner[3];
		Vec4 color = interpolateColor(borderColor, insideColor, inside);

		switch (type)
		{
			case cage::GuiElementTypeEnum::SpoilerCaption:
			case cage::GuiElementTypeEnum::Header:
			{
				const Real x1 = rects.inner[0];
				const Real x2 = rects.inner[2];
				const Real mid = (x1 + x2) * 0.5;
				const Real x = saturate(abs((gx - mid) / (x2 - mid)));
				color[3] *= 1 - x * x * x * x;
				break;
			}
			case GuiElementTypeEnum::InputButtonDecrement:
			case GuiElementTypeEnum::InputButtonIncrement:
			{
				const Vec4 triangle = colorTriangle(type, form, rects.inner, gx, gy);
				color = blend(color, triangle);
				break;
			}
			case GuiElementTypeEnum::CheckBoxChecked:
			case GuiElementTypeEnum::RadioBoxChecked:
			case GuiElementTypeEnum::CheckBoxIndetermined:
			case GuiElementTypeEnum::RadioBoxIndetermined:
			{
				const Vec4 mark = colorCheckMark(type, form, rects.inner, gx, gy);
				color = blend(color, mark);
				break;
			}
		}

		return color;
	}

	// default form: 0 = normal, 1 = focus, 2 = hover, 3 = disabled
	// tooltip form: 4 = normal, 5 = focus, 6 = hover, 7 = disabled
	void renderWidget(Image *png, GuiElementTypeEnum type, uint32 form, const GuiSkinElementLayout::TextureUvOi &rects)
	{
		const uint32 iw = png->width();
		const uint32 ih = png->width();
		const uint32 x1 = numeric_cast<uint32>(rects.outer[0] * iw);
		const uint32 x2 = numeric_cast<uint32>(rects.outer[2] * iw);
		const uint32 y1 = numeric_cast<uint32>(rects.outer[1] * ih);
		const uint32 y2 = numeric_cast<uint32>(rects.outer[3] * ih);

		for (uint32 y = y1; y < y2; y++)
		{
			for (uint32 x = x1; x < x2; x++)
			{
				const Vec4 c = findColor(type, form, rects, Real(x) / iw, Real(y) / ih);
				png->set(x, y, c);
			}
		}
	}

	Holder<Image> renderSkin(bool tooltip)
	{
		const GuiSkinConfig skin = detail::guiSkinGenerate(GuiSkinDefault);
		Holder<Image> png = newImage();
		png->initialize(2048, 2048, 4);
		for (uint32 type = 0; type < (uint32)GuiElementTypeEnum::TotalElements; type++)
		{
			const GuiSkinElementLayout::TextureUv &element = skin.layouts[type].textureUv;
			for (uint32 i = 0; i < 4; i++)
				renderWidget(+png, (GuiElementTypeEnum)type, i + tooltip * 4, element.data[i]);
		}
		return png;
	}

	void renderTextures()
	{
		renderSkin(false)->exportFile("skinDefault.png");
		renderSkin(true)->exportFile("skinTooltips.png");
	}
}

int main()
{
	try
	{
		initializeConsoleLogger();
		renderTextures();
		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
