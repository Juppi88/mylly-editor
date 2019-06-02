#include "editor.h"
#include "menubar.h"
#include "sidebar.h"
#include "particleeditor.h"
#include "spriteeditor.h"
#include "spriteselector.h"
#include "colourpicker.h"
#include <mylly/math/matrix.h>
#include <mylly/scene/camera.h>
#include <mylly/resources/resources.h>
#include <mylly/renderer/shader.h>
#include <mylly/core/time.h>

// -------------------------------------------------------------------------------------------------

Editor::Editor(void)
{
	// Create editor window instances.
	m_menuBar = MenuBar::Instance(this);
	m_sideBar = SideBar::Instance(this);
	m_particleEditor = ParticleEditor::Instance(this);
	m_spriteSelector = SpriteSelector::Instance(this);
	m_spriteEditor = SpriteEditor::Instance(this);
	m_colourPicker = ColourPicker::Instance(this);
}

Editor::~Editor(void)
{
	delete m_particleEditor;
	delete m_spriteEditor;
	delete m_spriteSelector;
	delete m_colourPicker;
	delete m_sideBar;
	delete m_menuBar;
}

void Editor::Create(void)
{
	// Create the menu and side bars.
	m_menuBar->Create();
	m_sideBar->Create();
	m_particleEditor->Create();
	m_spriteSelector->Create();
	m_spriteEditor->Create();
	m_colourPicker->Create();

	// Add a close button to the menubar.
	m_menuBar->AddButton("Close", OnCloseButtonClicked, this);

	m_spriteEditor->SetSpriteSheet(res_get_texture("ui-white"));

	SetVisible(true);
}

void Editor::Process(void)
{
	// TODO: Process all editors here! (add them to a list first)
	if (m_spriteEditor->IsVisible()) {
		m_spriteEditor->Process();
	}

	if (m_colourPicker->IsVisible()) {
		m_colourPicker->Process();
	}

	if (m_particleEditor->IsVisible()) {
		m_particleEditor->Process();
	}
}

void Editor::OnSceneLoad(scene_t *scene)
{
	// TODO: Process all editors here! (add them to a list first)
	m_currentScene = scene;
	m_particleEditor->OnSceneLoad(scene);
}

void Editor::OnSceneUnload(void)
{
	// TODO: Process all editors here! (add them to a list first)
	m_particleEditor->OnSceneUnload();
	m_currentScene = nullptr;
}

void Editor::SetVisible(bool isVisible)
{
	m_isVisible = isVisible;
	
	m_menuBar->SetVisible(isVisible);
	m_sideBar->SetVisible(isVisible);

	if (!isVisible) {

		// Hide all floating windows.
		m_colourPicker->SetVisible(false);
	}
}

vec2_t Editor::GetCursorPosition(void) const
{
	// Get the position of the cursor.
	uint16_t mouseX, mouseY;
	input_get_cursor_position(&mouseX, &mouseY);

	return vec2(
		(float)mouseX,
		(float)mouseY
	);
}

vec2_t Editor::GetCursorWorldPosition(void) const
{
	// Get the position of the cursor.
	vec2_t cursorPosition = GetCursorPosition();
	vec3_t position = vec3(cursorPosition.x, cursorPosition.y, 0);

	// Project the cursor position into the scene.
	camera_t *camera = GetCamera();
	vec3_t projected = camera_screen_to_world(camera, position);

	return vec2(projected.x, projected.y);
}

bool Editor::WasDoubleClicked(mouse_button_t button)
{
	bool doubleClicked = (button == m_lastMouseButton &&
	                      get_time().time - m_lastMouseClickTime < DOUBLE_CLICK_INTERVAL);

	m_lastMouseButton = button;
	m_lastMouseClickTime = get_time().time;

	return doubleClicked;
}

widget_t *Editor::CreatePanel(widget_t *parent,
	                          anchor_type_t left_type, int16_t left_offset,
	                          anchor_type_t right_type, int16_t right_offset,
	                          anchor_type_t top_type, int16_t top_offset,
	                          anchor_type_t bottom_type, int16_t bottom_offset)
{
	widget_t *panel = panel_create(parent);
	widget_set_sprite(panel, res_get_sprite("ui-white/background_white"));
	widget_set_colour(panel, col_a(20, 20, 20, 200));

	widget_set_anchors(panel,
	                   left_type, left_offset, right_type, right_offset,
	                   top_type, top_offset, bottom_type, bottom_offset);

	return panel;
}

