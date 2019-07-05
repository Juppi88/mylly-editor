#pragma once

#include "editordefs.h"
#include "editorwindow.h"
#include <mylly/collections/array.h>

// -------------------------------------------------------------------------------------------------

class ParticleEditor : public EditorWindow<ParticleEditor>
{
public:
	virtual ~ParticleEditor(void);

	virtual void Create(void) override;
	virtual void Process(void) override;

	virtual void OnSceneLoad(scene_t *scene) override;
	virtual void OnSceneUnload(void) override;

private:
	friend class EditorWindow<ParticleEditor>;
	ParticleEditor(Editor *editor);

	widget_t *AddWidgetRow(widget_t *widget, int16_t height = 25, int16_t padding = 0);

	// TODO: Combine Add*ToLabel methods to one!
	void AddInputsToLabel(widget_t *parentLabel, widget_t *parentPanel, widget_t **outInput,
	                      widget_t **outInput2 = nullptr,
	                      widget_t **outInput3 = nullptr,
	                      widget_t **outInput4 = nullptr);

	void AddColourPickersToLabel(widget_t *parentLabel, widget_t **outCol1, widget_t **outCol2);
	void AddDropdownToLabel(widget_t *parentLabel, widget_t *parentPanel, widget_t **outDropdown);
	void AddDropdownWithAddRemoveButtons(widget_t *parentLabel, widget_t **outDropdown,
	                                     widget_t **outAddButton, widget_t **outRemoveButton);

	void AddMargin(int16_t amount = 10) { m_nextWidgetOffsetY += amount; }

	float GetInputFloat(widget_t *inputBox);
	uint16_t GetInputUShort(widget_t *inputBox);

	void SetPreviewColour(widget_t *widget, const colour_t &colour);

	void LoadEmitter(emitter_t *resource, bool start = false);
	void CopyValuesFromEmitter(void);
	void ApplyValuesToEmitter(void);

	void SaveEmitterToFile(void);

	void AddSubemitter(void);
	void RemoveSubemitter(uint32_t subemitterIndex);
	void RefreshSubEmitter(uint32_t subemitterIndex);
	void RefreshSubemitterList(uint32_t selectSubemitterIndex);

	static void OnMenuOpenClicked(widget_t *button);
	static void OnApplyClicked(widget_t *button);
	static void OnSaveClicked(widget_t *button);
	static void OnEffectSelected(widget_t *dropdown, const char *option, void *data);
	static void OnColourPickerInputEvent(widget_event_t *event);
	static void OnColourSelected(const colour_t &colour, void *context);
	static void OnEmitShapeSelected(widget_t *dropdown, const char *option, void *data);

	static void OnSubemitterSelected(widget_t *dropdown, const char *option, void *data);
	static void OnSubemitterEffectSelected(widget_t *dropdown, const char *option, void *data);
	static void OnSubemitterTypeSelected(widget_t *dropdown, const char *option, void *data);
	static void OnAddSubemitterClicked(widget_t *button);
	static void OnRemoveSubemitterClicked(widget_t *button);

	static void AddToEffectList(emitter_t *emitter);

private:
	static const int16_t LABEL_WIDTH = 120;
	static const int16_t INPUT_MARGIN = 5;

	arr_t(emitter_t*) m_effectList = arr_initializer; // A list of all emitter resources

	emitter_t *m_emitter = nullptr; // A reference to the scene emitter
	emitter_t *m_emitterResource = nullptr; // Currently selected emitter resource
	emitter_t *m_loadedEmitterResource = nullptr; // The edited and displayed particle emitter

	int16_t m_nextWidgetOffsetY = 0;

	widget_t *m_dropdownEffect = nullptr;

	widget_t *m_inputNumParticles = nullptr;
	widget_t *m_inputEmitDuration = nullptr;
	widget_t *m_inputEmitRate = nullptr;
	widget_t *m_inputEmitBurst = nullptr;
	widget_t *m_checkWorldSpace = nullptr;

	widget_t *m_inputLifeMin = nullptr;
	widget_t *m_inputLifeMax = nullptr;
	widget_t *m_inputStartSpeedMin = nullptr;
	widget_t *m_inputStartSpeedMax = nullptr;
	widget_t *m_inputEndSpeedMin = nullptr;
	widget_t *m_inputEndSpeedMax = nullptr;
	widget_t *m_inputAccelerationMin[3] = { nullptr, nullptr, nullptr };
	widget_t *m_inputAccelerationMax[3] = { nullptr, nullptr, nullptr };
	widget_t *m_pickerStartColourMin = nullptr;
	widget_t *m_pickerStartColourMax = nullptr;
	widget_t *m_pickerEndColourMin = nullptr;
	widget_t *m_pickerEndColourMax = nullptr;
	widget_t *m_inputStartSizeMin = nullptr;
	widget_t *m_inputStartSizeMax = nullptr;
	widget_t *m_inputEndSizeMin = nullptr;
	widget_t *m_inputEndSizeMax = nullptr;
	widget_t *m_inputRotationSpeedMin = nullptr;
	widget_t *m_inputRotationSpeedMax = nullptr;

	widget_t *m_emitCircleContainer = nullptr;
	widget_t *m_emitBoxContainer = nullptr;
	widget_t *m_emitConeContainer = nullptr;
	widget_t *m_dropdownEmitShape = nullptr;
	widget_t *m_inputEmitPosition[3] = { nullptr, nullptr, nullptr };
	widget_t *m_inputEmitCircleRadius = nullptr;
	widget_t *m_inputEmitBoxExtents[3] = { nullptr, nullptr, nullptr };
	widget_t *m_inputEmitConeAngle = nullptr;
	widget_t *m_inputEmitConeRadius = nullptr;
	widget_t *m_inputEmitConeVolume = nullptr;

	colour_t m_startColourMin = COL_WHITE;
	colour_t m_startColourMax = COL_WHITE;
	colour_t m_endColourMin = COL_WHITE;
	colour_t m_endColourMax = COL_WHITE;

	widget_t *m_subemitterContainer = nullptr;
	widget_t *m_buttonAddSubemitter = nullptr;
	widget_t *m_buttonRemoveSubemitter = nullptr;
	widget_t *m_dropdownSubemitter = nullptr;
	widget_t *m_dropdownSubemitterResource = nullptr;
	widget_t *m_dropdownSubemitterType = nullptr;

	widget_t *m_buttonApply = nullptr;
	widget_t *m_buttonSave = nullptr;

	widget_t *m_checkMoveEffect = nullptr;
	widget_t *m_checkRotateEffect = nullptr;
};
