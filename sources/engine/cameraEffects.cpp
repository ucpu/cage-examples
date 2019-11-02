
#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-engine/core.h>
#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/fpsCamera.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;

static const uint32 assetsName = hashString("cage-tests/camera-effects/effects.pack");

bool windowClose()
{
	engineStop();
	return false;
}

void enableEffect(cameraEffectsFlags effect, bool enable)
{
	entity *camera = entities()->get(1);
	CAGE_COMPONENT_ENGINE(camera, cam, camera);
	if (enable)
		cam.effects = cam.effects | effect;
	else
		cam.effects = cam.effects & ~effect;
}

bool update()
{
	entityManager *ents = gui()->entities();

	entity *camera = entities()->get(1);
	CAGE_COMPONENT_ENGINE(camera, cam, camera);

	{ // ambient occlusion
		{ // enable
			entity *e = ents->get(1);
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::AmbientOcclusion, cb.state == checkBoxStateEnum::Checked);
		}
		{ // world radius
			entity *e = ents->get(2);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.ssao.worldRadius = in.value.toFloat();
		}
		{ // strength
			entity *e = ents->get(4);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.ssao.strength = in.value.toFloat();
		}
		{ // bias
			entity *e = ents->get(5);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.ssao.bias = in.value.toFloat();
		}
		{ // power
			entity *e = ents->get(6);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.ssao.power = in.value.toFloat();
		}
		{ // samples count
			entity *e = ents->get(7);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.ssao.samplesCount = in.value.toUint32();
		}
	}

	{ // motion blur
		{ // enable
			entity *e = ents->get(11);
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::MotionBlur, cb.state == checkBoxStateEnum::Checked);
		}
	}

	{ // bloom
		{ // enable
			entity *e = ents->get(21);
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::Bloom, cb.state == checkBoxStateEnum::Checked);
		}
		{ // threshold
			entity *e = ents->get(22);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.bloom.threshold = in.value.toFloat();
		}
		{ // blur passes
			entity *e = ents->get(24);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.bloom.blurPasses = in.value.toUint32();
		}
	}

	{ // eye adaptation
		{ // enable
			entity *e = ents->get(31);
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::EyeAdaptation, cb.state == checkBoxStateEnum::Checked);
		}
	}

	{ // tone mapping
		{ // enable
			entity *e = ents->get(41);
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::ToneMapping, cb.state == checkBoxStateEnum::Checked);
		}
		{ // shoulderStrength
			entity *e = ents->get(42);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.tonemap.shoulderStrength = in.value.toFloat();
		}
		{ // linearStrength
			entity *e = ents->get(43);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.tonemap.linearStrength = in.value.toFloat();
		}
		{ // linearAngle
			entity *e = ents->get(44);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.tonemap.linearAngle = in.value.toFloat();
		}
		{ // toeStrength
			entity *e = ents->get(45);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.tonemap.toeStrength = in.value.toFloat();
		}
		{ // toeNumerator
			entity *e = ents->get(46);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.tonemap.toeNumerator = in.value.toFloat();
		}
		{ // toeDenominator
			entity *e = ents->get(47);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.tonemap.toeDenominator = in.value.toFloat();
		}
		{ // white
			entity *e = ents->get(48);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.tonemap.white = in.value.toFloat();
		}
	}

	{ // gamma
		{ // enable
			entity *e = ents->get(51);
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::GammaCorrection, cb.state == checkBoxStateEnum::Checked);
		}
		{ // gamma
			entity *e = ents->get(52);
			CAGE_COMPONENT_GUI(input, in, e);
			if (in.valid)
				cam.gamma = in.value.toFloat();
		}
	}

	{ // antialiasing
		{ // enable
			entity *e = ents->get(61);
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::AntiAliasing, cb.state == checkBoxStateEnum::Checked);
		}
	}

	return false;
}

