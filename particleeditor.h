#pragma once

#include "editordefs.h"
#include "editorwindow.h"

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
	friend class EditorWindow;
	ParticleEditor(Editor *editor);

	widget_t *AddWidgetRow(widget_t *widget, int16_t height = 25, int16_t padding = 0);

	// TODO: Combine Add*ToLabel methods to one!
	void AddInputsToLabel(widget_t *parentLabel, widget_t *parentPanel, widget_t **outInput,
	                      widget_t **outInput2 = nullptr,
	                      widget_t **outInput3 = nullptr,
	                      widget_t **outInput4 = nullptr);

	void AddColourPickersToLabel(widget_t *parentLabel, widget_t **outCol1, widget_t **outCol2);
	void AddDropdownToLabel(widget_t *parentLabel, widget_t **outDropdown);

	void AddMargin(int16_t amount = 10) { m_nextWidgetOffsetY += amount; }

	float GetInputFloat(widget_t *inputBox);
	uint16_t GetInputUShort(widget_t *inputBox);

	void SetPreviewColour(widget_t *widget, const colour_t &colour);

	void CopyValuesFromEmitter(void);
	void ApplyValuesToEmitter(void);

	void SaveEmitterToFile(void);

	static void OnMenuOpenClicked(widget_t *button);
	static void OnApplyClicked(widget_t *button);
	static void OnSaveClicked(widget_t *button);
	static void OnColourPickerInputEvent(widget_event_t *event);
	static void OnColourSelected(const colour_t &colour, void *context);
	static void OnEmitShapeSelected(widget_t *dropdown, const char *option, void *data);

private:
	static const int16_t LABEL_WIDTH = 120;
	static const int16_t INPUT_MARGIN = 5;

	emitter_t *m_emitter = nullptr; // The edited and displayed particle emitter
	bool m_valuesCopiedFromEmitter = false;

	int16_t m_nextWidgetOffsetY = 0;

	widget_t *m_inputNumParticles = nullptr;
	widget_t *m_inputEmitDuration = nullptr;
	widget_t *m_inputEmitRate = nullptr;
	widget_t *m_inputEmitBurst = nullptr;
	widget_t *m_checkWorldSpace = nullptr;

	widget_t *m_inputLifeMin = nullptr;
	widget_t *m_inputLifeMax = nullptr;
	widget_t *m_inputSpeedMin;
	widget_t *m_inputSpeedMax;
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

	widget_t *m_buttonApply = nullptr;
	widget_t *m_buttonSave = nullptr;

	widget_t *m_checkMoveEffect = nullptr;
};
