#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/macros.h>
#include <cage-core/string.h>

#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineStatistics.h>
#include <cage-engine/fpsCamera.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;

constexpr uint32 assetsName = HashString("cage-tests/camera-effects/effects.pack");

bool windowClose()
{
	engineStop();
	return false;
}

void enableEffect(CameraEffectsFlags effect, bool enable)
{
	Entity *camera = engineEntities()->get(1);
	CAGE_COMPONENT_ENGINE(Camera, cam, camera);
	if (enable)
		cam.effects = cam.effects | effect;
	else
		cam.effects = cam.effects & ~effect;
}

constexpr sint32 genBaseName(CameraEffectsFlags f_)
{
	sint32 ret = 1;
	uint32 f = (uint32)f_;
	while (f)
	{
		f /= 2;
		ret++;
	}
	return ret * 10;
}

bool update()
{
	EntityManager *ents = engineGui()->entities();

	Entity *camera = engineEntities()->get(1);
	CAGE_COMPONENT_ENGINE(Camera, cam, camera);

	{ // ambient occlusion
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::AmbientOcclusion);
		{ // enable
			Entity *e = ents->get(baseName);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::AmbientOcclusion, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // world radius
			Entity *e = ents->get(baseName + 1);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.ssao.worldRadius = toFloat(in.value);
		}
		{ // strength
			Entity *e = ents->get(baseName + 2);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.ssao.strength = toFloat(in.value);
		}
		{ // bias
			Entity *e = ents->get(baseName + 3);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.ssao.bias = toFloat(in.value);
		}
		{ // power
			Entity *e = ents->get(baseName + 4);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.ssao.power = toFloat(in.value);
		}
		{ // samples count
			Entity *e = ents->get(baseName + 5);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.ssao.samplesCount = toUint32(in.value);
		}
		{ // blur passes
			Entity *e = ents->get(baseName + 6);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.ssao.blurPasses = toUint32(in.value);
		}
	}

	{ // depth of field
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::DepthOfField);
		{ // enable
			Entity *e = ents->get(baseName);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::DepthOfField, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // focus distance
			Entity *e = ents->get(baseName + 1);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.depthOfField.focusDistance = toFloat(in.value);
		}
		{ // focus radius
			Entity *e = ents->get(baseName + 2);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.depthOfField.focusRadius = toFloat(in.value);
		}
		{ // blend radius
			Entity *e = ents->get(baseName + 3);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.depthOfField.blendRadius = toFloat(in.value);
		}
		{ // blur passes
			Entity *e = ents->get(baseName + 4);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.depthOfField.blurPasses = toUint32(in.value);
		}
	}

	{ // motion blur
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::MotionBlur);
		{ // enable
			Entity *e = ents->get(baseName);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::MotionBlur, cb.state == CheckBoxStateEnum::Checked);
		}
	}

	{ // bloom
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::Bloom);
		{ // enable
			Entity *e = ents->get(baseName);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::Bloom, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // threshold
			Entity *e = ents->get(baseName + 1);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.bloom.threshold = toFloat(in.value);
		}
		{ // blur passes
			Entity *e = ents->get(baseName + 2);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.bloom.blurPasses = toUint32(in.value);
		}
	}

	{ // eye adaptation
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::EyeAdaptation);
		{ // enable
			Entity *e = ents->get(baseName);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::EyeAdaptation, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // key
			Entity *e = ents->get(baseName + 1);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.eyeAdaptation.key = toFloat(in.value);
		}
		{ // strength
			Entity *e = ents->get(baseName + 2);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.eyeAdaptation.strength = toFloat(in.value);
		}
		{ // darkerSpeed
			Entity *e = ents->get(baseName + 3);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.eyeAdaptation.darkerSpeed = toFloat(in.value);
		}
		{ // lighterSpeed
			Entity *e = ents->get(baseName + 4);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.eyeAdaptation.lighterSpeed = toFloat(in.value);
		}
	}

	{ // tone mapping
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::ToneMapping);
		{ // enable
			Entity *e = ents->get(baseName);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::ToneMapping, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // shoulderStrength
			Entity *e = ents->get(baseName + 1);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.shoulderStrength = toFloat(in.value);
		}
		{ // linearStrength
			Entity *e = ents->get(baseName + 2);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.linearStrength = toFloat(in.value);
		}
		{ // linearAngle
			Entity *e = ents->get(baseName + 3);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.linearAngle = toFloat(in.value);
		}
		{ // toeStrength
			Entity *e = ents->get(baseName + 4);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.toeStrength = toFloat(in.value);
		}
		{ // toeNumerator
			Entity *e = ents->get(baseName + 5);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.toeNumerator = toFloat(in.value);
		}
		{ // toeDenominator
			Entity *e = ents->get(baseName + 6);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.toeDenominator = toFloat(in.value);
		}
		{ // white
			Entity *e = ents->get(baseName + 7);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.white = toFloat(in.value);
		}
	}

	{ // gamma
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::GammaCorrection);
		{ // enable
			Entity *e = ents->get(baseName);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::GammaCorrection, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // gamma
			Entity *e = ents->get(baseName + 1);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.gamma = toFloat(in.value);
		}
	}

	{ // antialiasing
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::AntiAliasing);
		{ // enable
			Entity *e = ents->get(baseName);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::AntiAliasing, cb.state == CheckBoxStateEnum::Checked);
		}
	}

	return false;
}

