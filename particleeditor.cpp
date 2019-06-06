#include "particleeditor.h"
#include "menubar.h"
#include "sidebar.h"
#include "editor.h"
#include "resourcewriter.h"
#include "colourpicker.h"
#include <mylly/mgui/widget.h>
#include <mylly/mgui/text.h>
#include <mylly/scene/scene.h>
#include <mylly/scene/object.h>
#include <mylly/scene/emitter.h>
#include <mylly/scene/sprite.h>
#include <mylly/resources/resources.h>
#include <mylly/renderer/shader.h>
#include <mylly/core/time.h>
#include <mylly/core/string.h>
#include <mylly/math/math.h>
#include <mylly/io/log.h>
#include <stdlib.h>

// -------------------------------------------------------------------------------------------------

ParticleEditor::ParticleEditor(Editor *editor) :
	EditorWindow(editor)
{
}

ParticleEditor::~ParticleEditor(void)
{
}

void ParticleEditor::Create(void)
{
	// Create an empty widget as a container for the editor.
	widget_t *panel = widget_create(GetEditor()->GetSideBar()->GetPanel());

	widget_set_anchors(panel,
		ANCHOR_MIN, 15,
		ANCHOR_MAX, -15,
		ANCHOR_MIN, 15,
		ANCHOR_MAX, -15
	);

	SetPanel(panel);

	widget_t *label;

	AddWidgetRow(GetEditor()->CreateLabel(panel, "Particle Editor"), 20, 10);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Particle Count"));
	AddInputsToLabel(label, &m_inputNumParticles);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Emit Duration"));
	AddInputsToLabel(label, &m_inputEmitDuration);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Emit Rate"));
	AddInputsToLabel(label, &m_inputEmitRate);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Initial Burst"));
	AddInputsToLabel(label, &m_inputEmitBurst);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "World Space"));

	m_checkWorldSpace = GetEditor()->CreateCheckbox(label,
		ANCHOR_MIN, LABEL_WIDTH,
		ANCHOR_MIN, LABEL_WIDTH + 24,
		ANCHOR_MIN, 2,
		ANCHOR_MIN, 26
	);

	AddMargin();

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Life (min/max)"));
	AddInputsToLabel(label, &m_inputLifeMin, &m_inputLifeMax);

	AddMargin();

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Velocity (min)"));
	AddInputsToLabel(label, &m_inputVelocityMin[0], &m_inputVelocityMin[1], &m_inputVelocityMin[2]);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Velocity (max)"));
	AddInputsToLabel(label, &m_inputVelocityMax[0], &m_inputVelocityMax[1], &m_inputVelocityMax[2]);

	AddMargin();

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Acceleration (min)"));
	AddInputsToLabel(label, &m_inputAccelerationMin[0], &m_inputAccelerationMin[1], &m_inputAccelerationMin[2]);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Acceleration (max)"));
	AddInputsToLabel(label, &m_inputAccelerationMax[0], &m_inputAccelerationMax[1], &m_inputAccelerationMax[2]);

	AddMargin();

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Start col. (min/max)"));
	AddColourPickersToLabel(label, &m_pickerStartColourMin, &m_pickerStartColourMax);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "End col. (min/max)"));
	AddColourPickersToLabel(label, &m_pickerEndColourMin, &m_pickerEndColourMax);

	AddMargin();

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Start size (min/max)"));
	AddInputsToLabel(label, &m_inputStartSizeMin, &m_inputStartSizeMax);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "End size (min/max)"));
	AddInputsToLabel(label, &m_inputEndSizeMin, &m_inputEndSizeMax);

	AddMargin();

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Rotation (min/max)"));
	AddInputsToLabel(label, &m_inputRotationSpeedMin, &m_inputRotationSpeedMax);

	AddMargin(25);

	// Add a checkbox to move the effect around for demonstration.
	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Move Effect"));

	m_checkMoveEffect = GetEditor()->CreateCheckbox(label,
		ANCHOR_MIN, LABEL_WIDTH,
		ANCHOR_MIN, LABEL_WIDTH + 24,
		ANCHOR_MIN, 2,
		ANCHOR_MIN, 26
	);

	AddMargin(25);

	m_buttonApply = AddWidgetRow(GetEditor()->CreateButton(panel, "Start"), 30);
	button_set_clicked_handler(m_buttonApply, OnApplyClicked);

	AddMargin(5);

	m_buttonSave = AddWidgetRow(GetEditor()->CreateButton(panel, "Save"), 30);
	button_set_clicked_handler(m_buttonSave, OnSaveClicked);

	// Hide the editor until needed.
	//SetVisible(false);

	// Add a menu bar button to open the particle editor view.
	GetEditor()->GetMenuBar()->AddButton("Particle Editor", OnMenuOpenClicked);
}

