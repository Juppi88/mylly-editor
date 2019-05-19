#include "editor.h"
#include "spriteeditor.h"
#include "spriteselector.h"
#include <mylly/math/matrix.h>
#include <mylly/scene/camera.h>
#include <mylly/resources/resources.h>
#include <mylly/core/time.h>

// -------------------------------------------------------------------------------------------------

Editor::Editor(void)
{
	// Create editor window instances.
	m_spriteSelector = SpriteSelector::Instance(this);
	m_spriteEditor = SpriteEditor::Instance(this);

	// Display widgets for testing.
	m_spriteSelector->SetVisible(true);
	m_spriteEditor->SetVisible(true);

	m_spriteEditor->SetSpriteSheet(res_get_texture("ui-white"));
}

Editor::~Editor(void)
{
	delete m_spriteEditor;
	delete m_spriteSelector;
}

void Editor::Process(void)
{
	if (m_spriteEditor->IsVisible()) {
		m_spriteEditor->Process();
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

widget_t *Editor::CreatePanel(widget_t *parent)
{
	widget_t *panel = panel_create(parent);
	widget_set_sprite(panel, res_get_sprite("ui-white/button01"));
	widget_set_colour(panel, col(220, 220, 220));

	return panel;
}

widget_t *Editor::CreateWindow(const vec2i_t &position, const vec2i_t &size,
                               widget_t *&outCloseButton)
{
	widget_t *panel = CreatePanel(nullptr);

	widget_set_position(panel, position);
	widget_set_size(panel, size);

	// Create and anchor the close button.
	outCloseButton = button_create(panel);

	widget_set_size(outCloseButton, vec2i(32, 32));
	widget_set_sprite(outCloseButton, res_get_sprite("ui-white/button_close"));
	button_set_colours(outCloseButton, col(120, 200, 80), col(180, 255, 150), col(100, 190, 70));

	widget_set_anchors(outCloseButton,
		ANCHOR_MAX, -16,
		ANCHOR_NONE, 0,
		ANCHOR_MIN, -16,
		ANCHOR_NONE, 0
	);

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
	button_set_colours(button, col(120, 200, 80), col(180, 255, 150), col(100, 190, 70));
	widget_set_text_font(button, res_get_font("ConnectionII", 20));
	widget_set_text_colour(button, COL_BLACK);
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

	widget_set_text_font(label, res_get_font("ConnectionII", 20));
	widget_set_text_colour(label, COL_BLACK);
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

	widget_set_text_font(label, res_get_font("SGK075", 16));
	widget_set_text_colour(label, COL_BLACK);
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
	widget_set_text_font(input, res_get_font("SGK075", 16));
	widget_set_text_colour(input, COL_BLACK);
	widget_set_text_alignment(input, ALIGNMENT_BOTTOMLEFT);
	widget_set_text_margin(input, 8, 8, 10, 8);

	widget_set_anchors(input,
	                   left_type, left_offset, right_type, right_offset,
	                   top_type, top_offset, bottom_type, bottom_offset);

	inputbox_set_cursor_sprite(input, res_get_sprite("ui-white/textbox_cursor_02"), 2);

	return input;
}