Entity *genInputFloat(Entity *table, sint32 &childIndex, uint32 nameBase, const string &labelText, real rangeMin, real rangeMax, real step, real current)
{
	EntityManager *ents = engineGui()->entities();
	{
		Entity *e = ents->createUnique();
		CAGE_COMPONENT_GUI(Parent, p, e);
		p.parent = table->name();
		p.order = childIndex++;
		CAGE_COMPONENT_GUI(Label, label, e);
		CAGE_COMPONENT_GUI(Text, t, e);
		t.value = labelText;
	}
	{
		Entity *e = ents->create(nameBase + childIndex / 2);
		CAGE_COMPONENT_GUI(Parent, p, e);
		p.parent = table->name();
		p.order = childIndex++;
		CAGE_COMPONENT_GUI(Input, in, e);
		in.type = InputTypeEnum::Real;
		in.min.f = rangeMin;
		in.max.f = rangeMax;
		in.step.f = step;
		in.value = stringizer() + current;
		return e;
	}
}

Entity *genInputInt(Entity *table, sint32 &childIndex, uint32 nameBase, const string &labelText, sint32 rangeMin, sint32 rangeMax, sint32 step, sint32 current)
{
	Entity *e = genInputFloat(table, childIndex, nameBase, labelText, 0, 0, 0, 0);
	CAGE_COMPONENT_GUI(Input, in, e);
	in.type = InputTypeEnum::Integer;
	in.min.i = rangeMin;
	in.max.i = rangeMax;
	in.step.i = step;
	in.value = stringizer() + current;
	return e;
}

constexpr CheckBoxStateEnum genEnabled(CameraEffectsFlags f)
{
	return any(CameraEffectsFlags::Default & f) ? CheckBoxStateEnum::Checked : CheckBoxStateEnum::Unchecked;
}