entity *genInput(entity *table, sint32 &childIndex, uint32 nameBase, const string &labelText, real rangeMin, real rangeMax, real step, real current)
{
	entityManager *ents = gui()->entities();
	{
		entity *e = ents->createUnique();
		CAGE_COMPONENT_GUI(parent, p, e);
		p.parent = table->name();
		p.order = childIndex++;
		CAGE_COMPONENT_GUI(label, label, e);
		CAGE_COMPONENT_GUI(text, t, e);
		t.value = labelText;
	}
	{
		entity *e = ents->create(nameBase + childIndex / 2);
		CAGE_COMPONENT_GUI(parent, p, e);
		p.parent = table->name();
		p.order = childIndex++;
		CAGE_COMPONENT_GUI(input, in, e);
		in.type = inputTypeEnum::Real;
		in.min.f = rangeMin;
		in.max.f = rangeMax;
		in.step.f = step;
		in.value = current;
		return e;
	}
}

void initializeGui()
{
	entityManager *ents = gui()->entities();
	entity *layout = ents->createUnique();
	{ // layout
		CAGE_COMPONENT_GUI(scrollbars, sc, layout);
		CAGE_COMPONENT_GUI(layoutLine, l, layout);
		l.vertical = true;
	}

	{ // ambient occlusion
		entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, p, panel);
			p.parent = layout->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(spoiler, c, panel);
			CAGE_COMPONENT_GUI(text, t, panel);
			t.value = "Ambient Occlusion";
			CAGE_COMPONENT_GUI(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entity *e = ents->create(1);
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Checked;
		}
		entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(layoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInput(table, childIndex, 1, "World Radius:", 0.2, 5, 0.05, cameraEffects().ssao.worldRadius);
		childIndex += 2;
		genInput(table, childIndex, 1, "Strength:", 0.2, 5, 0.2, cameraEffects().ssao.strength);
		genInput(table, childIndex, 1, "Bias:", 0, 1, 0.01, cameraEffects().ssao.bias);
		genInput(table, childIndex, 1, "Power:", 0.1, 1, 0.02, cameraEffects().ssao.power);
		{
			entity *e = genInput(table, childIndex, 1, "Samples:", 0, 0, 0, 0);
			CAGE_COMPONENT_GUI(input, in, e);
			in.type = inputTypeEnum::Integer;
			in.min.i = 1;
			in.max.i = 128;
			in.step.i = 1;
			in.value = cameraEffects().ssao.samplesCount;
		}
	}

	{ // motion blur
		entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, p, panel);
			p.parent = layout->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(spoiler, c, panel);
			CAGE_COMPONENT_GUI(text, t, panel);
			t.value = "Motion Blur";
			CAGE_COMPONENT_GUI(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entity *e = ents->create(11);
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Checked;
		}
	}

	{ // bloom
		entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, p, panel);
			p.parent = layout->name();
			p.order = 3;
			CAGE_COMPONENT_GUI(spoiler, c, panel);
			CAGE_COMPONENT_GUI(text, t, panel);
			t.value = "Bloom";
			CAGE_COMPONENT_GUI(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entity *e = ents->create(21);
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Checked;
		}
		entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(layoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInput(table, childIndex, 21, "Threshold:", 0, 5, 0.01, cameraEffects().bloom.threshold);
		childIndex += 2;
		{
			entity *e = genInput(table, childIndex, 21, "Blur Passes:", 0, 0, 0, 0);
			CAGE_COMPONENT_GUI(input, in, e);
			in.type = inputTypeEnum::Integer;
			in.min.i = 0;
			in.max.i = 10;
			in.step.i = 1;
			in.value = cameraEffects().bloom.blurPasses;
		}
	}

	{ // eye adaptation
		entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, p, panel);
			p.parent = layout->name();
			p.order = 4;
			CAGE_COMPONENT_GUI(spoiler, c, panel);
			CAGE_COMPONENT_GUI(text, t, panel);
			t.value = "Eye Adaptation";
			CAGE_COMPONENT_GUI(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entity *e = ents->create(31);
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Unchecked;
		}
	}

	{ // tone mapping
		entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, p, panel);
			p.parent = layout->name();
			p.order = 5;
			CAGE_COMPONENT_GUI(spoiler, c, panel);
			CAGE_COMPONENT_GUI(text, t, panel);
			t.value = "Tone Mapping";
			CAGE_COMPONENT_GUI(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entity *e = ents->create(41);
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Checked;
		}
		entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(layoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInput(table, childIndex, 41, "Shoulder Strength:", 0, 1, 0.02, cameraEffects().tonemap.shoulderStrength);
		genInput(table, childIndex, 41, "Linear Strength:", 0, 1, 0.02, cameraEffects().tonemap.linearStrength);
		genInput(table, childIndex, 41, "Linear Angle:", 0, 1, 0.02, cameraEffects().tonemap.linearAngle);
		genInput(table, childIndex, 41, "Toe Strength:", 0, 1, 0.02, cameraEffects().tonemap.toeStrength);
		genInput(table, childIndex, 41, "Toe Numerator:", 0, 1, 0.02, cameraEffects().tonemap.toeNumerator);
		genInput(table, childIndex, 41, "Toe Denominator:", 0, 1, 0.02, cameraEffects().tonemap.toeDenominator);
		genInput(table, childIndex, 41, "White:", 0, 100, 1, cameraEffects().tonemap.white);
	}

	{ // gamma
		entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, p, panel);
			p.parent = layout->name();
			p.order = 6;
			CAGE_COMPONENT_GUI(spoiler, c, panel);
			CAGE_COMPONENT_GUI(text, t, panel);
			t.value = "Gamma";
			CAGE_COMPONENT_GUI(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entity *e = ents->create(51);
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Checked;
		}
		entity *table = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(layoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInput(table, childIndex, 51, "Gamma:", 1, 5, 0.1, cameraEffects().gamma);
	}

	{ // antialiasing
		entity *panel = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, p, panel);
			p.parent = layout->name();
			p.order = 7;
			CAGE_COMPONENT_GUI(spoiler, c, panel);
			CAGE_COMPONENT_GUI(text, t, panel);
			t.value = "Antialiasing";
			CAGE_COMPONENT_GUI(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entity *e = ents->create(61);
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "Enabled";
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Checked;
		}
	}
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		holder<logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, window()->events.windowClose);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		window()->title("camera effects");
		window()->setMaximized();
		initializeGui();

		// entities
		entityManager *ents = entities();
		{ // camera
			entity *e = ents->create(1);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(11.5, 1, -1);
			t.orientation = quat(degs(10), degs(110), degs());
			CAGE_COMPONENT_ENGINE(camera, c, e);
			c.ambientLight = vec3(0.02);
			c.near = 0.1;
			c.far = 100;
			c.effects = cameraEffectsFlags::CombinedPass;
		}
		{ // sun
			entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(0, 5, 0);
			t.orientation = quat(degs(-75), degs(-120), degs());
			CAGE_COMPONENT_ENGINE(light, l, e);
			l.lightType = lightTypeEnum::Directional;
			l.color = vec3(3);
			CAGE_COMPONENT_ENGINE(shadowmap, s, e);
			s.resolution = 2048;
			s.worldSize = vec3(30);
		}
		{ // floor
			entity *e = ents->create(10);
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("scenes/common/ground.obj");
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(0, -1.264425, 0);
		}
		{ // sponza
			entity *e = ents->create(11);
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("scenes/mcguire/crytek/sponza.object");
			CAGE_COMPONENT_ENGINE(transform, t, e);
		}

		holder<fpsCamera> fpsCamera = newFpsCamera(ents->get(1));
		fpsCamera->mouseButton = mouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 0.3;
		holder<engineProfiling> engineProfiling = newEngineProfiling();

		assets()->add(assetsName);
		engineStart();
		assets()->remove(assetsName);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
