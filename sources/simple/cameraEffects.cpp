#include <cage-core/assetManager.h>
#include <cage-core/config.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/string.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>
#include <cage-engine/window.h>

#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;

constexpr uint32 assetsName = HashString("cage-tests/camera-effects/effects.pack");

void windowClose(InputWindow)
{
	engineStop();
}

void enableEffect(ScreenSpaceEffectsFlags effect, bool enable)
{
	Entity *camera = engineEntities()->get(1);
	ScreenSpaceEffectsComponent &eff = camera->value<ScreenSpaceEffectsComponent>();
	if (enable)
		eff.effects = eff.effects | effect;
	else
		eff.effects = eff.effects & ~effect;
}

constexpr sint32 genBaseName(ScreenSpaceEffectsFlags f_)
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

void update()
{
	EntityManager *ents = engineGuiEntities();

	Entity *camera = engineEntities()->get(1);
	ScreenSpaceEffectsComponent &eff = camera->value<ScreenSpaceEffectsComponent>();

	{ // ambient occlusion
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::AmbientOcclusion);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(ScreenSpaceEffectsFlags::AmbientOcclusion, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // world radius
			Entity *e = ents->get(baseName + 1);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.ssao.worldRadius = toFloat(in.value);
		}
		{ // strength
			Entity *e = ents->get(baseName + 2);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.ssao.strength = toFloat(in.value);
		}
		{ // bias
			Entity *e = ents->get(baseName + 3);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.ssao.bias = toFloat(in.value);
		}
		{ // power
			Entity *e = ents->get(baseName + 4);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.ssao.power = toFloat(in.value);
		}
		{ // samples count
			Entity *e = ents->get(baseName + 5);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.ssao.samplesCount = toUint32(in.value);
		}
		{ // blur passes
			Entity *e = ents->get(baseName + 6);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.ssao.blurPasses = toUint32(in.value);
		}
	}

	{ // depth of field
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::DepthOfField);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(ScreenSpaceEffectsFlags::DepthOfField, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // focus distance
			Entity *e = ents->get(baseName + 1);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.depthOfField.focusDistance = toFloat(in.value);
		}
		{ // focus radius
			Entity *e = ents->get(baseName + 2);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.depthOfField.focusRadius = toFloat(in.value);
		}
		{ // blend radius
			Entity *e = ents->get(baseName + 3);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.depthOfField.blendRadius = toFloat(in.value);
		}
		{ // blur passes
			Entity *e = ents->get(baseName + 4);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.depthOfField.blurPasses = toUint32(in.value);
		}
	}

	{ // bloom
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::Bloom);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(ScreenSpaceEffectsFlags::Bloom, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // threshold
			Entity *e = ents->get(baseName + 1);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.bloom.threshold = toFloat(in.value);
		}
		{ // blur passes
			Entity *e = ents->get(baseName + 2);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.bloom.blurPasses = toUint32(in.value);
		}
	}

	{ // eye adaptation
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::EyeAdaptation);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(ScreenSpaceEffectsFlags::EyeAdaptation, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // darkerSpeed
			Entity *e = ents->get(baseName + 1);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.eyeAdaptation.darkerSpeed = toFloat(in.value);
		}
		{ // lighterSpeed
			Entity *e = ents->get(baseName + 2);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.eyeAdaptation.lighterSpeed = toFloat(in.value);
		}
		{ // lowLogLum
			Entity *e = ents->get(baseName + 3);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.eyeAdaptation.lowLogLum = toFloat(in.value);
		}
		{ // highLogLum
			Entity *e = ents->get(baseName + 4);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.eyeAdaptation.highLogLum = toFloat(in.value);
		}
		{ // nightOffset
			Entity *e = ents->get(baseName + 5);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.eyeAdaptation.nightOffset = toFloat(in.value);
		}
		{ // nightDesaturate
			Entity *e = ents->get(baseName + 6);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.eyeAdaptation.nightDesaturate = toFloat(in.value);
		}
		{ // nightContrast
			Entity *e = ents->get(baseName + 7);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.eyeAdaptation.nightContrast = toFloat(in.value);
		}
		{ // key
			Entity *e = ents->get(baseName + 8);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.eyeAdaptation.key = toFloat(in.value);
		}
		{ // strength
			Entity *e = ents->get(baseName + 9);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.eyeAdaptation.strength = toFloat(in.value);
		}
	}

	{ // tone mapping
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::ToneMapping);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(ScreenSpaceEffectsFlags::ToneMapping, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // shoulderStrength
			Entity *e = ents->get(baseName + 1);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.tonemap.shoulderStrength = toFloat(in.value);
		}
		{ // linearStrength
			Entity *e = ents->get(baseName + 2);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.tonemap.linearStrength = toFloat(in.value);
		}
		{ // linearAngle
			Entity *e = ents->get(baseName + 3);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.tonemap.linearAngle = toFloat(in.value);
		}
		{ // toeStrength
			Entity *e = ents->get(baseName + 4);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.tonemap.toeStrength = toFloat(in.value);
		}
		{ // toeNumerator
			Entity *e = ents->get(baseName + 5);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.tonemap.toeNumerator = toFloat(in.value);
		}
		{ // toeDenominator
			Entity *e = ents->get(baseName + 6);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.tonemap.toeDenominator = toFloat(in.value);
		}
		{ // white
			Entity *e = ents->get(baseName + 7);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				eff.tonemap.white = toFloat(in.value);
		}
	}

	{ // gamma
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::GammaCorrection);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(ScreenSpaceEffectsFlags::GammaCorrection, cb.state == CheckBoxStateEnum::Checked);
		}
		{ // gamma
			ConfigFloat confRenderGamma("cage/graphics/gamma");
			Entity *e = ents->get(baseName + 1);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				confRenderGamma = toFloat(in.value);
		}
	}

	{ // antialiasing
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::AntiAliasing);
		{ // enable
			Entity *e = ents->get(baseName);
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			enableEffect(ScreenSpaceEffectsFlags::AntiAliasing, cb.state == CheckBoxStateEnum::Checked);
		}
	}

	{ // lights intensities
		constexpr sint32 baseName = 2000;
		{ // sun
			Entity *e = ents->get(baseName + 1);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				engineEntities()->get(2)->value<LightComponent>().intensity = toFloat(in.value);
		}
		CameraComponent &cam = camera->value<CameraComponent>();
		{ // ambient
			Entity *e = ents->get(baseName + 2);
			GuiInputComponent &in = e->value<GuiInputComponent>();
			if (in.valid)
				cam.ambientIntensity = toFloat(in.value);
		}
	}
}