void ParticleEditor::Process(void)
{
	// TODO: Do this relative to camera!
	vec3_t position = vec3_zero();

	// If the move around checkbox is checked, calculate a path for the effect.
	if (checkbox_is_toggled(m_checkMoveEffect)) {

		position = vec3(
			25 * cosf(0.75f * get_time().time),
			0,
			18 * cosf(3 * get_time().time)
		);
	}

	obj_set_position(m_emitter->parent, position);
}

void ParticleEditor::OnSceneLoad(scene_t *scene)
{
	object_t *object = scene_create_object(scene, nullptr);

	// TODO: Load emitter resource and use it instead of assigning it directly to the object!
	m_emitter = res_get_emitter("shipexplosion");
	object->emitter = m_emitter; // TODO: This right here is wrong!
	m_emitter->parent = object;

	/*m_emitter = obj_add_emitter(object);

	m_emitter->max_particles = 1000;
	m_emitter->is_world_space = true;
	m_emitter->emit_rate = 500;
	m_emitter->emit_duration = 60;
	m_emitter->initial_burst = 50;
	m_emitter->velocity.min = vec3(-15, -15, -15);
	m_emitter->velocity.max = vec3(15, 15, 15);
	m_emitter->acceleration.min = vec3(-1, -1, -1);
	m_emitter->acceleration.max = vec3(1, 1, 1);
	m_emitter->start_size.min = 3;
	m_emitter->start_size.max = 5;
	m_emitter->end_size.min = 0;
	m_emitter->end_size.max = 0.5f;
	m_emitter->end_colour.min = col_a(255, 255, 255, 0);
	m_emitter->end_colour.max = col_a(255, 255, 255, 0);*/

	// Copy initial values from the emitter. If the scene loaded for a second time, copy values back
	// to the new emitter.
	if (!m_valuesCopiedFromEmitter) {

		CopyValuesFromEmitter();
		m_valuesCopiedFromEmitter = true;
	}
	else if (GetEditor()->IsVisible() && IsVisible()) {
		ApplyValuesToEmitter();
	}

	// TODO: Either attach the emitter to a camera or create a new camera for editing the emitter!
}

void ParticleEditor::OnSceneUnload(void)
{
	if (m_emitter != nullptr) {

		m_emitter->parent->emitter = nullptr; // TODO: Remove this line when using cloned emitters!
		obj_destroy(m_emitter->parent);
	}
}

widget_t *ParticleEditor::AddWidgetRow(widget_t *widget, int16_t height, int16_t padding)
{
	widget_set_anchors(widget,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, m_nextWidgetOffsetY,
		ANCHOR_MIN, m_nextWidgetOffsetY + height
	);

	m_nextWidgetOffsetY += height + padding;

	return widget;
}

void ParticleEditor::AddInputsToLabel(widget_t *parentLabel, widget_t **outInput,
                                     widget_t **outInput2, widget_t **outInput3, widget_t **outInput4)
{
	if (outInput == nullptr) {
		return;
	}

	// Create as many input widgets as there are references (but always at least 1).
	uint32_t numInputs = 1;
	widget_t **inputReferences[4] = { outInput };

	if (outInput2 != nullptr) { inputReferences[numInputs++] = outInput2; }
	if (outInput3 != nullptr) { inputReferences[numInputs++] = outInput3; }
	if (outInput4 != nullptr) { inputReferences[numInputs++] = outInput4; }

	// Calculate the width of each input box.
	vec2i_t parentSize = widget_get_size(parentLabel->parent);
	int16_t totalWidth = parentSize.x - LABEL_WIDTH;
	int16_t inputWidth = (totalWidth - INPUT_MARGIN * (numInputs - 1)) / numInputs;
	int16_t x = LABEL_WIDTH;
	int16_t y = widget_get_position(parentLabel).y;

	// Create the input boxes.
	for (uint32_t i = 0; i < numInputs; i++) {

		*inputReferences[i] = GetEditor()->CreateInputBox(GetPanel(),
			ANCHOR_MIN, x,
			ANCHOR_MIN, x + inputWidth,
			ANCHOR_MIN, y + 2,
			ANCHOR_MIN, y + widget_get_size(parentLabel).y - 2
		);

		x += inputWidth + INPUT_MARGIN;
	}
}

