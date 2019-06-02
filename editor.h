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

	void Create(void);
	void Process(void);

	void OnSceneLoad(scene_t *scene);
	void OnSceneUnload(void);

	scene_t *GetScene(void) const { return m_currentScene; }
	camera_t *GetCamera(void) const { return scene_get_main_camera(m_currentScene); }

	MenuBar *GetMenuBar(void) const { return m_menuBar; }
	SideBar *GetSideBar(void) const { return m_sideBar; }
	ParticleEditor *GetParticleEditor(void) const { return m_particleEditor; }
	SpriteEditor *GetSpriteEditor(void) const { return m_spriteEditor; }
	SpriteSelector *GetSpriteSelector(void) const { return m_spriteSelector; }
	ColourPicker *GetColourPicker(void) const { return m_colourPicker; }

	bool IsVisible(void) const { return m_isVisible; }
	void SetVisible(bool isVisible);

	vec2_t GetCursorPosition(void) const;
	vec2_t GetCursorWorldPosition(void) const;

	bool WasDoubleClicked(mouse_button_t button);

	//
	// Methods for creating similarly looking widgets.
	//
	widget_t *CreatePanel(widget_t *parent,
	                      anchor_type_t left_type = ANCHOR_MIN, int16_t left_offset = 0,
	                      anchor_type_t right_type = ANCHOR_MAX, int16_t right_offset = 0,
	                      anchor_type_t top_type = ANCHOR_MIN, int16_t top_offset = 0,
	                      anchor_type_t bottom_type = ANCHOR_MAX, int16_t bottom_offset = 0);
	
	widget_t *CreateWindow(const vec2i_t &position, const vec2i_t &size,
	                       widget_t **outTitle = nullptr, widget_t **outCloseButton = nullptr);

	widget_t *CreateButton(widget_t *parent, const char *text,
	                       anchor_type_t left_type = ANCHOR_MIN, int16_t left_offset = 0,
	                       anchor_type_t right_type = ANCHOR_MAX, int16_t right_offset = 0,
	                       anchor_type_t top_type = ANCHOR_MIN, int16_t top_offset = 0,
	                       anchor_type_t bottom_type = ANCHOR_MAX, int16_t bottom_offset = 0);

	widget_t *CreateLabel(widget_t *parent, const char *text,
	                      anchor_type_t left_type = ANCHOR_MIN, int16_t left_offset = 0,
	                      anchor_type_t right_type = ANCHOR_MAX, int16_t right_offset = 0,
	                      anchor_type_t top_type = ANCHOR_MIN, int16_t top_offset = 0,
	                      anchor_type_t bottom_type = ANCHOR_MAX, int16_t bottom_offset = 0);

	widget_t *CreateSmallLabel(widget_t *parent, const char *text,
	                           anchor_type_t left_type = ANCHOR_MIN, int16_t left_offset = 0,
	                           anchor_type_t right_type = ANCHOR_MAX, int16_t right_offset = 0,
	                           anchor_type_t top_type = ANCHOR_MIN, int16_t top_offset = 0,
	                           anchor_type_t bottom_type = ANCHOR_MAX, int16_t bottom_offset = 0);

	widget_t *CreateInputBox(widget_t *parent,
	                         anchor_type_t left_type = ANCHOR_MIN, int16_t left_offset = 0,
	                         anchor_type_t right_type = ANCHOR_MAX, int16_t right_offset = 0,
	                         anchor_type_t top_type = ANCHOR_MIN, int16_t top_offset = 0,
	                         anchor_type_t bottom_type = ANCHOR_MAX, int16_t bottom_offset = 0);

	widget_t *CreateSlider(widget_t *parent, float minValue, float maxValue, int numSteps,
	                       anchor_type_t left_type = ANCHOR_MIN, int16_t left_offset = 0,
	                       anchor_type_t right_type = ANCHOR_MAX, int16_t right_offset = 0,
	                       anchor_type_t top_type = ANCHOR_MIN, int16_t top_offset = 0,
	                       anchor_type_t bottom_type = ANCHOR_MAX, int16_t bottom_offset = 0);

	widget_t *CreateColourPicker(widget_t *parent,
	                             anchor_type_t left_type = ANCHOR_MIN, int16_t left_offset = 0,
	                             anchor_type_t right_type = ANCHOR_MAX, int16_t right_offset = 0,
	                             anchor_type_t top_type = ANCHOR_MIN, int16_t top_offset = 0,
	                             anchor_type_t bottom_type = ANCHOR_MAX, int16_t bottom_offset = 0);

	widget_t *CreateCheckbox(widget_t *parent,
	                         anchor_type_t left_type = ANCHOR_MIN, int16_t left_offset = 0,
	                         anchor_type_t right_type = ANCHOR_MAX, int16_t right_offset = 0,
	                         anchor_type_t top_type = ANCHOR_MIN, int16_t top_offset = 0,
	                         anchor_type_t bottom_type = ANCHOR_MAX, int16_t bottom_offset = 0);

private:
	static void OnCloseButtonClicked(widget_t *closeButton);

private:
	static constexpr float DOUBLE_CLICK_INTERVAL = 0.25f;

	mouse_button_t m_lastMouseButton = MOUSE_NONE;
	float m_lastMouseClickTime = 0;

	scene_t *m_currentScene = nullptr;

	bool m_isVisible = false;

	// Editor windows
	MenuBar *m_menuBar = nullptr;
	SideBar *m_sideBar = nullptr;
	ParticleEditor *m_particleEditor = nullptr;
	SpriteEditor *m_spriteEditor = nullptr;
	SpriteSelector *m_spriteSelector = nullptr;
	ColourPicker *m_colourPicker = nullptr;
};
