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
	CameraComponent &cam = camera->value<CameraComponent>();
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
	CameraComponent &cam = camera->value<CameraComponent>();

	{ // ambient occlusion
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::AmbientOcclusion);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(CameraEffectsFlags::AmbientOcclusion, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // world radius
			Entity *e = ents->get(baseName + 1);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.ssao.worldRadius = toFloat(in.value);
		}
		{ // strength
			Entity *e = ents->get(baseName + 2);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.ssao.strength = toFloat(in.value);
		}
		{ // bias
			Entity *e = ents->get(baseName + 3);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.ssao.bias = toFloat(in.value);
		}
		{ // power
			Entity *e = ents->get(baseName + 4);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.ssao.power = toFloat(in.value);
		}
		{ // samples count
			Entity *e = ents->get(baseName + 5);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.ssao.samplesCount = toUint32(in.value);
		}
		{ // blur passes
			Entity *e = ents->get(baseName + 6);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.ssao.blurPasses = toUint32(in.value);
		}
	}

	{ // depth of field
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::DepthOfField);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(CameraEffectsFlags::DepthOfField, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // focus distance
			Entity *e = ents->get(baseName + 1);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.depthOfField.focusDistance = toFloat(in.value);
		}
		{ // focus radius
			Entity *e = ents->get(baseName + 2);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.depthOfField.focusRadius = toFloat(in.value);
		}
		{ // blend radius
			Entity *e = ents->get(baseName + 3);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.depthOfField.blendRadius = toFloat(in.value);
		}
		{ // blur passes
			Entity *e = ents->get(baseName + 4);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.depthOfField.blurPasses = toUint32(in.value);
		}
	}

	{ // motion blur
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::MotionBlur);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(CameraEffectsFlags::MotionBlur, cb.state == CheckBoxStateEnum::Checked);
		}
	}

	{ // bloom
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::Bloom);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(CameraEffectsFlags::Bloom, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // threshold
			Entity *e = ents->get(baseName + 1);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.bloom.threshold = toFloat(in.value);
		}
		{ // blur passes
			Entity *e = ents->get(baseName + 2);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.bloom.blurPasses = toUint32(in.value);
		}
	}

	{ // eye adaptation
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::EyeAdaptation);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(CameraEffectsFlags::EyeAdaptation, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // key
			Entity *e = ents->get(baseName + 1);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.eyeAdaptation.key = toFloat(in.value);
		}
		{ // strength
			Entity *e = ents->get(baseName + 2);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.eyeAdaptation.strength = toFloat(in.value);
		}
		{ // darkerSpeed
			Entity *e = ents->get(baseName + 3);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.eyeAdaptation.darkerSpeed = toFloat(in.value);
		}
		{ // lighterSpeed
			Entity *e = ents->get(baseName + 4);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.eyeAdaptation.lighterSpeed = toFloat(in.value);
		}
	}

	{ // tone mapping
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::ToneMapping);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(CameraEffectsFlags::ToneMapping, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // shoulderStrength
			Entity *e = ents->get(baseName + 1);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.tonemap.shoulderStrength = toFloat(in.value);
		}
		{ // linearStrength
			Entity *e = ents->get(baseName + 2);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.tonemap.linearStrength = toFloat(in.value);
		}
		{ // linearAngle
			Entity *e = ents->get(baseName + 3);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.tonemap.linearAngle = toFloat(in.value);
		}
		{ // toeStrength
			Entity *e = ents->get(baseName + 4);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.tonemap.toeStrength = toFloat(in.value);
		}
		{ // toeNumerator
			Entity *e = ents->get(baseName + 5);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.tonemap.toeNumerator = toFloat(in.value);
		}
		{ // toeDenominator
			Entity *e = ents->get(baseName + 6);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.tonemap.toeDenominator = toFloat(in.value);
		}
		{ // white
			Entity *e = ents->get(baseName + 7);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.tonemap.white = toFloat(in.value);
		}
	}

	{ // gamma
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::GammaCorrection);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(CameraEffectsFlags::GammaCorrection, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // gamma
			Entity *e = ents->get(baseName + 1);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.gamma = toFloat(in.value);
		}
	}

	{ // antialiasing
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::AntiAliasing);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(CameraEffectsFlags::AntiAliasing, cb.state == CheckBoxStateEnum::Checked);
		}
	}

	return false;
}

