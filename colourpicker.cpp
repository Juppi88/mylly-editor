#include "colourpicker.h"
#include "editor.h"
#include <mylly/core/mylly.h>
#include <mylly/mgui/widget.h>
#include <mylly/resources/resources.h>
#include <mylly/renderer/shader.h>

// -------------------------------------------------------------------------------------------------

ColourPicker::ColourPicker(Editor *editor)
	: EditorWindow(editor)
{
}

ColourPicker::~ColourPicker(void)
{
}

void ColourPicker::Create(void)
{
	// Window panel and close button.
	widget_t *closeButton;
	widget_t *panel = GetEditor()->CreateWindow(vec2i(400, 250), vec2i(500, 400),
	                                            &m_title, &closeButton);

	button_set_clicked_handler(closeButton, OnClickClose);

	SetPanel(panel);

	widget_set_text_s(m_title, "Colour Picker");

	// Create the picker widget.
	m_picker = colourpicker_create(panel);

	widget_set_user_context(m_picker, this);
	colourpicker_set_selected_handler(m_picker, OnColourChanged);

	widget_set_anchors(m_picker,
		ANCHOR_MIN, 20,
		ANCHOR_MAX, -20,
		ANCHOR_MIN, 30,
		ANCHOR_MAX, -80
	);

	// Slider/label for brightness.
	m_brightnessSlider = GetEditor()->CreateSlider(m_picker, 0, 100, 1001,
		ANCHOR_MIN, 80,
		ANCHOR_MIDDLE, 0,
		ANCHOR_MAX, 20,
		ANCHOR_MAX, 32
	);

	widget_set_user_context(m_brightnessSlider, this);
	slider_set_value(m_brightnessSlider, 100);
	slider_set_value_changed_handler(m_brightnessSlider, OnBrightnessChanged);

	GetEditor()->CreateSmallLabel(m_brightnessSlider, "Brightness",
		ANCHOR_MIN, -80,
		ANCHOR_MIN, 0,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0
	);

	m_brightnessLabel = GetEditor()->CreateSmallLabel(m_brightnessSlider, "100%",
		ANCHOR_MAX, 5,
		ANCHOR_MAX, 55,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0
	);

	// Slider/label for alpha.
	m_alphaSlider = GetEditor()->CreateSlider(m_picker, 0, 255, 256,
		ANCHOR_MIN, 80,
		ANCHOR_MIDDLE, 0,
		ANCHOR_MAX, 46,
		ANCHOR_MAX, 58
	);

	widget_set_user_context(m_alphaSlider, this);
	slider_set_value(m_alphaSlider, 255);
	slider_set_value_changed_handler(m_alphaSlider, OnAlphaChanged);

	GetEditor()->CreateSmallLabel(m_alphaSlider, "Alpha",
		ANCHOR_MIN, -80,
		ANCHOR_MIN, 0,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0
	);

	m_alphaLabel = GetEditor()->CreateSmallLabel(m_alphaSlider, "255",
		ANCHOR_MAX, 5,
		ANCHOR_MAX, 55,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0
	);

	// Colour result panel.
	m_previewPanel = GetEditor()->CreatePanel(m_picker,
		ANCHOR_MAX, -80,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 20,
		ANCHOR_MAX, 60
	);

	// Set a custom shader to the background panel for rendering the background alpha squares.
	widget_set_custom_shader(m_previewPanel, shader_clone(res_get_shader("default-colour-picker")));

	widget_set_colour(m_previewPanel, COL_WHITE);
	shader_set_uniform_float(m_previewPanel->custom_shader, "Brightness", 1.0f);
	shader_set_uniform_float(m_previewPanel->custom_shader, "Alpha", m_alpha);

	// Preview panel for the previous colour, to be able to compare with the old one.
	m_comparisonPanel = GetEditor()->CreatePanel(m_picker,
		ANCHOR_MAX, -170,
		ANCHOR_MAX, -90,
		ANCHOR_MAX, 20,
		ANCHOR_MAX, 60
	);

	// Set a custom shader to the background panel for rendering the background alpha squares.
	widget_set_custom_shader(m_comparisonPanel, shader_clone(res_get_shader("default-colour-picker")));

	widget_set_colour(m_comparisonPanel, COL_WHITE);
	shader_set_uniform_float(m_comparisonPanel->custom_shader, "Brightness", 1.0f);
	shader_set_uniform_float(m_comparisonPanel->custom_shader, "Alpha", 1.0f);

	// Reset the colour back to original when the user clicks the comparison panel.
	widget_set_input_handler(m_comparisonPanel, OnComparisonPanelInputEvent);
	
	// Hide everything until required.
	SetVisible(false);
}

