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
	// Collect a list of available resources.
	res_foreach_emitter(AddToEffectList);

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

	// Effect selector
	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Effect"));
	AddDropdownToLabel(label, panel, &m_dropdownEffect);

	dropdown_set_selected_handler(m_dropdownEffect, OnEffectSelected);

	AddMargin(15);

	// Emit properties
	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Particle Count"));
	AddInputsToLabel(label, nullptr, &m_inputNumParticles);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Emit Duration"));
	AddInputsToLabel(label, nullptr, &m_inputEmitDuration);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Emit Rate"));
	AddInputsToLabel(label, nullptr, &m_inputEmitRate);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Initial Burst"));
	AddInputsToLabel(label, nullptr, &m_inputEmitBurst);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "World Space"));

	m_checkWorldSpace = GetEditor()->CreateCheckbox(label,
		ANCHOR_MIN, LABEL_WIDTH,
		ANCHOR_MIN, LABEL_WIDTH + 24,
		ANCHOR_MIN, 2,
		ANCHOR_MIN, 26
	);

	AddMargin();

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Start spd (min/max)"));
	AddInputsToLabel(label, nullptr, &m_inputStartSpeedMin, &m_inputStartSpeedMax);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "End spd (min/max)"));
	AddInputsToLabel(label, nullptr, &m_inputEndSpeedMin, &m_inputEndSpeedMax);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Acceleration (min)"));
	AddInputsToLabel(label, nullptr, &m_inputAccelerationMin[0], &m_inputAccelerationMin[1], &m_inputAccelerationMin[2]);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Acceleration (max)"));
	AddInputsToLabel(label, nullptr, &m_inputAccelerationMax[0], &m_inputAccelerationMax[1], &m_inputAccelerationMax[2]);

	AddMargin();

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Start col. (min/max)"));
	AddColourPickersToLabel(label, &m_pickerStartColourMin, &m_pickerStartColourMax);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "End col. (min/max)"));
	AddColourPickersToLabel(label, &m_pickerEndColourMin, &m_pickerEndColourMax);

	AddMargin();

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Life (min/max)"));
	AddInputsToLabel(label, nullptr, &m_inputLifeMin, &m_inputLifeMax);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Start size (min/max)"));
	AddInputsToLabel(label, nullptr, &m_inputStartSizeMin, &m_inputStartSizeMax);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "End size (min/max)"));
	AddInputsToLabel(label, nullptr, &m_inputEndSizeMin, &m_inputEndSizeMax);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Rotation (min/max)"));
	AddInputsToLabel(label, nullptr, &m_inputRotationSpeedMin, &m_inputRotationSpeedMax);

	AddMargin(5);

	// Emit shapes
	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Emit Shape"));
	AddDropdownToLabel(label, panel, &m_dropdownEmitShape);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Emit Position"));
	AddInputsToLabel(label, nullptr, &m_inputEmitPosition[0], &m_inputEmitPosition[1], &m_inputEmitPosition[2]);

	// Emit shape containers
	m_emitCircleContainer = widget_create(panel);
	widget_set_anchors(m_emitCircleContainer,
		ANCHOR_MIN, 0, ANCHOR_MAX, 0, ANCHOR_MIN, 0, ANCHOR_MAX, 0);

	m_emitBoxContainer = widget_create(panel);
	widget_set_anchors(m_emitBoxContainer,
		ANCHOR_MIN, 0, ANCHOR_MAX, 0, ANCHOR_MIN, 0, ANCHOR_MAX, 0);

	m_emitConeContainer = widget_create(panel);
	widget_set_anchors(m_emitConeContainer,
		ANCHOR_MIN, 0, ANCHOR_MAX, 0, ANCHOR_MIN, 0, ANCHOR_MAX, 0);

	// Store current Y level so we can spawn multiple widgets there.
	int16_t y = m_nextWidgetOffsetY;

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(m_emitCircleContainer, "Shape Radius"));
	AddInputsToLabel(label, m_emitCircleContainer, &m_inputEmitCircleRadius);

	// Reset widget layer Y.
	m_nextWidgetOffsetY = y;

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(m_emitBoxContainer, "Box Extents"));
	AddInputsToLabel(label, m_emitBoxContainer,
		&m_inputEmitBoxExtents[0], &m_inputEmitBoxExtents[1], &m_inputEmitBoxExtents[2]);

	m_nextWidgetOffsetY = y;

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(m_emitConeContainer, "Cone Angle"));
	AddInputsToLabel(label, m_emitConeContainer, &m_inputEmitConeAngle);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(m_emitConeContainer, "Cone Radius"));
	AddInputsToLabel(label, m_emitConeContainer, &m_inputEmitConeRadius);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(m_emitConeContainer, "Cone Volume"));
	AddInputsToLabel(label, m_emitConeContainer, &m_inputEmitConeVolume);

	// Hide all emit shape fields.
	widget_set_visible(m_emitCircleContainer, false);
	widget_set_visible(m_emitBoxContainer, false);
	widget_set_visible(m_emitConeContainer, false);

	AddMargin();

	// Subemitters
	m_subemitterContainer = widget_create(panel);

	widget_set_anchors(m_subemitterContainer,
		ANCHOR_MIN, 0, ANCHOR_MAX, 0, ANCHOR_MIN, 0, ANCHOR_MAX, 0);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Subemitter"));
	AddDropdownWithAddRemoveButtons(label, &m_dropdownSubemitter,
	                                &m_buttonAddSubemitter, &m_buttonRemoveSubemitter);

	button_set_clicked_handler(m_buttonAddSubemitter, OnAddSubemitterClicked);
	button_set_clicked_handler(m_buttonRemoveSubemitter, OnRemoveSubemitterClicked);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(m_subemitterContainer, "Subemitter Effect"));
	AddDropdownToLabel(label, m_subemitterContainer, &m_dropdownSubemitterResource);

	label = AddWidgetRow(GetEditor()->CreateSmallLabel(m_subemitterContainer, "Trigger"));
	AddDropdownToLabel(label, m_subemitterContainer, &m_dropdownSubemitterType);

	AddMargin(25);

	// Add a checkbox to move the effect around for demonstration.
	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Move Effect"));

	m_checkMoveEffect = GetEditor()->CreateCheckbox(label,
		ANCHOR_MIN, LABEL_WIDTH,
		ANCHOR_MIN, LABEL_WIDTH + 24,
		ANCHOR_MIN, 2,
		ANCHOR_MIN, 26
	);

	AddMargin(5);

	// Same but rotate it 90deg.
	label = AddWidgetRow(GetEditor()->CreateSmallLabel(panel, "Rotate 90 deg"));

	m_checkRotateEffect = GetEditor()->CreateCheckbox(label,
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

	// Add a change callback and options to emit shape dropdown.
	dropdown_set_selected_handler(m_dropdownEmitShape, OnEmitShapeSelected);
	dropdown_add_option(m_dropdownEmitShape, "Circle", (void *)SHAPE_CIRCLE);
	dropdown_add_option(m_dropdownEmitShape, "Sphere", (void *)SHAPE_SPHERE);
	dropdown_add_option(m_dropdownEmitShape, "Cone", (void *)SHAPE_CONE);
	dropdown_add_option(m_dropdownEmitShape, "Box", (void *)SHAPE_BOX);

	// Populate the editable effect list.
	emitter_t *emitter;

	arr_foreach(m_effectList, emitter) {

		dropdown_add_option(m_dropdownEffect, emitter->resource.res_name, emitter);
		dropdown_add_option(m_dropdownSubemitterResource, emitter->resource.res_name, emitter);
	}

	// Populate subemitter types.
	dropdown_add_option(m_dropdownSubemitterType, "On Create", (void *)SUBEMITTER_CREATE);
	dropdown_add_option(m_dropdownSubemitterType, "On Particle Spawn", (void *)SUBEMITTER_PARTICLE_SPAWN);
	dropdown_add_option(m_dropdownSubemitterType, "On Particle Death", (void *)SUBEMITTER_PARTICLE_DEATH);

	dropdown_set_selected_handler(m_dropdownSubemitter, OnSubemitterSelected);
	dropdown_set_selected_handler(m_dropdownSubemitterResource, OnSubemitterEffectSelected);
	dropdown_set_selected_handler(m_dropdownSubemitterType, OnSubemitterTypeSelected);

	// Hide the editor until needed.
	//SetVisible(false);

	// Add a menu bar button to open the particle editor view.
	GetEditor()->GetMenuBar()->AddButton("Particle Editor", OnMenuOpenClicked);
}