Entity *genInputFloat(Entity *table, sint32 &childIndex, uint32 nameBase, const String &labelText, Real rangeMin, Real rangeMax, Real step, Real current)
{
	EntityManager *ents = engineGui()->entities();
	{
		Entity *e = ents->createUnique();
		GuiParentComponent &p = e->value<GuiParentComponent>();
		p.parent = table->name();
		p.order = childIndex++;
		GuiLabelComponent &label = e->value<GuiLabelComponent>();
		GuiTextComponent &t = e->value<GuiTextComponent>();
		t.value = labelText;
	}
	{
		Entity *e = ents->create(nameBase + childIndex / 2);
		GuiParentComponent &p = e->value<GuiParentComponent>();
		p.parent = table->name();
		p.order = childIndex++;
		GuiInputComponent &in = e->value<GuiInputComponent>();
		in.type = InputTypeEnum::Real;
		in.min.f = rangeMin;
		in.max.f = rangeMax;
		in.step.f = step;
		in.value = Stringizer() + current;
		return e;
	}
}

Entity *genInputInt(Entity *table, sint32 &childIndex, uint32 nameBase, const String &labelText, sint32 rangeMin, sint32 rangeMax, sint32 step, sint32 current)
{
	Entity *e = genInputFloat(table, childIndex, nameBase, labelText, 0, 0, 0, 0);
	GuiInputComponent &in = e->value<GuiInputComponent>();
	in.type = InputTypeEnum::Integer;
	in.min.i = rangeMin;
	in.max.i = rangeMax;
	in.step.i = step;
	in.value = Stringizer() + current;
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
		GuiScrollbarsComponent &sc = layout->value<GuiScrollbarsComponent>();
		GuiLayoutLineComponent &l = layout->value<GuiLayoutLineComponent>();
		l.vertical = true;
	}

	{ // ambient occlusion
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::AmbientOcclusion);
		Entity *panel = ents->createUnique();
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			GuiSpoilerComponent &c = panel->value<GuiSpoilerComponent>();
			GuiTextComponent &t = panel->value<GuiTextComponent>();
			t.value = "Ambient Occlusion";
			GuiLayoutLineComponent &l = panel->value<GuiLayoutLineComponent>();
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "Enabled";
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			cb.state = genEnabled(CameraEffectsFlags::AmbientOcclusion);
		}
		Entity *table = ents->createUnique();
		{
			GuiParentComponent &p = table->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			GuiLayoutTableComponent &t = table->value<GuiLayoutTableComponent>();
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
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			GuiSpoilerComponent &c = panel->value<GuiSpoilerComponent>();
			GuiTextComponent &t = panel->value<GuiTextComponent>();
			t.value = "Depth of field";
			GuiLayoutLineComponent &l = panel->value<GuiLayoutLineComponent>();
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "Enabled";
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			cb.state = genEnabled(CameraEffectsFlags::DepthOfField);
		}
		Entity *table = ents->createUnique();
		{
			GuiParentComponent &p = table->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			GuiLayoutTableComponent &t = table->value<GuiLayoutTableComponent>();
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
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			GuiSpoilerComponent &c = panel->value<GuiSpoilerComponent>();
			GuiTextComponent &t = panel->value<GuiTextComponent>();
			t.value = "Motion Blur";
			GuiLayoutLineComponent &l = panel->value<GuiLayoutLineComponent>();
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "Enabled";
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			cb.state = genEnabled(CameraEffectsFlags::MotionBlur);
		}
	}

	{ // bloom
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::Bloom);
		Entity *panel = ents->createUnique();
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			GuiSpoilerComponent &c = panel->value<GuiSpoilerComponent>();
			GuiTextComponent &t = panel->value<GuiTextComponent>();
			t.value = "Bloom";
			GuiLayoutLineComponent &l = panel->value<GuiLayoutLineComponent>();
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "Enabled";
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			cb.state = genEnabled(CameraEffectsFlags::Bloom);
		}
		Entity *table = ents->createUnique();
		{
			GuiParentComponent &p = table->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			GuiLayoutTableComponent &t = table->value<GuiLayoutTableComponent>();
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "Threshold:", 0, 5, 0.01, CameraEffects().bloom.threshold);
		genInputInt(table, childIndex, baseName, "Blur Passes:", 1, 10, 1, CameraEffects().bloom.blurPasses);
	}

	{ // eye adaptation
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::EyeAdaptation);
		Entity *panel = ents->createUnique();
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			GuiSpoilerComponent &c = panel->value<GuiSpoilerComponent>();
			GuiTextComponent &t = panel->value<GuiTextComponent>();
			t.value = "Eye Adaptation";
			GuiLayoutLineComponent &l = panel->value<GuiLayoutLineComponent>();
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "Enabled";
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			cb.state = genEnabled(CameraEffectsFlags::EyeAdaptation);
		}
		Entity *table = ents->createUnique();
		{
			GuiParentComponent &p = table->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			GuiLayoutTableComponent &t = table->value<GuiLayoutTableComponent>();
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
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			GuiSpoilerComponent &c = panel->value<GuiSpoilerComponent>();
			GuiTextComponent &t = panel->value<GuiTextComponent>();
			t.value = "Tone Mapping";
			GuiLayoutLineComponent &l = panel->value<GuiLayoutLineComponent>();
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "Enabled";
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			cb.state = genEnabled(CameraEffectsFlags::ToneMapping);
		}
		Entity *table = ents->createUnique();
		{
			GuiParentComponent &p = table->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			GuiLayoutTableComponent &t = table->value<GuiLayoutTableComponent>();
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
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			GuiSpoilerComponent &c = panel->value<GuiSpoilerComponent>();
			GuiTextComponent &t = panel->value<GuiTextComponent>();
			t.value = "Gamma";
			GuiLayoutLineComponent &l = panel->value<GuiLayoutLineComponent>();
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "Enabled";
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			cb.state = genEnabled(CameraEffectsFlags::GammaCorrection);
		}
		Entity *table = ents->createUnique();
		{
			GuiParentComponent &p = table->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			GuiLayoutTableComponent &t = table->value<GuiLayoutTableComponent>();
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "Gamma:", 1, 5, 0.1, CameraEffects().gamma);
	}

	{ // antialiasing
		constexpr sint32 baseName = genBaseName(CameraEffectsFlags::AntiAliasing);
		Entity *panel = ents->createUnique();
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			GuiSpoilerComponent &c = panel->value<GuiSpoilerComponent>();
			GuiTextComponent &t = panel->value<GuiTextComponent>();
			t.value = "Antialiasing";
			GuiLayoutLineComponent &l = panel->value<GuiLayoutLineComponent>();
			l.vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "Enabled";
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
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
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(11.5, 1, -1);
			t.orientation = Quat(Degs(10), Degs(110), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.01;
			c.ambientDirectionalColor = Vec3(1);
			c.ambientDirectionalIntensity = 0.03;
			c.near = 0.1;
			c.far = 100;
			c.effects = CameraEffectsFlags::Default;
		}
		{ // sun
			Entity *e = ents->create(2);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 5, 0);
			t.orientation = Quat(Degs(-75), Degs(-120), Degs());
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 3;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
			s.worldSize = Vec3(30);
		}
		{ // floor
			Entity *e = ents->create(10);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("scenes/common/ground.obj");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, -1.264425, 0);
		}
		{ // sponza
			Entity *e = ents->create(11);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("scenes/mcguire/crytek/sponza.object");
			TransformComponent &t = e->value<TransformComponent>();
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