widget_t *Editor::CreateWindow(const vec2i_t &position, const vec2i_t &size,
                               widget_t **outTitle, widget_t **outCloseButton)
{
	widget_t *panel = panel_create(nullptr);
	widget_set_sprite(panel, res_get_sprite("ui-white/button01"));
	widget_set_colour(panel, col_a(20, 20, 20, 200));

	widget_set_position(panel, position);
	widget_set_size(panel, size);

	// Create and anchor a title text.
	if (outTitle != nullptr) {

		widget_t *title = label_create(panel);

		widget_set_text_font(title, res_get_font("Louis George Cafe", 14));
		widget_set_text_colour(title, col(220, 220, 220));
		widget_set_text_s(title, "Window");

		widget_set_anchors(title,
			ANCHOR_MIN, 0,
			ANCHOR_MAX, 0,
			ANCHOR_MIN, 0,
			ANCHOR_MIN, 30
		);

		*outTitle = title;
	}

	// Create and anchor the close button.
	if (outCloseButton != nullptr) {

		widget_t *closeButton = button_create(panel);

		widget_set_size(closeButton, vec2i(32, 32));
		widget_set_sprite(closeButton, res_get_sprite("ui-white/button_close"));
		button_set_colours(closeButton, col(120, 200, 80), col(180, 255, 150), col(100, 190, 70));

		widget_set_anchors(closeButton,
			ANCHOR_MAX, -16,
			ANCHOR_NONE, 0,
			ANCHOR_MIN, -16,
			ANCHOR_NONE, 0
		);

		*outCloseButton = closeButton;
	}

	return panel;
}

widget_t *Editor::CreateButton(widget_t *parent, const char *text,
                               anchor_type_t left_type, int16_t left_offset,
                               anchor_type_t right_type, int16_t right_offset,
                               anchor_type_t top_type, int16_t top_offset,
                               anchor_type_t bottom_type, int16_t bottom_offset)
{
	widget_t *button = button_create(parent);

	widget_set_sprite(button, res_get_sprite("ui-white/button02"));
	button_set_colours(button, col(35, 35, 35), col(50, 50, 50), col(20, 20, 20));
	widget_set_text_font(button, res_get_font("Louis George Cafe", 14));
	widget_set_text_colour(button, COL_WHITE);
	widget_set_text_s(button, text);

	widget_set_anchors(button,
	                   left_type, left_offset, right_type, right_offset,
	                   top_type, top_offset, bottom_type, bottom_offset);

	return button;
}

widget_t *Editor::CreateLabel(widget_t *parent, const char *text,
                              anchor_type_t left_type, int16_t left_offset,
                              anchor_type_t right_type, int16_t right_offset,
                              anchor_type_t top_type, int16_t top_offset,
                              anchor_type_t bottom_type, int16_t bottom_offset)
{
	widget_t *label = label_create(parent);

	widget_set_text_font(label, res_get_font("Louis George Cafe", 14));
	widget_set_text_colour(label, col(220, 220, 220));
	widget_set_text_s(label, text);

	widget_set_anchors(label,
	                   left_type, left_offset, right_type, right_offset,
	                   top_type, top_offset, bottom_type, bottom_offset);

	return label;
}

widget_t *Editor::CreateSmallLabel(widget_t *parent, const char *text,
                                   anchor_type_t left_type, int16_t left_offset,
                                   anchor_type_t right_type, int16_t right_offset,
                                   anchor_type_t top_type, int16_t top_offset,
                                   anchor_type_t bottom_type, int16_t bottom_offset)
{
	widget_t *label = label_create(parent);

	widget_set_text_font(label, res_get_font("Louis George Cafe", 12));
	widget_set_text_colour(label, col(220, 220, 220));
	widget_set_text_s(label, text);
	widget_set_text_alignment(label, ALIGNMENT_LEFT);

	widget_set_anchors(label,
	                   left_type, left_offset, right_type, right_offset,
	                   top_type, top_offset, bottom_type, bottom_offset);

	return label;
}