void ParticleEditor::AddColourPickersToLabel(widget_t *parentLabel, widget_t **outCol1, widget_t **outCol2)
{
	if (outCol1 == nullptr || outCol2 == nullptr) {
		return;
	}

	// Calculate the width of each colour picker.
	vec2i_t parentSize = widget_get_size(parentLabel->parent);
	int16_t totalWidth = parentSize.x - LABEL_WIDTH;
	int16_t pickerWidth = (totalWidth - INPUT_MARGIN) / 2;
	int16_t x = LABEL_WIDTH;
	int16_t y = widget_get_position(parentLabel).y;
	

	// Create the input boxes.
	for (uint32_t i = 0; i < 2; i++) {

		widget_t *widget = GetEditor()->CreateColourPicker(GetPanel(),
			ANCHOR_MIN, x,
			ANCHOR_MIN, x + pickerWidth,
			ANCHOR_MIN, y + 2,
			ANCHOR_MIN, y + widget_get_size(parentLabel).y - 2
		);

		widget_set_input_handler(widget, OnColourPickerInputEvent);

		if (i == 0) *outCol1 = widget;
		if (i == 1) *outCol2 = widget;

		x += pickerWidth + INPUT_MARGIN;
	}
}

float ParticleEditor::GetInputFloat(widget_t *inputBox)
{
	return atof(inputBox->text->buffer);
}

uint16_t ParticleEditor::GetInputUShort(widget_t *inputBox)
{
	int value = atoi(inputBox->text->buffer);
	if (value < 0) { value = 0; }
	if (value > 0xFFFF) { value = 0xFFFF; }

	return (uint16_t)value;
}

void ParticleEditor::SetPreviewColour(widget_t *widget, const colour_t &colour)
{
	widget_set_colour(widget, colour);
	shader_set_uniform_float(widget->custom_shader, "Alpha", colour.a / 255.0f);
}

void ParticleEditor::CopyValuesFromEmitter(void)
{

	widget_set_text(m_inputNumParticles, "%u", m_emitter->max_particles);
	widget_set_text(m_inputEmitDuration, "%.1f", m_emitter->emit_duration);
	widget_set_text(m_inputEmitRate, "%.1f", m_emitter->emit_rate);
	widget_set_text(m_inputEmitBurst, "%u", m_emitter->initial_burst);

	checkbox_set_toggled(m_checkWorldSpace, m_emitter->is_world_space);

	widget_set_text(m_inputLifeMin, "%.1f", m_emitter->life.min);
	widget_set_text(m_inputLifeMax, "%.1f", m_emitter->life.max);

	// Velocity
	widget_set_text(m_inputVelocityMin[0], "%.1f", m_emitter->velocity.min.x);
	widget_set_text(m_inputVelocityMin[1], "%.1f", m_emitter->velocity.min.y);
	widget_set_text(m_inputVelocityMin[2], "%.1f", m_emitter->velocity.min.z);

	widget_set_text(m_inputVelocityMax[0], "%.1f", m_emitter->velocity.max.x);
	widget_set_text(m_inputVelocityMax[1], "%.1f", m_emitter->velocity.max.y);
	widget_set_text(m_inputVelocityMax[2], "%.1f", m_emitter->velocity.max.z);

	// Acceleration
	widget_set_text(m_inputAccelerationMin[0], "%.1f", m_emitter->acceleration.min.x);
	widget_set_text(m_inputAccelerationMin[1], "%.1f", m_emitter->acceleration.min.y);
	widget_set_text(m_inputAccelerationMin[2], "%.1f", m_emitter->acceleration.min.z);

	widget_set_text(m_inputAccelerationMax[0], "%.1f", m_emitter->acceleration.max.x);
	widget_set_text(m_inputAccelerationMax[1], "%.1f", m_emitter->acceleration.max.y);
	widget_set_text(m_inputAccelerationMax[2], "%.1f", m_emitter->acceleration.max.z);

	// Colours
	m_startColourMin = m_emitter->start_colour.min;
	SetPreviewColour(m_pickerStartColourMin, m_startColourMin);

	m_startColourMax = m_emitter->start_colour.max;
	SetPreviewColour(m_pickerStartColourMax, m_startColourMax);

	m_endColourMin = m_emitter->end_colour.min;
	SetPreviewColour(m_pickerEndColourMin, m_endColourMin);

	m_endColourMax = m_emitter->end_colour.max;
	SetPreviewColour(m_pickerEndColourMax, m_endColourMax);

	// Size
	widget_set_text(m_inputStartSizeMin, "%.1f", m_emitter->start_size.min);
	widget_set_text(m_inputStartSizeMax, "%.1f", m_emitter->start_size.max);
	widget_set_text(m_inputEndSizeMin, "%.1f", m_emitter->end_size.min);
	widget_set_text(m_inputEndSizeMax, "%.1f", m_emitter->end_size.max);

	// Rotation
	widget_set_text(m_inputRotationSpeedMin, "%.1f", m_emitter->rotation_speed.min);
	widget_set_text(m_inputRotationSpeedMax, "%.1f", m_emitter->rotation_speed.max);
}

