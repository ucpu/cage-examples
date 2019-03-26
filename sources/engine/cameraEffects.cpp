
#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/assets.h>
#include <cage-core/hashString.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/gui.h>
#include <cage-client/engine.h>
#include <cage-client/engineProfiling.h>
#include <cage-client/cameraController.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;

static const uint32 assetsName = hashString("cage-tests/camera-effects/effects.pack");

bool windowClose()
{
	engineStop();
	return false;
}

void enableEffect(cameraEffectsFlags effect, bool enable)
{
	entityClass *camera = entities()->get(1);
	ENGINE_GET_COMPONENT(camera, cam, camera);
	if (enable)
		cam.effects = cam.effects | effect;
	else
		cam.effects = cam.effects & ~effect;
}

bool update()
{
	entityManagerClass *ents = gui()->entities();

	entityClass *camera = entities()->get(1);
	ENGINE_GET_COMPONENT(camera, cam, camera);

	{ // ambient occlusion
		{ // enable
			entityClass *e = ents->get(1);
			GUI_GET_COMPONENT(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::AmbientOcclusion, cb.state == checkBoxStateEnum::Checked);
		}
		{ // world radius
			entityClass *e = ents->get(2);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.ssao.worldRadius = in.value.toFloat();
		}
		{ // strength
			entityClass *e = ents->get(4);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.ssao.strength = in.value.toFloat();
		}
		{ // bias
			entityClass *e = ents->get(5);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.ssao.bias = in.value.toFloat();
		}
		{ // power
			entityClass *e = ents->get(6);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.ssao.power = in.value.toFloat();
		}
		{ // samples count
			entityClass *e = ents->get(7);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.ssao.samplesCount = in.value.toUint32();
		}
	}

	{ // motion blur
		{ // enable
			entityClass *e = ents->get(11);
			GUI_GET_COMPONENT(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::MotionBlur, cb.state == checkBoxStateEnum::Checked);
		}
	}

	{ // bloom
		{ // enable
			entityClass *e = ents->get(21);
			GUI_GET_COMPONENT(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::Bloom, cb.state == checkBoxStateEnum::Checked);
		}
		{ // threshold
			entityClass *e = ents->get(22);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.bloom.threshold = in.value.toFloat();
		}
		{ // blur passes
			entityClass *e = ents->get(24);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.bloom.blurPasses = in.value.toUint32();
		}
	}

	{ // eye adaptation
		{ // enable
			entityClass *e = ents->get(31);
			GUI_GET_COMPONENT(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::EyeAdaptation, cb.state == checkBoxStateEnum::Checked);
		}
	}

	{ // tone mapping
		{ // enable
			entityClass *e = ents->get(41);
			GUI_GET_COMPONENT(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::ToneMapping, cb.state == checkBoxStateEnum::Checked);
		}
		{ // shoulderStrength
			entityClass *e = ents->get(42);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.tonemap.shoulderStrength = in.value.toFloat();
		}
		{ // linearStrength
			entityClass *e = ents->get(43);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.tonemap.linearStrength = in.value.toFloat();
		}
		{ // linearAngle
			entityClass *e = ents->get(44);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.tonemap.linearAngle = in.value.toFloat();
		}
		{ // toeStrength
			entityClass *e = ents->get(45);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.tonemap.toeStrength = in.value.toFloat();
		}
		{ // toeNumerator
			entityClass *e = ents->get(46);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.tonemap.toeNumerator = in.value.toFloat();
		}
		{ // toeDenominator
			entityClass *e = ents->get(47);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.tonemap.toeDenominator = in.value.toFloat();
		}
		{ // white
			entityClass *e = ents->get(48);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.tonemap.white = in.value.toFloat();
		}
	}

	{ // gamma
		{ // enable
			entityClass *e = ents->get(51);
			GUI_GET_COMPONENT(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::GammaCorrection, cb.state == checkBoxStateEnum::Checked);
		}
		{ // gamma
			entityClass *e = ents->get(52);
			GUI_GET_COMPONENT(input, in, e);
			if (in.valid)
				cam.gamma = in.value.toFloat();
		}
	}

	{ // antialiasing
		{ // enable
			entityClass *e = ents->get(61);
			GUI_GET_COMPONENT(checkBox, cb, e);
			enableEffect(cameraEffectsFlags::AntiAliasing, cb.state == checkBoxStateEnum::Checked);
		}
	}

	return false;
}

