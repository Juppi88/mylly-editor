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

	void AddInputsToLabel(widget_t *parentLabel, widget_t **outInput,
	                      widget_t **outInput2 = nullptr,
	                      widget_t **outInput3 = nullptr,
	                      widget_t **outInput4 = nullptr);

	void AddColourPickersToLabel(widget_t *parentLabel, widget_t **outCol1, widget_t **outCol2);

	void AddMargin(int16_t amount = 10) { m_nextWidgetOffsetY += amount; }

	float GetInputFloat(widget_t *inputBox);
	uint16_t GetInputUShort(widget_t *inputBox);

	void SetPreviewColour(widget_t *widget, const colour_t &colour);

	void CopyValuesFromEmitter(void);
	void ApplyValuesToEmitter(void);

	static void OnMenuOpenClicked(widget_t *button);
	static void OnApplyClicked(widget_t *button);
	static void OnColourPickerInputEvent(widget_event_t *event);
	static void OnColourSelected(const colour_t &colour, void *context);

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
	widget_t *m_inputVelocityMin[3] = { nullptr, nullptr, nullptr };
	widget_t *m_inputVelocityMax[3] = { nullptr, nullptr, nullptr };
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

	colour_t m_startColourMin = COL_WHITE;
	colour_t m_startColourMax = COL_WHITE;
	colour_t m_endColourMin = COL_WHITE;
	colour_t m_endColourMax = COL_WHITE;

	widget_t *m_buttonApply = nullptr;
};