void ParticleEditor::ApplyValuesToEmitter(void)
{
	vec3_t v;

	m_emitter->max_particles = GetInputUShort(m_inputNumParticles);
	m_emitter->emit_duration = GetInputFloat(m_inputEmitDuration);
	m_emitter->emit_rate = GetInputFloat(m_inputEmitRate);
	m_emitter->initial_burst = GetInputUShort(m_inputEmitBurst);

	m_emitter->is_world_space = checkbox_is_toggled(m_checkWorldSpace);

	m_emitter->life.min = GetInputFloat(m_inputLifeMin);
	m_emitter->life.max = GetInputFloat(m_inputLifeMax);

	// Velocity
	v.x = GetInputFloat(m_inputVelocityMin[0]);
	v.y = GetInputFloat(m_inputVelocityMin[1]);
	v.z = GetInputFloat(m_inputVelocityMin[2]);
	m_emitter->velocity.min = v;

	v.x = GetInputFloat(m_inputVelocityMax[0]);
	v.y = GetInputFloat(m_inputVelocityMax[1]);
	v.z = GetInputFloat(m_inputVelocityMax[2]);
	m_emitter->velocity.max = v;

	// Acceleration
	v.x = GetInputFloat(m_inputAccelerationMin[0]);
	v.y = GetInputFloat(m_inputAccelerationMin[1]);
	v.z = GetInputFloat(m_inputAccelerationMin[2]);
	m_emitter->acceleration.min = v;

	v.x = GetInputFloat(m_inputAccelerationMax[0]);
	v.y = GetInputFloat(m_inputAccelerationMax[1]);
	v.z = GetInputFloat(m_inputAccelerationMax[2]);
	m_emitter->acceleration.max = v;

	// Colours
	m_emitter->start_colour.min = m_startColourMin;
	m_emitter->start_colour.max = m_startColourMax;
	m_emitter->end_colour.min = m_endColourMin;
	m_emitter->end_colour.max = m_endColourMax;

	// Size
	m_emitter->start_size.min = GetInputFloat(m_inputStartSizeMin);
	m_emitter->start_size.max = GetInputFloat(m_inputStartSizeMax);
	m_emitter->end_size.min = GetInputFloat(m_inputEndSizeMin);
	m_emitter->end_size.max = GetInputFloat(m_inputEndSizeMax);

	// Rotation
	m_emitter->rotation_speed.min = GetInputFloat(m_inputRotationSpeedMin);
	m_emitter->rotation_speed.max = GetInputFloat(m_inputRotationSpeedMax);

	// TODO: Make it possible to change the sprite and emit shape from the editor.
	//emitter_set_particle_sprite(m_emitter, res_get_sprite("shipicons/red"));
	//emitter_set_particle_sprite(m_emitter, res_get_sprite("0027"));
	//emitter_set_emit_shape(m_emitter, SHAPE_POINT, shape_point(vec3(0, -3, 0)));

	// Restart the emitter.
	emitter_start(m_emitter);
}

