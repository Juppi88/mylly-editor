#pragma once

#include "editordefs.h"
#include <mylly/math/vector.h>
#include <mylly/io/input.h>
#include <mylly/mgui/widget.h>
#include <mylly/scene/scene.h>

// -------------------------------------------------------------------------------------------------

class Editor
{
public:
	Editor(void);
	~Editor(void);

	void Process(void);

	scene_t *GetScene(void) const { return m_currentScene; }
	camera_t *GetCamera(void) const { return scene_get_main_camera(m_currentScene); }

	SpriteEditor *GetSpriteEditor(void) const { return m_spriteEditor; }
	SpriteSelector *GetSpriteSelector(void) const { return m_spriteSelector; }

	vec2_t GetCursorPosition(void) const;
	vec2_t GetCursorWorldPosition(void) const;

	bool WasDoubleClicked(mouse_button_t button);

	// Methods for creating similarly looking widgets.
	widget_t *CreatePanel(widget_t *parent);
	widget_t *CreateWindow(const vec2i_t &position, const vec2i_t &size, widget_t *&outCloseButton);

	widget_t *CreateButton(widget_t *parent, const char *text,
	                       anchor_type_t left_type, int16_t left_offset,
	                       anchor_type_t right_type, int16_t right_offset,
	                       anchor_type_t top_type, int16_t top_offset,
	                       anchor_type_t bottom_type, int16_t bottom_offset);

	widget_t *CreateLabel(widget_t *parent, const char *text,
	                      anchor_type_t left_type, int16_t left_offset,
	                      anchor_type_t right_type, int16_t right_offset,
	                      anchor_type_t top_type, int16_t top_offset,
	                      anchor_type_t bottom_type, int16_t bottom_offset);

	widget_t *CreateSmallLabel(widget_t *parent, const char *text,
	                           anchor_type_t left_type, int16_t left_offset,
	                           anchor_type_t right_type, int16_t right_offset,
	                           anchor_type_t top_type, int16_t top_offset,
	                           anchor_type_t bottom_type, int16_t bottom_offset);

	widget_t *CreateInputBox(widget_t *parent,
	                         anchor_type_t left_type, int16_t left_offset,
	                         anchor_type_t right_type, int16_t right_offset,
	                         anchor_type_t top_type, int16_t top_offset,
	                         anchor_type_t bottom_type, int16_t bottom_offset);

private:
	static constexpr float DOUBLE_CLICK_INTERVAL = 0.25f;

	mouse_button_t m_lastMouseButton;
	float m_lastMouseClickTime;

	scene_t *m_currentScene = nullptr;

	// Editor windows
	SpriteEditor *m_spriteEditor;
	SpriteSelector *m_spriteSelector;
};