widget_t *Editor::CreateInputBox(widget_t *parent,
	                             anchor_type_t left_type, int16_t left_offset,
	                             anchor_type_t right_type, int16_t right_offset,
	                             anchor_type_t top_type, int16_t top_offset,
	                             anchor_type_t bottom_type, int16_t bottom_offset)
{
	widget_t *input = inputbox_create(parent);

	widget_set_sprite(input, res_get_sprite("ui-white/textbox_02"));
	widget_set_text_font(input, res_get_font("Louis George Cafe", 14));
	widget_set_text_colour(input, COL_BLACK);
	widget_set_text_alignment(input, ALIGNMENT_LEFT);
	//widget_set_text_margin(input, 8, 8, 10, 8);

	widget_set_anchors(input,
	                   left_type, left_offset, right_type, right_offset,
	                   top_type, top_offset, bottom_type, bottom_offset);

	inputbox_set_cursor_sprite(input, res_get_sprite("ui-white/textbox_cursor_02"), 2);

	return input;
}

widget_t *Editor::CreateSlider(widget_t *parent, float minValue, float maxValue, int numSteps,
	                           anchor_type_t left_type, int16_t left_offset,
	                           anchor_type_t right_type, int16_t right_offset,
	                           anchor_type_t top_type, int16_t top_offset,
	                           anchor_type_t bottom_type, int16_t bottom_offset)
{
	widget_t *slider = slider_create(parent);

	widget_set_colour(slider, col(75, 75, 75));
	widget_set_sprite(slider, res_get_sprite("ui-white/slider"));
	slider_set_knob_sprite(slider, res_get_sprite("ui-white/slider_knob"));
	slider_set_knob_colour(slider, col(50, 50, 50));
	slider_set_range(slider, minValue, maxValue);
	slider_set_steps(slider, numSteps);

	widget_set_anchors(slider,
	                   left_type, left_offset, right_type, right_offset,
	                   top_type, top_offset, bottom_type, bottom_offset);

	return slider;
}

widget_t *Editor::CreateColourPicker(widget_t *parent,
                                     anchor_type_t left_type, int16_t left_offset,
                                     anchor_type_t right_type, int16_t right_offset,
                                     anchor_type_t top_type, int16_t top_offset,
                                     anchor_type_t bottom_type, int16_t bottom_offset)
{
	widget_t *panel = panel_create(parent);

	widget_set_sprite(panel, res_get_sprite("ui-white/background_white"));

	widget_set_anchors(panel,
	                   left_type, left_offset, right_type, right_offset,
	                   top_type, top_offset, bottom_type, bottom_offset);

	// Set a custom shader to the background panel for rendering the background alpha squares.
	widget_set_custom_shader(panel, shader_clone(res_get_shader("default-colour-picker")));

	widget_set_colour(panel, COL_WHITE);
	shader_set_uniform_float(panel->custom_shader, "Brightness", 1.0f);
	shader_set_uniform_float(panel->custom_shader, "Alpha", 1.0f);

	vec2i_t position = widget_get_world_position(panel);

	shader_set_uniform_float(panel->custom_shader, "WidgetPosX", position.x);
	shader_set_uniform_float(panel->custom_shader, "WidgetPosY",
	                         mgui_parameters.height - position.y);

	return panel;
}

widget_t *Editor::CreateCheckbox(widget_t *parent,
                                 anchor_type_t left_type, int16_t left_offset,
                                 anchor_type_t right_type, int16_t right_offset,
                                 anchor_type_t top_type, int16_t top_offset,
                                 anchor_type_t bottom_type, int16_t bottom_offset)
{
	widget_t *checkbox = checkbox_create(parent);

	widget_set_sprite(checkbox, res_get_sprite("ui-white/textbox_02"));
	widget_set_colour(checkbox, COL_WHITE);
	checkbox_set_icon(checkbox, res_get_sprite("ui-white/icon_check"));
	checkbox_set_icon_colour(checkbox, col(35, 35, 35));

	widget_set_anchors(checkbox,
	                   left_type, left_offset, right_type, right_offset,
	                   top_type, top_offset, bottom_type, bottom_offset);

	return checkbox;
}

void Editor::OnCloseButtonClicked(widget_t *closeButton)
{
	Editor *editor = (Editor *)closeButton->user_context;
	editor->SetVisible(false);
}