entityClass *genInput(entityClass *table, sint32 &childIndex, uint32 nameBase, const string &labelText, real rangeMin, real rangeMax, real step, real current)
{
	entityManagerClass *ents = gui()->entities();
	{
		entityClass *e = ents->createUnique();
		GUI_GET_COMPONENT(parent, p, e);
		p.parent = table->name();
		p.order = childIndex++;
		GUI_GET_COMPONENT(label, label, e);
		GUI_GET_COMPONENT(text, t, e);
		t.value = labelText;
	}
	{
		entityClass *e = ents->create(nameBase + childIndex / 2);
		GUI_GET_COMPONENT(parent, p, e);
		p.parent = table->name();
		p.order = childIndex++;
		GUI_GET_COMPONENT(input, in, e);
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
	entityManagerClass *ents = gui()->entities();
	entityClass *layout = ents->createUnique();
	{ // layout
		GUI_GET_COMPONENT(scrollbars, sc, layout);
		GUI_GET_COMPONENT(layoutLine, l, layout);
		l.vertical = true;
	}

	{ // ambient occlusion
		entityClass *panel = ents->createUnique();
		{
			GUI_GET_COMPONENT(parent, p, panel);
			p.parent = layout->name();
			p.order = 1;
			GUI_GET_COMPONENT(spoiler, c, panel);
			GUI_GET_COMPONENT(text, t, panel);
			t.value = "Ambient Occlusion";
			GUI_GET_COMPONENT(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entityClass *e = ents->create(1);
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			GUI_GET_COMPONENT(text, t, e);
			t.value = "Enabled";
			GUI_GET_COMPONENT(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Checked;
		}
		entityClass *table = ents->createUnique();
		{
			GUI_GET_COMPONENT(parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			GUI_GET_COMPONENT(layoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInput(table, childIndex, 1, "World Radius:", 0.2, 5, 0.05, cameraEffectsStruct().ssao.worldRadius);
		childIndex += 2;
		genInput(table, childIndex, 1, "Strength:", 0.2, 5, 0.2, cameraEffectsStruct().ssao.strength);
		genInput(table, childIndex, 1, "Bias:", 0, 1, 0.01, cameraEffectsStruct().ssao.bias);
		genInput(table, childIndex, 1, "Power:", 0.1, 1, 0.02, cameraEffectsStruct().ssao.power);
		{
			entityClass *e = genInput(table, childIndex, 1, "Samples:", 0, 0, 0, 0);
			GUI_GET_COMPONENT(input, in, e);
			in.type = inputTypeEnum::Integer;
			in.min.i = 1;
			in.max.i = 128;
			in.step.i = 1;
			in.value = cameraEffectsStruct().ssao.samplesCount;
		}
	}

	{ // motion blur
		entityClass *panel = ents->createUnique();
		{
			GUI_GET_COMPONENT(parent, p, panel);
			p.parent = layout->name();
			p.order = 2;
			GUI_GET_COMPONENT(spoiler, c, panel);
			GUI_GET_COMPONENT(text, t, panel);
			t.value = "Motion Blur";
			GUI_GET_COMPONENT(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entityClass *e = ents->create(11);
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			GUI_GET_COMPONENT(text, t, e);
			t.value = "Enabled";
			GUI_GET_COMPONENT(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Checked;
		}
	}

	{ // bloom
		entityClass *panel = ents->createUnique();
		{
			GUI_GET_COMPONENT(parent, p, panel);
			p.parent = layout->name();
			p.order = 3;
			GUI_GET_COMPONENT(spoiler, c, panel);
			GUI_GET_COMPONENT(text, t, panel);
			t.value = "Bloom";
			GUI_GET_COMPONENT(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entityClass *e = ents->create(21);
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			GUI_GET_COMPONENT(text, t, e);
			t.value = "Enabled";
			GUI_GET_COMPONENT(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Checked;
		}
		entityClass *table = ents->createUnique();
		{
			GUI_GET_COMPONENT(parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			GUI_GET_COMPONENT(layoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInput(table, childIndex, 21, "Threshold:", 0, 5, 0.01, cameraEffectsStruct().bloom.threshold);
		childIndex += 2;
		{
			entityClass *e = genInput(table, childIndex, 21, "Blur Passes:", 0, 0, 0, 0);
			GUI_GET_COMPONENT(input, in, e);
			in.type = inputTypeEnum::Integer;
			in.min.i = 0;
			in.max.i = 10;
			in.step.i = 1;
			in.value = cameraEffectsStruct().bloom.blurPasses;
		}
	}

	{ // eye adaptation
		entityClass *panel = ents->createUnique();
		{
			GUI_GET_COMPONENT(parent, p, panel);
			p.parent = layout->name();
			p.order = 4;
			GUI_GET_COMPONENT(spoiler, c, panel);
			GUI_GET_COMPONENT(text, t, panel);
			t.value = "Eye Adaptation";
			GUI_GET_COMPONENT(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entityClass *e = ents->create(31);
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			GUI_GET_COMPONENT(text, t, e);
			t.value = "Enabled";
			GUI_GET_COMPONENT(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Unchecked;
		}
	}

	{ // tone mapping
		entityClass *panel = ents->createUnique();
		{
			GUI_GET_COMPONENT(parent, p, panel);
			p.parent = layout->name();
			p.order = 5;
			GUI_GET_COMPONENT(spoiler, c, panel);
			GUI_GET_COMPONENT(text, t, panel);
			t.value = "Tone Mapping";
			GUI_GET_COMPONENT(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entityClass *e = ents->create(41);
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			GUI_GET_COMPONENT(text, t, e);
			t.value = "Enabled";
			GUI_GET_COMPONENT(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Checked;
		}
		entityClass *table = ents->createUnique();
		{
			GUI_GET_COMPONENT(parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			GUI_GET_COMPONENT(layoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInput(table, childIndex, 41, "Shoulder Strength:", 0, 1, 0.02, cameraEffectsStruct().tonemap.shoulderStrength);
		genInput(table, childIndex, 41, "Linear Strength:", 0, 1, 0.02, cameraEffectsStruct().tonemap.linearStrength);
		genInput(table, childIndex, 41, "Linear Angle:", 0, 1, 0.02, cameraEffectsStruct().tonemap.linearAngle);
		genInput(table, childIndex, 41, "Toe Strength:", 0, 1, 0.02, cameraEffectsStruct().tonemap.toeStrength);
		genInput(table, childIndex, 41, "Toe Numerator:", 0, 1, 0.02, cameraEffectsStruct().tonemap.toeNumerator);
		genInput(table, childIndex, 41, "Toe Denominator:", 0, 1, 0.02, cameraEffectsStruct().tonemap.toeDenominator);
		genInput(table, childIndex, 41, "White:", 0, 100, 1, cameraEffectsStruct().tonemap.white);
	}

	{ // gamma
		entityClass *panel = ents->createUnique();
		{
			GUI_GET_COMPONENT(parent, p, panel);
			p.parent = layout->name();
			p.order = 6;
			GUI_GET_COMPONENT(spoiler, c, panel);
			GUI_GET_COMPONENT(text, t, panel);
			t.value = "Gamma";
			GUI_GET_COMPONENT(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entityClass *e = ents->create(51);
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			GUI_GET_COMPONENT(text, t, e);
			t.value = "Enabled";
			GUI_GET_COMPONENT(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Checked;
		}
		entityClass *table = ents->createUnique();
		{
			GUI_GET_COMPONENT(parent, p, table);
			p.parent = panel->name();
			p.order = 2;
			GUI_GET_COMPONENT(layoutTable, t, table);
		}
		sint32 childIndex = 1;
		genInput(table, childIndex, 51, "Gamma:", 1, 5, 0.1, cameraEffectsStruct().gamma);
	}

	{ // antialiasing
		entityClass *panel = ents->createUnique();
		{
			GUI_GET_COMPONENT(parent, p, panel);
			p.parent = layout->name();
			p.order = 7;
			GUI_GET_COMPONENT(spoiler, c, panel);
			GUI_GET_COMPONENT(text, t, panel);
			t.value = "Antialiasing";
			GUI_GET_COMPONENT(layoutLine, l, panel);
			l.vertical = true;
		}
		{ // enabled
			entityClass *e = ents->create(61);
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = panel->name();
			p.order = 1;
			GUI_GET_COMPONENT(text, t, e);
			t.value = "Enabled";
			GUI_GET_COMPONENT(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Checked;
		}
	}
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		holder<loggerClass> log1 = newLogger();
		log1->filter.bind<logFilterPolicyPass>();
		log1->format.bind<logFormatPolicyConsole>();
		log1->output.bind<logOutputPolicyStdOut>();

		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, window()->events.windowClose);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		window()->setWindowed();
		window()->windowedSize(pointStruct(800, 600));
		window()->title("camera effects");
		initializeGui();

		// entities
		entityManagerClass *ents = entities();
		{ // camera
			entityClass *e = ents->create(1);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(11.5, 1, -1);
			t.orientation = quat(degs(10), degs(110), degs());
			ENGINE_GET_COMPONENT(camera, c, e);
			c.ambientLight = vec3(0.02);
			c.near = 0.1;
			c.far = 100;
			c.effects = cameraEffectsFlags::CombinedPass;
		}
		{ // sun
			entityClass *e = ents->create(2);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(0, 5, 0);
			t.orientation = quat(degs(-75), degs(-120), degs());
			ENGINE_GET_COMPONENT(light, l, e);
			l.lightType = lightTypeEnum::Directional;
			l.color = vec3(3);
			ENGINE_GET_COMPONENT(shadowmap, s, e);
			s.resolution = 2048;
			s.worldSize = vec3(30);
		}
		{ // floor
			entityClass *e = ents->create(10);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("scenes/common/ground.obj");
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(0, -1.264425, 0);
		}
		{ // sponza
			entityClass *e = ents->create(11);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("scenes/mcguire/crytek/sponza.object");
			ENGINE_GET_COMPONENT(transform, t, e);
		}

		holder<cameraControllerClass> cameraController = newCameraController(ents->get(1));
		cameraController->mouseButton = mouseButtonsFlags::Left;
		cameraController->movementSpeed = 0.3;
		holder<engineProfilingClass> engineProfiling = newEngineProfiling();

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
