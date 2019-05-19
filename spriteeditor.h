#pragma once

#include "editorwindow.h"
#include <mylly/mgui/widget.h>

// -------------------------------------------------------------------------------------------------

class SpriteEditor : public EditorWindow<SpriteEditor>
{
	friend class EditorWindow;

public:
	virtual ~SpriteEditor(void);

	virtual void Process(void) override;
	virtual void SetVisible(bool isVisible) override;

	void SetSpriteSheet(texture_t *spriteSheet);
	
private:
	SpriteEditor(Editor *editor);

	void CreateEditorWindow(void);
	void UpdateView(void);

	void CreateNewSpriteAtCursorPosition(void);
	void SelectSprite(sprite_t *sprite);

	// Returns the sprite rectangle in relation to the screen. Return true when the rectangle is
	// visible, false when it is outside of the viewable panel area.
	bool GetSpriteRectInPanel(sprite_t *sprite, vec2_t &min, vec2_t &max);
	sprite_t *GetSpriteAtCursorPosition(void);

	void HandleInputEvent(widget_event_t *event);
	void HandleDrag(int16_t dx, int16_t dy);
	void HandleZoom(bool zoomIn);

	static void OnClickClose(widget_t *widget);
	static void OnInputEvent(widget_event_t *event);

	const float VIEW_MARGIN = 75;
	const uint8_t MAX_SCALE_LEVEL = 25;

	static constexpr colour_t COL_SELECTED_SPRITE = col(255, 255, 255);
	static constexpr colour_t COL_SPRITE = col(100, 100, 100);

	// Currently edited spritesheet (texture).
	texture_t *m_spriteSheet = nullptr;

	// Pixel scale factor. How many pixels on the panel will one pixel on the texture take up.
	uint8_t m_viewScale = 4;

	// Offset of texture in screen units (not texture pixels - i.e. affected by m_viewScale).
	vec2_t m_viewOffset = vec2_zero();

	// Is the current view dirty, requiring recalculation of positions during the next frame.
	bool m_isViewDirty = false;

	bool m_isDragging = false;
	vec2i_t m_cursor = vec2i_zero();

	sprite_t *m_selectedSprite = nullptr;

	widget_t *m_window = nullptr;
	widget_t *m_title = nullptr;
	widget_t *m_texturePanel = nullptr;

	widget_t *m_paramsPanel = nullptr;
	widget_t *m_inputName = nullptr;
	widget_t *m_inputPosX = nullptr;
	widget_t *m_inputPosY = nullptr;
	widget_t *m_inputWidth = nullptr;
	widget_t *m_inputHeight = nullptr;
};