void ParticleEditor::Process(void)
{
	if (m_emitter == nullptr) {
		return;
	}

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
	UNUSED(scene);

	// Create a new scene object with a copy of the edited emitter resource.
	if (m_emitterResource != nullptr)	 {
		LoadEmitter(m_emitterResource);
	}
}

void ParticleEditor::OnSceneUnload(void)
{
	if (m_emitter != nullptr) {

		obj_destroy(m_emitter->parent);
		m_emitter = nullptr;
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

void ParticleEditor::AddInputsToLabel(widget_t *parentLabel, widget_t *parentPanel, widget_t **outInput,
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
	if (parentPanel == nullptr) {
		parentPanel = GetPanel();
	}

	for (uint32_t i = 0; i < numInputs; i++) {

		*inputReferences[i] = GetEditor()->CreateInputBox(parentPanel,
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
	
	// Create the pickers.
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

void ParticleEditor::AddDropdownToLabel(widget_t *parentLabel,
                                        widget_t *parentPanel, widget_t **outDropdown)
{
	if (outDropdown == nullptr) {
		return;
	}

	// Calculate the width of the dropdown.
	int16_t dropdownWidth = widget_get_size(parentLabel->parent).x - LABEL_WIDTH;
	int16_t x = LABEL_WIDTH;
	int16_t y = widget_get_position(parentLabel).y;
	
	// Create the input boxes.
	*outDropdown = GetEditor()->CreateDropDown(parentPanel,
		ANCHOR_MIN, x,
		ANCHOR_MIN, x + dropdownWidth,
		ANCHOR_MIN, y + 2,
		ANCHOR_MIN, y + widget_get_size(parentLabel).y - 2
	);
}

void ParticleEditor::AddDropdownWithAddRemoveButtons(widget_t *parentLabel, widget_t **outDropdown,
                                               widget_t **outAddButton, widget_t **outRemoveButton)
{
	if (outDropdown == nullptr || outAddButton == nullptr || outRemoveButton == nullptr) {
		return;
	}

	// Calculate the width of the dropdown.
	const int16_t buttonWidth = 20;
	const int16_t buttonPadding = 2;

	int16_t dropdownWidth = widget_get_size(parentLabel->parent).x - LABEL_WIDTH -
		2 * (buttonWidth - buttonPadding);

	int16_t x = LABEL_WIDTH;
	int16_t y = widget_get_position(parentLabel).y;
	
	// Create the input box.
	*outDropdown = GetEditor()->CreateDropDown(GetPanel(),
		ANCHOR_MIN, x,
		ANCHOR_MIN, x + dropdownWidth,
		ANCHOR_MIN, y + 2,
		ANCHOR_MIN, y + widget_get_size(parentLabel).y - 2
	);

	x += dropdownWidth + buttonPadding;

	// Create the add button.
	*outAddButton = GetEditor()->CreateButton(GetPanel(), "+",
		ANCHOR_MIN, x,
		ANCHOR_MIN, x + buttonWidth,
		ANCHOR_MIN, y + 2,
		ANCHOR_MIN, y + widget_get_size(parentLabel).y - 2
	);

	x += buttonWidth + buttonPadding;

	// Create the remove button.
	*outRemoveButton = GetEditor()->CreateButton(GetPanel(), "-",
		ANCHOR_MIN, x,
		ANCHOR_MIN, x + buttonWidth,
		ANCHOR_MIN, y + 2,
		ANCHOR_MIN, y + widget_get_size(parentLabel).y - 2
	);

	widget_set_text_alignment(*outAddButton, ALIGNMENT_BOTTOM);
	widget_set_text_alignment(*outRemoveButton, ALIGNMENT_BOTTOM);
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

void ParticleEditor::LoadEmitter(emitter_t *resource, bool start)
{
	// The scene has not been created yet.
	if (GetEditor()->GetScene() == nullptr) {
		return;
	}

	// Destroy the old emitter.
	if (m_emitter != nullptr) {

		obj_destroy(m_emitter->parent);
		m_emitter = nullptr;
	}

	if (resource != m_loadedEmitterResource) {

		m_loadedEmitterResource = resource;

		// Copy values from the emitter resource into the UI.
		CopyValuesFromEmitter();
	}

	// Create a new object and attach a copy of the emitter resource to it.
	object_t *object = scene_create_object(GetEditor()->GetScene(), nullptr);
	m_emitter = obj_add_emitter(object, resource);

	// TODO: Either attach the emitter to a camera or create a new camera for editing the emitter!

	// Start the emitter.
	if (start) {
		emitter_start(m_emitter);
	}

	// TODO: Should there be per-emitter rotation in addition to object's rotation?
	if (checkbox_is_toggled(m_checkRotateEffect)) {
		obj_set_local_rotation(m_emitter->parent, quat_from_euler_deg(90, 0, 0));
	}
}

void ParticleEditor::CopyValuesFromEmitter(void)
{
	widget_set_text(m_inputNumParticles, "%u", m_emitterResource->max_particles);
	widget_set_text(m_inputEmitDuration, "%.1f", m_emitterResource->emit_duration);
	widget_set_text(m_inputEmitRate, "%.1f", m_emitterResource->emit_rate);
	widget_set_text(m_inputEmitBurst, "%u", m_emitterResource->initial_burst);

	checkbox_set_toggled(m_checkWorldSpace, m_emitterResource->is_world_space);

	widget_set_text(m_inputLifeMin, "%.1f", m_emitterResource->life.min);
	widget_set_text(m_inputLifeMax, "%.1f", m_emitterResource->life.max);

	// Speed
	widget_set_text(m_inputStartSpeedMin, "%.1f", m_emitterResource->start_speed.min);
	widget_set_text(m_inputStartSpeedMax, "%.1f", m_emitterResource->start_speed.max);
	widget_set_text(m_inputEndSpeedMin, "%.1f", m_emitterResource->end_speed.min);
	widget_set_text(m_inputEndSpeedMax, "%.1f", m_emitterResource->end_speed.max);

	// Acceleration
	widget_set_text(m_inputAccelerationMin[0], "%.1f", m_emitterResource->acceleration.min.x);
	widget_set_text(m_inputAccelerationMin[1], "%.1f", m_emitterResource->acceleration.min.y);
	widget_set_text(m_inputAccelerationMin[2], "%.1f", m_emitterResource->acceleration.min.z);

	widget_set_text(m_inputAccelerationMax[0], "%.1f", m_emitterResource->acceleration.max.x);
	widget_set_text(m_inputAccelerationMax[1], "%.1f", m_emitterResource->acceleration.max.y);
	widget_set_text(m_inputAccelerationMax[2], "%.1f", m_emitterResource->acceleration.max.z);

	// Colours
	m_startColourMin = m_emitterResource->start_colour.min;
	SetPreviewColour(m_pickerStartColourMin, m_startColourMin);

	m_startColourMax = m_emitterResource->start_colour.max;
	SetPreviewColour(m_pickerStartColourMax, m_startColourMax);

	m_endColourMin = m_emitterResource->end_colour.min;
	SetPreviewColour(m_pickerEndColourMin, m_endColourMin);

	m_endColourMax = m_emitterResource->end_colour.max;
	SetPreviewColour(m_pickerEndColourMax, m_endColourMax);

	// Size
	widget_set_text(m_inputStartSizeMin, "%.1f", m_emitterResource->start_size.min);
	widget_set_text(m_inputStartSizeMax, "%.1f", m_emitterResource->start_size.max);
	widget_set_text(m_inputEndSizeMin, "%.1f", m_emitterResource->end_size.min);
	widget_set_text(m_inputEndSizeMax, "%.1f", m_emitterResource->end_size.max);

	// Rotation
	widget_set_text(m_inputRotationSpeedMin, "%.1f", m_emitterResource->rotation_speed.min);
	widget_set_text(m_inputRotationSpeedMax, "%.1f", m_emitterResource->rotation_speed.max);

	// Emit shape
	dropdown_select_option_by_data(m_dropdownEmitShape, (void *)m_emitterResource->shape.type);

	widget_set_text(m_inputEmitPosition[0], "%.1f", m_emitterResource->shape.position.x);
	widget_set_text(m_inputEmitPosition[1], "%.1f", m_emitterResource->shape.position.y);
	widget_set_text(m_inputEmitPosition[2], "%.1f", m_emitterResource->shape.position.z);

	widget_set_text(m_inputEmitCircleRadius, "%.1f", m_emitterResource->shape.circle.radius);
	widget_set_text(m_inputEmitBoxExtents[0], "%.1f", m_emitterResource->shape.box.extents.x);
	widget_set_text(m_inputEmitBoxExtents[1], "%.1f", m_emitterResource->shape.box.extents.y);
	widget_set_text(m_inputEmitBoxExtents[2], "%.1f", m_emitterResource->shape.box.extents.z);
	widget_set_text(m_inputEmitConeAngle, "%.1f", m_emitterResource->shape.cone.angle);
	widget_set_text(m_inputEmitConeRadius, "%.1f", m_emitterResource->shape.cone.radius);
	widget_set_text(m_inputEmitConeVolume, "%.1f", m_emitterResource->shape.cone.emit_volume);

	// Refresh the subemitter list.
	RefreshSubemitterList(0);
}

void ParticleEditor::ApplyValuesToEmitter(void)
{
	vec3_t v;

	m_emitterResource->max_particles = GetInputUShort(m_inputNumParticles);
	m_emitterResource->emit_duration = GetInputFloat(m_inputEmitDuration);
	m_emitterResource->emit_rate = GetInputFloat(m_inputEmitRate);
	m_emitterResource->initial_burst = GetInputUShort(m_inputEmitBurst);

	m_emitterResource->is_world_space = checkbox_is_toggled(m_checkWorldSpace);

	m_emitterResource->life.min = GetInputFloat(m_inputLifeMin);
	m_emitterResource->life.max = GetInputFloat(m_inputLifeMax);

	// Speed
	m_emitterResource->start_speed.min = GetInputFloat(m_inputStartSpeedMin);
	m_emitterResource->start_speed.max = GetInputFloat(m_inputStartSpeedMax);
	m_emitterResource->end_speed.min = GetInputFloat(m_inputEndSpeedMin);
	m_emitterResource->end_speed.max = GetInputFloat(m_inputEndSpeedMax);

	// Acceleration
	v.x = GetInputFloat(m_inputAccelerationMin[0]);
	v.y = GetInputFloat(m_inputAccelerationMin[1]);
	v.z = GetInputFloat(m_inputAccelerationMin[2]);
	m_emitterResource->acceleration.min = v;

	v.x = GetInputFloat(m_inputAccelerationMax[0]);
	v.y = GetInputFloat(m_inputAccelerationMax[1]);
	v.z = GetInputFloat(m_inputAccelerationMax[2]);
	m_emitterResource->acceleration.max = v;

	// Colours
	m_emitterResource->start_colour.min = m_startColourMin;
	m_emitterResource->start_colour.max = m_startColourMax;
	m_emitterResource->end_colour.min = m_endColourMin;
	m_emitterResource->end_colour.max = m_endColourMax;

	// Size
	m_emitterResource->start_size.min = GetInputFloat(m_inputStartSizeMin);
	m_emitterResource->start_size.max = GetInputFloat(m_inputStartSizeMax);
	m_emitterResource->end_size.min = GetInputFloat(m_inputEndSizeMin);
	m_emitterResource->end_size.max = GetInputFloat(m_inputEndSizeMax);

	// Rotation
	m_emitterResource->rotation_speed.min = GetInputFloat(m_inputRotationSpeedMin);
	m_emitterResource->rotation_speed.max = GetInputFloat(m_inputRotationSpeedMax);

	// Emit shape
	vec3_t emitPos = vec3(
		GetInputFloat(m_inputEmitPosition[0]),
		GetInputFloat(m_inputEmitPosition[1]),
		GetInputFloat(m_inputEmitPosition[2])
	);


	emit_shape_t shape;
	emit_shape_type_t type =
		(emit_shape_type_t)(intptr_t)dropdown_get_selected_option_data(m_dropdownEmitShape);

	switch (type) {

		case SHAPE_CIRCLE:
			shape = shape_circle(emitPos, GetInputFloat(m_inputEmitCircleRadius));
			break;

		case SHAPE_SPHERE:
			shape = shape_sphere(emitPos, GetInputFloat(m_inputEmitCircleRadius));
			break;

		case SHAPE_BOX:
			v.x = GetInputFloat(m_inputEmitBoxExtents[0]);
			v.y = GetInputFloat(m_inputEmitBoxExtents[1]);
			v.z = GetInputFloat(m_inputEmitBoxExtents[2]);

			shape = shape_box(emitPos, v);
			break;

		case SHAPE_CONE:
			shape = shape_cone(emitPos,
				GetInputFloat(m_inputEmitConeAngle),
				GetInputFloat(m_inputEmitConeRadius),
				GetInputFloat(m_inputEmitConeVolume)
			);
			break;
	}

	emitter_set_emit_shape(m_emitterResource, shape);

	// TODO: Make it possible to change the sprite and emit shape from the editor.
	//emitter_set_particle_sprite(m_emitter, res_get_sprite("shipicons/red"));
	//emitter_set_particle_sprite(m_emitter, res_get_sprite("0027"));
	//emitter_set_emit_shape(m_emitter, SHAPE_POINT, shape_point(vec3(0, -3, 0)));
}

void ParticleEditor::SaveEmitterToFile(void)
{
	if (m_emitterResource == nullptr) {
		return;
	}

	ResourceWriter writer;

	if (string_is_null_or_empty(m_emitterResource->resource.path) ||
		!writer.BeginWrite(m_emitterResource->resource.path)) {

		log_warning("Editor", "Could not write to emitter resource file '%s'.",
		             m_emitterResource->resource.path);

		return;
	}

	writer.Write("version", 1);

	writer.BeginObject("emitter");
	{
		const char *spriteName = (m_emitterResource->sprite != nullptr ?
		                          m_emitterResource->sprite->resource.res_name : "");

		writer.Write("sprite", spriteName);
		writer.Write("is_world_space", m_emitterResource->is_world_space);
		writer.Write("max_particles", m_emitterResource->max_particles);
		writer.Write("emit_duration", m_emitterResource->emit_duration);
		writer.Write("emit_rate", m_emitterResource->emit_rate);
		writer.Write("initial_burst", m_emitterResource->initial_burst);
		writer.Write("life_min", m_emitterResource->life.min);
		writer.Write("life_max", m_emitterResource->life.max);
		writer.Write("start_size_min", m_emitterResource->start_size.min);
		writer.Write("start_size_max", m_emitterResource->start_size.max);
		writer.Write("end_size_min", m_emitterResource->end_size.min);
		writer.Write("end_size_max", m_emitterResource->end_size.max);
		writer.Write("rotation_min", m_emitterResource->rotation_speed.min);
		writer.Write("rotation_max", m_emitterResource->rotation_speed.max);
		writer.Write("start_speed_min", m_emitterResource->start_speed.min);
		writer.Write("start_speed_max", m_emitterResource->start_speed.max);
		writer.Write("end_speed_min", m_emitterResource->end_speed.min);
		writer.Write("end_speed_max", m_emitterResource->end_speed.max);
		writer.Write("acceleration_min", m_emitterResource->acceleration.min);
		writer.Write("acceleration_max", m_emitterResource->acceleration.max);
		writer.Write("start_colour_min", m_emitterResource->start_colour.min);
		writer.Write("start_colour_max", m_emitterResource->start_colour.max);
		writer.Write("end_colour_min", m_emitterResource->end_colour.min);
		writer.Write("end_colour_max", m_emitterResource->end_colour.max);

		// Emit shape
		writer.Write("emit_position", m_emitterResource->shape.position);

		switch (m_emitterResource->shape.type) {

			case SHAPE_CIRCLE:
				writer.Write("emit_circle_radius", m_emitterResource->shape.circle.radius);
				break;

			case SHAPE_SPHERE:
				writer.Write("emit_sphere_radius", m_emitterResource->shape.sphere.radius);
				break;

			case SHAPE_BOX:
				writer.Write("emit_box_extents", m_emitterResource->shape.box.extents);
				break;

			case SHAPE_CONE:
				writer.Write("emit_cone_angle", m_emitterResource->shape.cone.angle);
				writer.Write("emit_cone_radius", m_emitterResource->shape.cone.radius);
				writer.Write("emit_cone_volume", m_emitterResource->shape.cone.emit_volume);
				break;
		}

		// Subemitters
		writer.BeginArray("subemitters");

		for (uint32_t i = 0; i < m_emitterResource->subemitters.count; i++) {

			subemitter_t &subemitter = m_emitterResource->subemitters.items[i];

			// Skip subemitters which don't define a particle emitter.
			if (subemitter.emitter == nullptr) {
				continue;
			}

			writer.BeginObject();

				writer.Write("effect", subemitter.emitter->resource.res_name);
				writer.Write("type", (int)subemitter.type);

			writer.EndObject();
		}

		writer.EndArray();
	}
	writer.EndObject();

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

	// Apply current values to the template and restart the preview emitter.
	Instance()->ApplyValuesToEmitter();
	Instance()->LoadEmitter(Instance()->m_emitterResource, true);
}

void ParticleEditor::OnSaveClicked(widget_t *button)
{
	UNUSED(button);

	// Apply current values to the template and save it into a resource file.
	Instance()->ApplyValuesToEmitter();
	Instance()->SaveEmitterToFile();
}

void ParticleEditor::OnEffectSelected(widget_t *dropdown, const char *option, void *data)
{
	UNUSED(dropdown);
	UNUSED(option);

	Instance()->m_emitterResource = (emitter_t *)data;

	if (Instance()->GetEditor()->GetScene() != nullptr) {
		Instance()->LoadEmitter(Instance()->m_emitterResource);
	}
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

void ParticleEditor::OnEmitShapeSelected(widget_t *dropdown, const char *option, void *data)
{
	UNUSED(dropdown);
	UNUSED(option);

	// Hide all emit shape field containers.
	widget_set_visible(Instance()->m_emitCircleContainer, false);
	widget_set_visible(Instance()->m_emitBoxContainer, false);
	widget_set_visible(Instance()->m_emitConeContainer, false);

	emit_shape_type_t type = (emit_shape_type_t)(intptr_t)data;

	// Display the fields for the selected shape.
	switch (type) {

		case SHAPE_CIRCLE:
		case SHAPE_SPHERE:
			widget_set_visible(Instance()->m_emitCircleContainer, true);
			break;

		case SHAPE_BOX:
			widget_set_visible(Instance()->m_emitBoxContainer, true);
			break;

		case SHAPE_CONE:
			widget_set_visible(Instance()->m_emitConeContainer, true);
			break;
	}
}

void ParticleEditor::OnSubemitterSelected(widget_t *dropdown, const char *option, void *data)
{
	UNUSED(dropdown);
	UNUSED(option);

	if (Instance()->m_emitterResource != nullptr) {
		Instance()->RefreshSubEmitter((intptr_t)data);
	}
}

void ParticleEditor::OnSubemitterEffectSelected(widget_t *dropdown, const char *option, void *data)
{
	UNUSED(dropdown);
	UNUSED(option);

	if (Instance()->m_emitterResource != nullptr) {

		uint32_t selected =
			(intptr_t)dropdown_get_selected_option_data(Instance()->m_dropdownSubemitter);

		Instance()->m_emitterResource->subemitters.items[selected].emitter = (emitter_t *)data;
	}
}

void ParticleEditor::OnSubemitterTypeSelected(widget_t *dropdown, const char *option, void *data)
{
	UNUSED(dropdown);
	UNUSED(option);

	if (Instance()->m_emitterResource != nullptr) {

		uint32_t selected =
			(intptr_t)dropdown_get_selected_option_data(Instance()->m_dropdownSubemitter);

		Instance()->m_emitterResource->subemitters.items[selected].type = (subemitter_type_t)(intptr_t)data;
	}
}

void ParticleEditor::RefreshSubEmitter(uint32_t subemitterIndex)
{
	if (subemitterIndex >= m_emitterResource->subemitters.count) {
		return;
	}

	subemitter_t &subemitter = m_emitterResource->subemitters.items[subemitterIndex];

	dropdown_select_option_by_data(m_dropdownSubemitterResource, subemitter.emitter);
	dropdown_select_option_by_data(m_dropdownSubemitterType, (void *)subemitter.type);
}

void ParticleEditor::RefreshSubemitterList(uint32_t selectSubemitterIndex)
{
	dropdown_clear_options(m_dropdownSubemitter);

	char option[32];

	for (size_t i = 0; i < m_emitterResource->subemitters.count; i++) {

		snprintf(option, sizeof(option), "Subemitter %lu", i + 1);
		dropdown_add_option(m_dropdownSubemitter, option, (void *)i);
	}

	// Select the first subemitter by default.
	if (selectSubemitterIndex < m_emitterResource->subemitters.count) {

		const char *subemitterEffect =
			(m_emitterResource->subemitters.items[selectSubemitterIndex].emitter != nullptr ?
			m_emitterResource->subemitters.items[selectSubemitterIndex].emitter->resource.name : "");

		dropdown_select_option(m_dropdownSubemitterResource, subemitterEffect);
		dropdown_select_option_by_data(m_dropdownSubemitterType,
			(void *)m_emitterResource->subemitters.items[selectSubemitterIndex].type);

		dropdown_select_option_by_data(m_dropdownSubemitter, (void *)(long)selectSubemitterIndex);
	}

	// Display or hide subemitter settings based on whether there are subemitters or not.
	widget_set_visible(m_subemitterContainer, m_emitterResource->subemitters.count != 0);
}

void ParticleEditor::AddSubemitter(void)
{
	// Get currently selected subemitter values and create a new subemitter from them.
	emitter_t *effect =
		(emitter_t *)dropdown_get_selected_option_data(m_dropdownSubemitterResource);

	subemitter_type_t type =
		(subemitter_type_t)(intptr_t)dropdown_get_selected_option_data(m_dropdownSubemitterType);

	arr_push(m_emitterResource->subemitters, create_subemitter(type, effect));

	// Refresh the subemitter list and select the new subemitter.
	RefreshSubemitterList(arr_last_index(m_emitterResource->subemitters));
}

void ParticleEditor::RemoveSubemitter(uint32_t subemitterIndex)
{
	arr_remove_at(m_emitterResource->subemitters, subemitterIndex);

	// Refresh the subemitter list and select the first subemitter.
	RefreshSubemitterList(0);
}

void ParticleEditor::OnAddSubemitterClicked(widget_t *button)
{
	UNUSED(button);

	if (Instance()->m_emitterResource != nullptr) {
		Instance()->AddSubemitter();
	}
}

void ParticleEditor::OnRemoveSubemitterClicked(widget_t *button)
{
	UNUSED(button);

	if (Instance()->m_emitterResource != nullptr) {

		uint32_t index =
			(uint32_t)(intptr_t)dropdown_get_selected_option_data(Instance()->m_dropdownSubemitter);

		Instance()->RemoveSubemitter(index);
	}
}

void ParticleEditor::AddToEffectList(emitter_t *emitter)
{
	if (emitter != NULL) {
		arr_push(Instance()->m_effectList, emitter);
	}
}