Entity *genInputFloat(Entity *table, sint32 &childIndex, uint32 nameBase, const String &labelText, Real rangeMin, Real rangeMax, Real step, Real current)
{
	EntityManager *ents = engineGuiEntities();
	{
		Entity *e = ents->createUnique();
		GuiParentComponent &p = e->value<GuiParentComponent>();
		p.parent = table->name();
		p.order = childIndex++;
		e->value<GuiLabelComponent>();
		e->value<GuiTextComponent>().value = labelText;
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

void initializeGui()
{
	EntityManager *ents = engineGuiEntities();
	Entity *layout = ents->createUnique();
	{ // layout
		layout->value<GuiLayoutScrollbarsComponent>();
		layout->value<GuiLayoutAlignmentComponent>().alignment = Vec2(0);
		layout->value<GuiLayoutLineComponent>().vertical = true;
	}

	{ // ambient occlusion
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::AmbientOcclusion);
		Entity *panel = ents->createUnique();
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			panel->value<GuiSpoilerComponent>();
			panel->value<GuiTextComponent>().value = "Ambient Occlusion";
			panel->value<GuiLayoutLineComponent>().vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			e->value<GuiTextComponent>().value = "Enabled";
			e->value<GuiCheckBoxComponent>().state = CheckBoxStateEnum::Checked;
		}
		Entity *table = ents->createUnique();
		{
			GuiParentComponent &p = table->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			table->value<GuiLayoutTableComponent>();
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "World Radius:", 0.1, 3, 0.05, ScreenSpaceEffectsComponent().ssao.worldRadius);
		genInputFloat(table, childIndex, baseName, "Strength:", 0, 3, 0.1, ScreenSpaceEffectsComponent().ssao.strength);
		genInputFloat(table, childIndex, baseName, "Bias:", -0.5, 0.5, 0.01, ScreenSpaceEffectsComponent().ssao.bias);
		genInputFloat(table, childIndex, baseName, "Power:", 0.1, 2, 0.02, ScreenSpaceEffectsComponent().ssao.power);
		genInputInt(table, childIndex, baseName, "Samples:", 1, 128, 1, ScreenSpaceEffectsComponent().ssao.samplesCount);
		genInputInt(table, childIndex, baseName, "Blur Passes:", 0, 10, 1, ScreenSpaceEffectsComponent().ssao.blurPasses);
	}

	{ // depth of field
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::DepthOfField);
		Entity *panel = ents->createUnique();
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			panel->value<GuiSpoilerComponent>();
			panel->value<GuiTextComponent>().value = "Depth of field";
			panel->value<GuiLayoutLineComponent>().vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			e->value<GuiTextComponent>().value = "Enabled";
			e->value<GuiCheckBoxComponent>().state = CheckBoxStateEnum::Checked;
		}
		Entity *table = ents->createUnique();
		{
			GuiParentComponent &p = table->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			table->value<GuiLayoutTableComponent>();
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "Focus distance:", 0, 20, 0.5, ScreenSpaceEffectsComponent().depthOfField.focusDistance);
		genInputFloat(table, childIndex, baseName, "Focus radius:", 0, 20, 0.5, ScreenSpaceEffectsComponent().depthOfField.focusRadius);
		genInputFloat(table, childIndex, baseName, "Blend radius:", 0, 20, 0.5, ScreenSpaceEffectsComponent().depthOfField.blendRadius);
		genInputInt(table, childIndex, baseName, "Blur passes:", 0, 10, 1, ScreenSpaceEffectsComponent().depthOfField.blurPasses);
	}

	{ // bloom
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::Bloom);
		Entity *panel = ents->createUnique();
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			panel->value<GuiSpoilerComponent>();
			panel->value<GuiTextComponent>().value = "Bloom";
			panel->value<GuiLayoutLineComponent>().vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			e->value<GuiTextComponent>().value = "Enabled";
			e->value<GuiCheckBoxComponent>().state = CheckBoxStateEnum::Checked;
		}
		Entity *table = ents->createUnique();
		{
			GuiParentComponent &p = table->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			GuiLayoutTableComponent &t = table->value<GuiLayoutTableComponent>();
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "Threshold:", 0, 5, 0.01, ScreenSpaceEffectsComponent().bloom.threshold);
		genInputInt(table, childIndex, baseName, "Blur Passes:", 1, 10, 1, ScreenSpaceEffectsComponent().bloom.blurPasses);
	}

	{ // eye adaptation
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::EyeAdaptation);
		Entity *panel = ents->createUnique();
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			panel->value<GuiSpoilerComponent>();
			panel->value<GuiTextComponent>().value = "Eye adaptation";
			panel->value<GuiLayoutLineComponent>().vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			e->value<GuiTextComponent>().value = "Enabled";
			e->value<GuiCheckBoxComponent>().state = CheckBoxStateEnum::Checked;
		}
		Entity *table = ents->createUnique();
		{
			GuiParentComponent &p = table->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			GuiLayoutTableComponent &t = table->value<GuiLayoutTableComponent>();
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "Darker speed:", 0, 10, 0.02, ScreenSpaceEffectsComponent().eyeAdaptation.darkerSpeed);
		genInputFloat(table, childIndex, baseName, "Lighter speed:", 0, 10, 0.02, ScreenSpaceEffectsComponent().eyeAdaptation.lighterSpeed);
		genInputFloat(table, childIndex, baseName, "Low logLum:", -20, 10, 0.05, ScreenSpaceEffectsComponent().eyeAdaptation.lowLogLum);
		genInputFloat(table, childIndex, baseName, "High logLum:", -20, 10, 0.05, ScreenSpaceEffectsComponent().eyeAdaptation.highLogLum);
		genInputFloat(table, childIndex, baseName, "Night offset:", 0, 10, 0.02, ScreenSpaceEffectsComponent().eyeAdaptation.nightOffset);
		genInputFloat(table, childIndex, baseName, "Night desaturate:", 0, 1, 0.02, ScreenSpaceEffectsComponent().eyeAdaptation.nightDesaturate);
		genInputFloat(table, childIndex, baseName, "Night contrast:", 0, 0.1, 0.002, ScreenSpaceEffectsComponent().eyeAdaptation.nightContrast);
		genInputFloat(table, childIndex, baseName, "Key:", 0, 1, 0.02, ScreenSpaceEffectsComponent().eyeAdaptation.key);
		genInputFloat(table, childIndex, baseName, "Strength:", 0, 1, 0.02, ScreenSpaceEffectsComponent().eyeAdaptation.strength);
	}

	{ // tone mapping
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::ToneMapping);
		Entity *panel = ents->createUnique();
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			panel->value<GuiSpoilerComponent>();
			panel->value<GuiTextComponent>().value = "Tone mapping";
			panel->value<GuiLayoutLineComponent>().vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			e->value<GuiTextComponent>().value = "Enabled";
			e->value<GuiCheckBoxComponent>().state = CheckBoxStateEnum::Checked;
		}
		Entity *table = ents->createUnique();
		{
			GuiParentComponent &p = table->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			GuiLayoutTableComponent &t = table->value<GuiLayoutTableComponent>();
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "Shoulder Strength:", 0, 1, 0.02, ScreenSpaceEffectsComponent().tonemap.shoulderStrength);
		genInputFloat(table, childIndex, baseName, "Linear Strength:", 0, 1, 0.02, ScreenSpaceEffectsComponent().tonemap.linearStrength);
		genInputFloat(table, childIndex, baseName, "Linear Angle:", 0, 1, 0.02, ScreenSpaceEffectsComponent().tonemap.linearAngle);
		genInputFloat(table, childIndex, baseName, "Toe Strength:", 0, 1, 0.02, ScreenSpaceEffectsComponent().tonemap.toeStrength);
		genInputFloat(table, childIndex, baseName, "Toe Numerator:", 0, 1, 0.02, ScreenSpaceEffectsComponent().tonemap.toeNumerator);
		genInputFloat(table, childIndex, baseName, "Toe Denominator:", 0, 1, 0.02, ScreenSpaceEffectsComponent().tonemap.toeDenominator);
		genInputFloat(table, childIndex, baseName, "White:", 0, 100, 1, ScreenSpaceEffectsComponent().tonemap.white);
	}

	{ // gamma
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::GammaCorrection);
		Entity *panel = ents->createUnique();
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			panel->value<GuiSpoilerComponent>();
			panel->value<GuiTextComponent>().value = "Gamma";
			panel->value<GuiLayoutLineComponent>().vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			e->value<GuiTextComponent>().value = "Enabled";
			e->value<GuiCheckBoxComponent>().state = CheckBoxStateEnum::Checked;
		}
		Entity *table = ents->createUnique();
		{
			GuiParentComponent &p = table->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			GuiLayoutTableComponent &t = table->value<GuiLayoutTableComponent>();
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "Gamma:", 1, 5, 0.1, ScreenSpaceEffectsComponent().gamma);
	}

	{ // antialiasing
		constexpr sint32 baseName = genBaseName(ScreenSpaceEffectsFlags::AntiAliasing);
		Entity *panel = ents->createUnique();
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			panel->value<GuiSpoilerComponent>();
			panel->value<GuiTextComponent>().value = "Antialiasing";
			panel->value<GuiLayoutLineComponent>().vertical = true;
		}
		{ // enabled
			Entity *e = ents->create(baseName);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 1;
			e->value<GuiTextComponent>().value = "Enabled";
			e->value<GuiCheckBoxComponent>().state = CheckBoxStateEnum::Checked;
		}
	}

	{ // light intensities
		constexpr sint32 baseName = 2000;
		Entity *panel = ents->createUnique();
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = baseName;
			panel->value<GuiSpoilerComponent>();
			panel->value<GuiTextComponent>().value = "Lights Intensities";
			panel->value<GuiLayoutLineComponent>().vertical = true;
		}
		Entity *table = ents->createUnique();
		{
			GuiParentComponent &p = table->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			table->value<GuiLayoutTableComponent>();
		}
		sint32 childIndex = 1;
		genInputFloat(table, childIndex, baseName, "Sun:", 0, 100, 0.1, 3);
		genInputFloat(table, childIndex, baseName, "Ambient:", 0, 1, 0.01, 0.05);
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
		const auto updateListener = controlThread().update.listen(&update);
		const auto closeListener = engineWindow()->events.listen(inputListener<InputClassEnum::WindowClose, InputWindow>(&windowClose));

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
			c.ambientIntensity = 0.05;
			c.near = 0.1;
			c.far = 100;
			e->value<ScreenSpaceEffectsComponent>();
		}
		{ // skybox
			Entity *e = ents->createAnonymous();
			e->value<TransformComponent>();
			e->value<RenderComponent>().object = HashString("scenes/common/skybox.obj");
			e->value<TextureAnimationComponent>();
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
			Entity *e = ents->createAnonymous();
			e->value<RenderComponent>().object = HashString("scenes/common/ground.obj");
			e->value<TransformComponent>().position = Vec3(0, -1.264425, 0);
		}
		{ // sponza
			Entity *e = ents->createAnonymous();
			e->value<RenderComponent>().object = HashString("scenes/mcguire/crytek/sponza.object");
			e->value<TransformComponent>();
		}

		Holder<FpsCamera> fpsCamera = newFpsCamera(ents->get(1));
		fpsCamera->mouseButton = MouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 0.3;
		Holder<StatisticsGui> statistics = newStatisticsGui();

		engineAssets()->add(assetsName);
		engineRun();
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
