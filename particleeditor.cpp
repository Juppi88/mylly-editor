#include "particleeditor.h"
#include "menubar.h"
#include "sidebar.h"
#include "editor.h"
#include "colourpicker.h"
#include <mylly/mgui/widget.h>
#include <mylly/mgui/text.h>
#include <mylly/scene/scene.h>
#include <mylly/scene/object.h>
#include <mylly/scene/emitter.h>
#include <mylly/resources/resources.h>
#include <mylly/renderer/shader.h>
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

	AddMargin(25);

	m_buttonApply = AddWidgetRow(GetEditor()->CreateButton(panel, "Apply"), 30);
	button_set_clicked_handler(m_buttonApply, OnApplyClicked);

	// Hide the editor until needed.
	//SetVisible(false);

	// Add a menu bar button to open the particle editor view.
	GetEditor()->GetMenuBar()->AddButton("Particle Editor", OnMenuOpenClicked);
}

void ParticleEditor::Process(void)
{
}

void ParticleEditor::OnSceneLoad(scene_t *scene)
{
	object_t *object = scene_create_object(scene, nullptr);
	m_emitter = obj_add_emitter(object);

	m_emitter->max_particles = 1000;
	m_emitter->emit_rate = 10;
	m_emitter->emit_duration = 10;
	m_emitter->initial_burst = 50;
	m_emitter->velocity.min = vec3(-5, -5, -5);
	m_emitter->velocity.max = vec3(5, 5, 5);

	// TODO: Rotate particles towards the camera!
	obj_set_local_rotation(object, quat_from_euler_deg(90, 00, 0));

	// Copy initial values from the emitter. If the scene loaded for a second time, copy values back
	// to the new emitter.
	if (!m_valuesCopiedFromEmitter) {

		CopyValuesFromEmitter();
		m_valuesCopiedFromEmitter = true;
	}
	else {
		ApplyValuesToEmitter();
	}

	// TODO: Either attach the emitter to a camera or create a new camera for editing the emitter!
}

void ParticleEditor::OnSceneUnload(void)
{
	if (m_emitter != nullptr) {
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

	widget_set_text(m_inputLifeMin, "%.2f", m_emitter->life.min);
	widget_set_text(m_inputLifeMax, "%.2f", m_emitter->life.max);

	// Velocity
	widget_set_text(m_inputVelocityMin[0], "%.2f", m_emitter->velocity.min.x);
	widget_set_text(m_inputVelocityMin[1], "%.2f", m_emitter->velocity.min.y);
	widget_set_text(m_inputVelocityMin[2], "%.2f", m_emitter->velocity.min.z);

	widget_set_text(m_inputVelocityMax[0], "%.2f", m_emitter->velocity.max.x);
	widget_set_text(m_inputVelocityMax[1], "%.2f", m_emitter->velocity.max.y);
	widget_set_text(m_inputVelocityMax[2], "%.2f", m_emitter->velocity.max.z);

	// Acceleration
	widget_set_text(m_inputAccelerationMin[0], "%.2f", m_emitter->acceleration.min.x);
	widget_set_text(m_inputAccelerationMin[1], "%.2f", m_emitter->acceleration.min.y);
	widget_set_text(m_inputAccelerationMin[2], "%.2f", m_emitter->acceleration.min.z);

	widget_set_text(m_inputAccelerationMax[0], "%.2f", m_emitter->acceleration.max.x);
	widget_set_text(m_inputAccelerationMax[1], "%.2f", m_emitter->acceleration.max.y);
	widget_set_text(m_inputAccelerationMax[2], "%.2f", m_emitter->acceleration.max.z);

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
	widget_set_text(m_inputStartSizeMin, "%.2f", m_emitter->start_size.min);
	widget_set_text(m_inputStartSizeMax, "%.2f", m_emitter->start_size.max);
	widget_set_text(m_inputEndSizeMin, "%.2f", m_emitter->end_size.min);
	widget_set_text(m_inputEndSizeMax, "%.2f", m_emitter->end_size.max);
}

void ParticleEditor::ApplyValuesToEmitter(void)
{
	vec3_t v;

	m_emitter->max_particles = GetInputUShort(m_inputNumParticles);
	m_emitter->emit_duration = GetInputFloat(m_inputEmitDuration);
	m_emitter->emit_rate = GetInputFloat(m_inputEmitRate);
	m_emitter->initial_burst = GetInputUShort(m_inputEmitBurst);

	// Velocity
	v.x = GetInputFloat(m_inputVelocityMin[0]);
	v.y = GetInputFloat(m_inputVelocityMin[1]);
	v.z = GetInputFloat(m_inputVelocityMin[2]);
	m_emitter->velocity.min = v;

	// Acceleration
	v.x = GetInputFloat(m_inputVelocityMax[0]);
	v.y = GetInputFloat(m_inputVelocityMax[1]);
	v.z = GetInputFloat(m_inputVelocityMax[2]);
	m_emitter->velocity.max = v;

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

	emitter_set_particle_sprite(m_emitter, res_get_sprite("shipicons/red"));
	emitter_set_emit_shape(m_emitter, SHAPE_POINT, shape_point(vec3(0, -3, 0)));

	// Restart the emitter.
	emitter_start(m_emitter, m_emitter->max_particles, m_emitter->initial_burst,
	              m_emitter->emit_rate, m_emitter->emit_duration);
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