void initializeGui()
{
	EntityManager *ents = engineGui()->entities();
	Entity *layout = ents->createUnique();
	{ // layout
		CAGE_COMPONENT_GUI(Scrollbars, sc, layout);
		CAGE_COMPONENT_GUI(LayoutLine, l, layout);
		l.vertical = true;
	}

	{ // ambient occlusion
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::AmbientOcclusion);
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = baseName;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Ambient Occlusion";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = genEnabled(CameraEffectsFlags::AmbientOcclusion);
		}
		Entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(LayoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "World Radius:", 0.1, 3, 0.05, CameraEffects().ssao.worldRadius);
		genInputFloat(table, childIndex, baseName, "Strength:", 0, 3, 0.1, CameraEffects().ssao.strength);
		genInputFloat(table, childIndex, baseName, "Bias:", -0.5, 0.5, 0.01, CameraEffects().ssao.bias);
		genInputFloat(table, childIndex, baseName, "Power:", 0.1, 2, 0.02, CameraEffects().ssao.power);
		genInputInt(table, childIndex, baseName, "Samples:", 1, 128, 1, CameraEffects().ssao.samplesCount);
		genInputInt(table, childIndex, baseName, "Blur Passes:", 0, 10, 1, CameraEffects().ssao.blurPasses);
	}

	{ // depth of field
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::DepthOfField);
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = baseName;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Depth of field";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = genEnabled(CameraEffectsFlags::DepthOfField);
		}
		Entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(LayoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "Focus distance:", 0, 20, 0.5, CameraEffects().depthOfField.focusDistance);
		genInputFloat(table, childIndex, baseName, "Focus radius:", 0, 20, 0.5, CameraEffects().depthOfField.focusRadius);
		genInputFloat(table, childIndex, baseName, "Blend radius:", 0, 20, 0.5, CameraEffects().depthOfField.blendRadius);
		genInputInt(table, childIndex, baseName, "Blur passes:", 0, 10, 1, CameraEffects().depthOfField.blurPasses);
	}

	{ // motion blur
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::MotionBlur);
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = baseName;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Motion Blur";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = genEnabled(CameraEffectsFlags::MotionBlur);
		}
	}

	{ // bloom
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::Bloom);
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = baseName;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Bloom";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = genEnabled(CameraEffectsFlags::Bloom);
		}
		Entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(LayoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "Threshold:", 0, 5, 0.01, CameraEffects().bloom.threshold);
		genInputInt(table, childIndex, baseName, "Blur Passes:", 1, 10, 1, CameraEffects().bloom.blurPasses);
	}

	{ // eye adaptation
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::EyeAdaptation);
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = baseName;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Eye Adaptation";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = genEnabled(CameraEffectsFlags::EyeAdaptation);
		}
		Entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(LayoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "Key:", 0, 1, 0.02, CameraEffects().eyeAdaptation.key);
		genInputFloat(table, childIndex, baseName, "Strength:", 0, 1, 0.02, CameraEffects().eyeAdaptation.strength);
		genInputFloat(table, childIndex, baseName, "Darker speed:", 0, 5, 0.02, CameraEffects().eyeAdaptation.darkerSpeed);
		genInputFloat(table, childIndex, baseName, "Lighter speed:", 0, 5, 0.02, CameraEffects().eyeAdaptation.lighterSpeed);
	}

	{ // tone mapping
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::ToneMapping);
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = baseName;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Tone Mapping";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = genEnabled(CameraEffectsFlags::ToneMapping);
		}
		Entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(LayoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "Shoulder Strength:", 0, 1, 0.02, CameraEffects().tonemap.shoulderStrength);
		genInputFloat(table, childIndex, baseName, "Linear Strength:", 0, 1, 0.02, CameraEffects().tonemap.linearStrength);
		genInputFloat(table, childIndex, baseName, "Linear Angle:", 0, 1, 0.02, CameraEffects().tonemap.linearAngle);
		genInputFloat(table, childIndex, baseName, "Toe Strength:", 0, 1, 0.02, CameraEffects().tonemap.toeStrength);
		genInputFloat(table, childIndex, baseName, "Toe Numerator:", 0, 1, 0.02, CameraEffects().tonemap.toeNumerator);
		genInputFloat(table, childIndex, baseName, "Toe Denominator:", 0, 1, 0.02, CameraEffects().tonemap.toeDenominator);
		genInputFloat(table, childIndex, baseName, "White:", 0, 100, 1, CameraEffects().tonemap.white);
	}

	{ // gamma
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::GammaCorrection);
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = baseName;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Gamma";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = genEnabled(CameraEffectsFlags::GammaCorrection);
		}
		Entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(LayoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "Gamma:", 1, 5, 0.1, CameraEffects().gamma);
	}

	{ // antialiasing
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::AntiAliasing);
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = baseName;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Antialiasing";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = genEnabled(CameraEffectsFlags::AntiAliasing);
		}
	}
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		Holder<Logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		engineInitialize(EngineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) EventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, engineWindow()->events.windowClose);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		engineWindow()->title("camera effects");
		engineWindow()->setMaximized();
		initializeGui();

		// entities
		EntityManager *ents = engineEntities();
		{ // camera
			Entity *e = ents->create(1);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(11.5, 1, -1);
			t.orientation = quat(degs(10), degs(110), degs());
			CAGE_COMPONENT_ENGINE(Camera, c, e);
			c.ambientColor = vec3(1);
			c.ambientIntensity = 0.01;
			c.ambientDirectionalColor = vec3(1);
			c.ambientDirectionalIntensity = 0.03;
			c.near = 0.1;
			c.far = 100;
			c.effects = CameraEffectsFlags::Default;
		}
		{ // sun
			Entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(0, 5, 0);
			t.orientation = quat(degs(-75), degs(-120), degs());
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.lightType = LightTypeEnum::Directional;
			l.color = vec3(1);
			l.intensity = 3;
			CAGE_COMPONENT_ENGINE(Shadowmap, s, e);
			s.resolution = 2048;
			s.worldSize = vec3(30);
		}
		{ // floor
			Entity *e = ents->create(10);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("scenes/common/ground.obj");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(0, -1.264425, 0);
		}
		{ // sponza
			Entity *e = ents->create(11);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("scenes/mcguire/crytek/sponza.object");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
		}

		Holder<FpsCamera> fpsCamera = newFpsCamera(ents->get(1));
		fpsCamera->mouseButton = MouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 0.3;
		Holder<EngineStatistics> statistics = newEngineStatistics();

		engineAssets()->add(assetsName);
		engineStart();
		engineAssets()->remove(assetsName);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