void ParticleEditor::SaveEmitterToFile(void)
{
	if (m_emitter == nullptr) {
		return;
	}

	ResourceWriter writer;

	if (string_is_null_or_empty(m_emitter->resource.path) ||
		!writer.BeginWrite(m_emitter->resource.path)) {

		log_warning("Editor", "Could not write to emitter resource file '%s'.",
		             m_emitter->resource.path);

		return;
	}

	writer.Write("version", 1);

	writer.BeginArray("emitters");
	{
		// TODO: Do this for each subemitter
		writer.BeginObject();
		{
			writer.Write("sprite", m_emitter->sprite != nullptr ? m_emitter->sprite->resource.res_name : "");
			writer.Write("is_world_space", m_emitter->is_world_space);
			writer.Write("max_particles", m_emitter->max_particles);
			writer.Write("emit_duration", m_emitter->emit_duration);
			writer.Write("emit_rate", m_emitter->emit_rate);
			writer.Write("initial_burst", m_emitter->initial_burst);
			writer.Write("life_min", m_emitter->life.min);
			writer.Write("life_max", m_emitter->life.max);
			writer.Write("start_size_min", m_emitter->start_size.min);
			writer.Write("start_size_max", m_emitter->start_size.max);
			writer.Write("end_size_min", m_emitter->end_size.min);
			writer.Write("end_size_max", m_emitter->end_size.max);
			writer.Write("rotation_min", m_emitter->rotation_speed.min);
			writer.Write("rotation_max", m_emitter->rotation_speed.max);
			writer.Write("velocity_min", m_emitter->velocity.min);
			writer.Write("velocity_max", m_emitter->velocity.max);
			writer.Write("acceleration_min", m_emitter->acceleration.min);
			writer.Write("acceleration_max", m_emitter->acceleration.max);
			writer.Write("start_colour_min", m_emitter->start_colour.min);
			writer.Write("start_colour_max", m_emitter->start_colour.max);
			writer.Write("end_colour_min", m_emitter->end_colour.min);
			writer.Write("end_colour_max", m_emitter->end_colour.max);

			switch (m_emitter->shape_type) {

				case SHAPE_POINT:
					writer.Write("emit_centre", m_emitter->shape.point.centre);
					break;

				case SHAPE_CIRCLE:
					writer.Write("emit_centre", m_emitter->shape.circle.centre);
					writer.Write("emit_radius", m_emitter->shape.circle.radius);
					break;

				case SHAPE_BOX:
					writer.Write("emit_box_min", m_emitter->shape.box.min);
					writer.Write("emit_box_max", m_emitter->shape.box.max);
					break;
			}

		}
		writer.EndObject();
	}
	writer.EndArray();

	// Write the resource file to disk.
	writer.FinishWrite();
}

void ParticleEditor::OnMenuOpenClicked(widget_t *button)
{
	UNUSED(button);
	Instance()->SetVisible(true);
}

void ParticleEditor::OnApplyClicked(widget_t *button)
{
	UNUSED(button);
	Instance()->ApplyValuesToEmitter();
}

void ParticleEditor::OnSaveClicked(widget_t *button)
{
	UNUSED(button);
	Instance()->SaveEmitterToFile();
}

void ParticleEditor::OnColourPickerInputEvent(widget_event_t *event)
{
	if (event->type != INPUT_MOUSE_BUTTON_DOWN) {
		return;
	}

	colour_t startColour = COL_WHITE;

	if (event->widget == Instance()->m_pickerStartColourMin) {
		startColour = Instance()->m_startColourMin;
	}
	else if (event->widget == Instance()->m_pickerStartColourMax) {
		startColour = Instance()->m_startColourMax;
	}
	else if (event->widget == Instance()->m_pickerEndColourMin) {
		startColour = Instance()->m_endColourMin;
	}
	else if (event->widget == Instance()->m_pickerEndColourMax) {
		startColour = Instance()->m_endColourMax;
	}

	Instance()->GetEditor()->GetColourPicker()->SelectColour(
		startColour, OnColourSelected, event->widget);
}

void ParticleEditor::OnColourSelected(const colour_t &colour, void *context)
{
	bool updatePreview = false;

	if (context == Instance()->m_pickerStartColourMin) {

		Instance()->m_startColourMin = colour;
		updatePreview = true;
	}
	else if (context == Instance()->m_pickerStartColourMax) {

		Instance()->m_startColourMax = colour;
		updatePreview = true;
	}
	else if (context == Instance()->m_pickerEndColourMin) {

		Instance()->m_endColourMin = colour;
		updatePreview = true;
	}
	else if (context == Instance()->m_pickerEndColourMax) {

		Instance()->m_endColourMax = colour;
		updatePreview = true;
	}

	// Update the preview.
	if (updatePreview) {
		Instance()->SetPreviewColour((widget_t *)context, colour);
	}
}
