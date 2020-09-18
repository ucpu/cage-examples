#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/macros.h>
#include <cage-core/string.h>

#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
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

bool update()
{
	EntityManager *ents = engineGui()->entities();

	Entity *camera = engineEntities()->get(1);
	CAGE_COMPONENT_ENGINE(Camera, cam, camera);

	{ // ambient occlusion
		{ // enable
			Entity *e = ents->get(1);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::AmbientOcclusion, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // world radius
			Entity *e = ents->get(2);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.ssao.worldRadius = in.value.toFloat();
		}
		{ // strength
			Entity *e = ents->get(4);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.ssao.strength = in.value.toFloat();
		}
		{ // bias
			Entity *e = ents->get(5);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.ssao.bias = in.value.toFloat();
		}
		{ // power
			Entity *e = ents->get(6);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.ssao.power = in.value.toFloat();
		}
		{ // samples count
			Entity *e = ents->get(7);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.ssao.samplesCount = in.value.toUint32();
		}
		{ // blur passes
			Entity *e = ents->get(8);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.ssao.blurPasses = in.value.toUint32();
		}
	}

	{ // motion blur
		{ // enable
			Entity *e = ents->get(11);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::MotionBlur, cb.state == CheckBoxStateEnum::Checked);
		}
	}

	{ // bloom
		{ // enable
			Entity *e = ents->get(21);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::Bloom, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // threshold
			Entity *e = ents->get(22);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.bloom.threshold = in.value.toFloat();
		}
		{ // blur passes
			Entity *e = ents->get(24);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.bloom.blurPasses = in.value.toUint32();
		}
	}

	{ // eye adaptation
		{ // enable
			Entity *e = ents->get(31);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::EyeAdaptation, cb.state == CheckBoxStateEnum::Checked);
		}
	}

	{ // tone mapping
		{ // enable
			Entity *e = ents->get(41);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::ToneMapping, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // shoulderStrength
			Entity *e = ents->get(42);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.shoulderStrength = in.value.toFloat();
		}
		{ // linearStrength
			Entity *e = ents->get(43);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.linearStrength = in.value.toFloat();
		}
		{ // linearAngle
			Entity *e = ents->get(44);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.linearAngle = in.value.toFloat();
		}
		{ // toeStrength
			Entity *e = ents->get(45);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.toeStrength = in.value.toFloat();
		}
		{ // toeNumerator
			Entity *e = ents->get(46);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.toeNumerator = in.value.toFloat();
		}
		{ // toeDenominator
			Entity *e = ents->get(47);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.toeDenominator = in.value.toFloat();
		}
		{ // white
			Entity *e = ents->get(48);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.tonemap.white = in.value.toFloat();
		}
	}

	{ // gamma
		{ // enable
			Entity *e = ents->get(51);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::GammaCorrection, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // gamma
			Entity *e = ents->get(52);
			CAGE_COMPONENT_GUI(Input, in, e);
			if (in.valid)
				cam.gamma = in.value.toFloat();
		}
	}

	{ // antialiasing
		{ // enable
			Entity *e = ents->get(61);
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			enableEffect(CameraEffectsFlags::AntiAliasing, cb.state == CheckBoxStateEnum::Checked);
		}
	}

	return false;
}

Entity *genInput(Entity *table, sint32 &childIndex, uint32 nameBase, const string &labelText, real rangeMin, real rangeMax, real step, real current)
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
		in.value = stringizer() + current.value;
		return e;
	}
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
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Ambient Occlusion";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(1);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = CheckBoxStateEnum::Checked;
		}
		Entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(LayoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInput(table, childIndex, 1, "World Radius:", 0.1, 3, 0.05, CameraEffects().ssao.worldRadius);
		childIndex += 2;
		genInput(table, childIndex, 1, "Strength:", 0, 3, 0.1, CameraEffects().ssao.strength);
		genInput(table, childIndex, 1, "Bias:", -0.5, 0.5, 0.01, CameraEffects().ssao.bias);
		genInput(table, childIndex, 1, "Power:", 0.1, 2, 0.02, CameraEffects().ssao.power);
		{
			Entity *e = genInput(table, childIndex, 1, "Samples:", 0, 0, 0, 0);
			CAGE_COMPONENT_GUI(Input, in, e);
			in.type = InputTypeEnum::Integer;
			in.min.i = 1;
			in.max.i = 128;
			in.step.i = 1;
			in.value = stringizer() + CameraEffects().ssao.samplesCount;
		}
		{
			Entity *e = genInput(table, childIndex, 1, "Blur Passes:", 0, 0, 0, 0);
			CAGE_COMPONENT_GUI(Input, in, e);
			in.type = InputTypeEnum::Integer;
			in.min.i = 0;
			in.max.i = 10;
			in.step.i = 1;
			in.value = stringizer() + CameraEffects().ssao.blurPasses;
		}
	}

	{ // motion blur
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Motion Blur";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(11);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = CheckBoxStateEnum::Checked;
		}
	}

	{ // bloom
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = 3;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Bloom";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(21);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = CheckBoxStateEnum::Checked;
		}
		Entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(LayoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInput(table, childIndex, 21, "Threshold:", 0, 5, 0.01, CameraEffects().bloom.threshold);
		childIndex += 2;
		{
			Entity *e = genInput(table, childIndex, 21, "Blur Passes:", 0, 0, 0, 0);
			CAGE_COMPONENT_GUI(Input, in, e);
			in.type = InputTypeEnum::Integer;
			in.min.i = 1;
			in.max.i = 10;
			in.step.i = 1;
			in.value = stringizer() + CameraEffects().bloom.blurPasses;
		}
	}

	{ // eye adaptation
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = 4;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Eye Adaptation";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(31);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = CheckBoxStateEnum::Unchecked;
		}
	}

	{ // tone mapping
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = 5;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Tone Mapping";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(41);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = CheckBoxStateEnum::Checked;
		}
		Entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(LayoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInput(table, childIndex, 41, "Shoulder Strength:", 0, 1, 0.02, CameraEffects().tonemap.shoulderStrength);
		genInput(table, childIndex, 41, "Linear Strength:", 0, 1, 0.02, CameraEffects().tonemap.linearStrength);
		genInput(table, childIndex, 41, "Linear Angle:", 0, 1, 0.02, CameraEffects().tonemap.linearAngle);
		genInput(table, childIndex, 41, "Toe Strength:", 0, 1, 0.02, CameraEffects().tonemap.toeStrength);
		genInput(table, childIndex, 41, "Toe Numerator:", 0, 1, 0.02, CameraEffects().tonemap.toeNumerator);
		genInput(table, childIndex, 41, "Toe Denominator:", 0, 1, 0.02, CameraEffects().tonemap.toeDenominator);
		genInput(table, childIndex, 41, "White:", 0, 100, 1, CameraEffects().tonemap.white);
	}

	{ // gamma
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = 6;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Gamma";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(51);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = CheckBoxStateEnum::Checked;
		}
		Entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(LayoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInput(table, childIndex, 51, "Gamma:", 1, 5, 0.1, CameraEffects().gamma);
	}

	{ // antialiasing
		Entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = 7;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Antialiasing";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(61);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = CheckBoxStateEnum::Checked;
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
			c.effects = CameraEffectsFlags::CombinedPass;
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
		Holder<EngineProfiling> engineProfiling = newEngineProfiling();

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