void ColourPicker::Process(void)
{
	// Update the preview panel's background. This is kind of a hack since we're using a custom
	// shader to render the panel.
	vec2i_t position = widget_get_world_position(m_previewPanel);

	shader_set_uniform_float(m_previewPanel->custom_shader, "WidgetPosX", position.x);
	shader_set_uniform_float(m_previewPanel->custom_shader, "WidgetPosY",
	                         mgui_parameters.height - position.y);
}

void ColourPicker::SelectColour(const colour_t &startColour,
                                OnColourSelected_t callback, void *context)
{
	m_originalColour = startColour;

	m_selectedCallback = callback;
	m_selectedContext = context;

	SetColour(startColour);
	SetVisible(true);

	// Show the old colour on the comparison panel.
	widget_set_colour(m_comparisonPanel, startColour);
	shader_set_uniform_float(m_comparisonPanel->custom_shader, "Alpha", startColour.a / 255.0f);
}

void ColourPicker::SetColour(const colour_t &colour)
{
	// Apply alphaless colour to the preview panel.
	widget_set_colour(m_previewPanel, colour);

	// Update alpha on the preview panel and the alpha slider.
	shader_set_uniform_float(m_previewPanel->custom_shader, "Alpha", colour.a / 255.0f);

	slider_set_value(m_alphaSlider, colour.a);

	// Convert RGB to HSV in order to set the brightness slider.
	float h, s, v;
	col_to_hsv(colour, &h, &s, &v);

	slider_set_value(m_brightnessSlider, v * m_brightnessSlider->slider.max_value);
}

void ColourPicker::SetBrightness(float value)
{
	m_brightness = value / 100.0f;

	widget_set_text(m_brightnessLabel, "%.0f%%", value);
	colourpicker_set_brightness_alpha(m_picker, m_brightness, m_alpha);
}

void ColourPicker::SetAlpha(float value)
{
	m_alpha = value / 255.0f;

	widget_set_text(m_alphaLabel, "%.0f", value);
	colourpicker_set_brightness_alpha(m_picker, m_brightness, m_alpha);
}

void ColourPicker::ResetColour(void)
{
	SetColour(m_originalColour);

	if (m_selectedCallback != nullptr) {
		m_selectedCallback(m_originalColour, m_selectedContext);
	}
}

void ColourPicker::OnColourChanged(widget_t *picker, colour_t colour)
{
	if (picker != nullptr &&
		picker->user_context != nullptr) {

		ColourPicker *self = (ColourPicker *)picker->user_context;
		self->SetColour(colour);

		if (self->m_selectedCallback != nullptr) {
			self->m_selectedCallback(colour, self->m_selectedContext);
		}
	}
}

void ColourPicker::OnBrightnessChanged(widget_t *slider)
{
	if (slider != nullptr &&
		slider->user_context != nullptr) {

		ColourPicker *self = (ColourPicker *)slider->user_context;
		self->SetBrightness(slider_get_value(slider));
	}
}

void ColourPicker::OnAlphaChanged(widget_t *slider)
{
	if (slider != nullptr &&
		slider->user_context != nullptr) {

		ColourPicker *self = (ColourPicker *)slider->user_context;
		self->SetAlpha(slider_get_value(slider));
	}
}

void ColourPicker::OnComparisonPanelInputEvent(widget_event_t *event)
{
	if (event->type == INPUT_MOUSE_BUTTON_DOWN) {
		Instance()->ResetColour();
	}
}

void ColourPicker::OnClickClose(widget_t *button)
{
	UNUSED(button);
	Instance()->SetVisible(false);
}
