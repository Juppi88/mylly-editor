#pragma once

#include "editorwindow.h"
#include <mylly/mgui/widget.h>

// -------------------------------------------------------------------------------------------------

class ColourPicker : public EditorWindow<ColourPicker>
{
public:
	virtual ~ColourPicker(void);

	virtual void Create(void) override;
	virtual void Process(void) override;

	typedef void (*OnColourSelected_t)(const colour_t &colour, void *context);

	void SelectColour(const colour_t &startColour,
	                  OnColourSelected_t callback, void *context = nullptr);

private:
	friend class EditorWindow<ColourPicker>;
	ColourPicker(Editor *editor);

	void SetColour(const colour_t &colour);

	void SetBrightness(float value);
	void SetAlpha(float value);

	void ResetColour(void);

	static void OnColourChanged(widget_t *picker, colour_t colour);
	static void OnBrightnessChanged(widget_t *slider);
	static void OnAlphaChanged(widget_t *slider);
	static void OnComparisonPanelInputEvent(widget_event_t *event);

	static void OnClickClose(widget_t *button);

private:
	widget_t *m_title = nullptr;
	widget_t *m_picker = nullptr;
	widget_t *m_brightnessSlider = nullptr;
	widget_t *m_alphaSlider = nullptr;
	widget_t *m_brightnessLabel = nullptr;
	widget_t *m_alphaLabel = nullptr;
	widget_t *m_previewPanel = nullptr;
	widget_t *m_comparisonPanel = nullptr;

	float m_brightness = 1.0f;
	float m_alpha = 1.0f;

	colour_t m_originalColour = COL_WHITE;
	colour_t m_selectedColour = COL_WHITE;

	OnColourSelected_t m_selectedCallback = nullptr;
	void *m_selectedContext = nullptr;
};
