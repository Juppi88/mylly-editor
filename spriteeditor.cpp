#include "spriteeditor.h"
#include "editor.h"
#include <mylly/core/mylly.h>
#include <mylly/mgui/widget.h>
#include <mylly/resources/resources.h>
#include <mylly/renderer/debug.h>
#include <mylly/renderer/texture.h>
#include <mylly/renderer/mesh.h>
#include <mylly/scene/sprite.h>
#include <mylly/math/math.h>

// -------------------------------------------------------------------------------------------------

constexpr colour_t SpriteEditor::COL_SPRITE;
constexpr colour_t SpriteEditor::COL_SELECTED_SPRITE;

// -------------------------------------------------------------------------------------------------

SpriteEditor::SpriteEditor(Editor *editor)
	: EditorWindow(editor)
{
	m_viewOffset = vec2(-VIEW_MARGIN, -VIEW_MARGIN);

	// Create UI widgets.
	CreateEditorWindow();

	// Hide the widgets until needed.
	SetVisible(false);
}

SpriteEditor::~SpriteEditor(void)
{
	// Destroy the widgets and all its children recursively.
	widget_destroy(m_window);
}

void SpriteEditor::Process(void)
{
	if (m_isViewDirty &&
		m_texturePanel->mesh != nullptr) {

		UpdateView();
		m_isViewDirty = false;
	}

	// Draw sprite boundaries for all visible sprites.
	sprite_t *sprite;

	arr_foreach(m_spriteSheet->sprites, sprite) {

		vec2_t min, max;

		if (GetSpriteRectInPanel(sprite, min, max)) {

			vec3_t centre = vec3((min.x + max.x) / 2, (min.y + max.y) / 2, 0);
			vec2_t extents = vec2((max.x - min.x) / 2, (max.y - min.y) / 2);

			debug_draw_rect(
				centre,
				extents,
				sprite == m_selectedSprite ? COL_SELECTED_SPRITE : COL_SPRITE,
				true
			);
		}
	}
}

void SpriteEditor::SetVisible(bool isVisible)
{
	EditorWindow::SetVisible(isVisible);

	// Toggle UI widget visibility.
	widget_set_visible(m_window, IsVisible());

	// Unselect previously selected sprite in case it no longer exists or is part of another sheet.
	if (isVisible) {
		SelectSprite(nullptr);
	}
}

void SpriteEditor::SetSpriteSheet(texture_t *spriteSheet)
{
	if (spriteSheet == nullptr) {
		return;
	}

	m_spriteSheet = spriteSheet;

	// Get the sprite consisting of the entire texture.
	widget_set_sprite(m_texturePanel, res_get_sprite(m_spriteSheet->resource.res_name));
	m_isViewDirty = true;

	// Update window title.
	widget_set_text(m_title, "Sprite Editor - %s", m_spriteSheet->resource.path);
}

void SpriteEditor::CreateEditorWindow(void)
{
	// Window panel and close button.
	widget_t *closeButton;
	m_window = GetEditor()->CreateWindow(vec2i(550, 250), vec2i(1600, 1000), closeButton);
	widget_set_user_context(closeButton, this);
	button_set_clicked_handler(closeButton, OnClickClose);

	// Title label.
	m_title = GetEditor()->CreateLabel(m_window, "",
		ANCHOR_MIN, 40,
		ANCHOR_MAX, -40,
		ANCHOR_MIN, 0,
		ANCHOR_MIN, 30
	);

	// Texture background panel.
	widget_t *background = GetEditor()->CreatePanel(m_window);

	widget_set_anchors(background,
		ANCHOR_MIN, 5,
		ANCHOR_MAX, -5,
		ANCHOR_MIN, 35,
		ANCHOR_MAX, -5
	);

	// Texture panel.
	m_texturePanel = panel_create(background);

	widget_set_anchors(m_texturePanel,
		ANCHOR_MIN, 5,
		ANCHOR_MAX, -5,
		ANCHOR_MIN, 5,
		ANCHOR_MAX, -5
	);

	// Capture input events in the texture panel.
	widget_set_input_handler(m_texturePanel, OnInputEvent);

	// Background panel for parameters.
	m_paramsPanel = GetEditor()->CreatePanel(background);

	widget_set_anchors(m_paramsPanel,
		ANCHOR_MAX, -200,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, -210,
		ANCHOR_MAX, 0
	);

	// Name label and input box.
	widget_t *nameLabel = GetEditor()->CreateSmallLabel(m_paramsPanel, "Name:",
		ANCHOR_MIN, 5,
		ANCHOR_MAX, -10,
		ANCHOR_MIN, 10,
		ANCHOR_MIN, 35
	);

	m_inputName = GetEditor()->CreateInputBox(nameLabel,
		ANCHOR_MIN, 90,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 2,
		ANCHOR_MAX, -2
	);

	// Position (x) label and input box.
	widget_t *posXLabel = GetEditor()->CreateSmallLabel(nameLabel, "Position (X):",
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 25
	);

	m_inputPosX = GetEditor()->CreateInputBox(posXLabel,
		ANCHOR_MIN, 90,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 2,
		ANCHOR_MAX, -2
	);

	// Position (y) label and input box.
	widget_t *posYLabel = GetEditor()->CreateSmallLabel(posXLabel, "Position (Y):",
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 25
	);

	m_inputPosY = GetEditor()->CreateInputBox(posYLabel,
		ANCHOR_MIN, 90,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 2,
		ANCHOR_MAX, -2
	);

	// Width label and input box.
	widget_t *widthLabel = GetEditor()->CreateSmallLabel(posYLabel, "Width:",
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 25
	);

	m_inputWidth = GetEditor()->CreateInputBox(widthLabel,
		ANCHOR_MIN, 90,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 2,
		ANCHOR_MAX, -2
	);

	// Height label and input box.
	widget_t *heightLabel = GetEditor()->CreateSmallLabel(widthLabel, "Height:",
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 25
	);

	m_inputHeight = GetEditor()->CreateInputBox(heightLabel,
		ANCHOR_MIN, 90,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 2,
		ANCHOR_MAX, -2
	);

	widget_t *instructionLabel1 = GetEditor()->CreateSmallLabel(heightLabel,
		"- Doubleclick to create new",
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 30
	);

	widget_t *instructionLabel2 = GetEditor()->CreateSmallLabel(instructionLabel1,
		"- [M] to move selected sprite",
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 15
	);

	widget_t *instructionLabel3 = GetEditor()->CreateSmallLabel(instructionLabel2,
		"- [R] to resize selected sprite",
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 15
	);

	widget_set_text_alignment(instructionLabel1, ALIGNMENT_BOTTOMLEFT);
	widget_set_text_alignment(instructionLabel2, ALIGNMENT_BOTTOMLEFT);
	widget_set_text_alignment(instructionLabel3, ALIGNMENT_BOTTOMLEFT);

	// Hide the sprite paramerers panel until a sprite is selected.
	widget_set_visible(m_paramsPanel, false);
}

void SpriteEditor::UpdateView(void)
{
	vec2i_t panelSize = widget_get_size(m_texturePanel);
	float scale = 1.0f / m_viewScale;

	// Calculate edges for the current view in relation to the texture.
	float left = m_viewOffset.x * scale;
	float top = m_viewOffset.y * scale;
	float right = (m_viewOffset.x + panelSize.x) * scale;
	float bottom = (m_viewOffset.y + panelSize.y) * scale;

	// Convert these to texture coordinates.
	float u1 = left / m_spriteSheet->width;
	float v1 = top / m_spriteSheet->height;
	float u2 = right / m_spriteSheet->width;
	float v2 = bottom / m_spriteSheet->height;

	panel_set_custom_texture_coord(m_texturePanel, vec2(u1, v1), vec2(u2, v2));
}

void SpriteEditor::CreateNewSpriteAtCursorPosition(void)
{
	sprite_t *sprite = res_add_empty_sprite(m_spriteSheet, "_sprite");

	// Get the position of the texture under the cursor.
	vec2i_t panelSize = widget_get_size(m_texturePanel);
	vec2i_t panelPosition = widget_get_world_position(m_texturePanel);

	// Convert the vertical offset from bottom based to top based.
	float vertOffset = -m_viewOffset.y + ((float)m_spriteSheet->height * m_viewScale - panelSize.y);
	float textureX = floorf((m_viewOffset.x + m_cursor.x - panelPosition.x) / m_viewScale);
	float textureY = floorf((vertOffset + m_cursor.y - panelPosition.y) / m_viewScale);

	// Create position, size and pivot for the sprite.
	vec2_t size = vec2(32, 32);
	vec2_t pivot = vec2_multiply(size, 0.5f); // Use centre pivoting for now
	vec2_t position = vec2(textureX - pivot.x, textureY - pivot.y);

	sprite_set(sprite, m_spriteSheet, position, size, pivot, 20);

	// Select the new sprite.
	SelectSprite(sprite);
}

void SpriteEditor::SelectSprite(sprite_t *sprite)
{
	if (m_selectedSprite == sprite) {
		return;
	}

	m_selectedSprite = sprite;

	// Display or hide the spite info panel.
	widget_set_visible(m_paramsPanel, sprite != nullptr);

	if (sprite == nullptr) {
		return;
	}

	// Update the contents of input boxes.
	widget_set_text(m_inputName, "%s", sprite->resource.name);
	widget_set_text(m_inputPosX, "%d", (int)floorf(sprite->position.x));
	widget_set_text(m_inputPosY, "%d", (int)floorf(sprite->position.y));
	widget_set_text(m_inputWidth, "%d", (int)floorf(sprite->size.x));
	widget_set_text(m_inputHeight, "%d", (int)floorf(sprite->size.y));
}

bool SpriteEditor::GetSpriteRectInPanel(sprite_t *sprite, vec2_t &min, vec2_t &max)
{
	vec2i_t panelPosition = widget_get_world_position(m_texturePanel);
	vec2i_t panelSize = widget_get_size(m_texturePanel);

	// Calculate rectangle min and max coordinates in UI units.
	min = vec2(
		panelPosition.x + m_viewScale * sprite->position.x - m_viewOffset.x,
		panelPosition.y + panelSize.y + m_viewOffset.y
			- m_viewScale * (sprite->texture->height - sprite->position.y)
	);

	max = vec2(
		min.x + m_viewScale * sprite->size.x,
		min.y + m_viewScale * sprite->size.y
	);

	// If the sprite rectangle does not overlap with the texture panel's area, skip the sprite.
	bool overlaps = (
		min.x < panelPosition.x + panelSize.x &&
		max.x > panelPosition.x &&
		min.y < panelPosition.y + panelSize.y &&
		max.y > panelPosition.y
	);

	if (!overlaps) {
		return false;
	}

	// Clamp rectangle coordinates to the panel area.
	if (min.x < panelPosition.x) { min.x = panelPosition.x; }
	if (min.y < panelPosition.y) { min.y = panelPosition.y; }
	if (max.x > panelPosition.x + panelSize.x) { max.x = panelPosition.x + panelSize.x; }
	if (max.y > panelPosition.y + panelSize.y) { max.y = panelPosition.y + panelSize.y; }

	// Flip the Y coordinates around for a coordinate which starts from the bottom of the screen.
	uint16_t screenWidth, screenHeight;
	mylly_get_resolution(&screenWidth, &screenHeight);

	float tmp = min.y;

	min.y = screenHeight - max.y;
	max.y = screenHeight - tmp;

	return true;
}

sprite_t *SpriteEditor::GetSpriteAtCursorPosition(void)
{
	// Get the position of the texture under the cursor.
	vec2i_t panelSize = widget_get_size(m_texturePanel);
	vec2i_t panelPosition = widget_get_world_position(m_texturePanel);

	// Convert the vertical offset from bottom based to top based.
	float vertOffset = -m_viewOffset.y + ((float)m_spriteSheet->height * m_viewScale - panelSize.y);
	float textureX = floorf((m_viewOffset.x + m_cursor.x - panelPosition.x) / m_viewScale);
	float textureY = floorf((vertOffset + m_cursor.y - panelPosition.y) / m_viewScale);

	// Find out whether any of the sprites on the sheet is under the cursor.
	sprite_t *sprite;

	arr_foreach(m_spriteSheet->sprites, sprite) {

		if (textureX >= sprite->position.x &&
			textureY >= sprite->position.y &&
			textureX <= sprite->position.x + sprite->size.x &&
			textureY <= sprite->position.y + sprite->size.y) {

			return sprite;
		}
	}

	return nullptr;
}

void SpriteEditor::HandleInputEvent(widget_event_t *event)
{
	switch (event->type) {

		case INPUT_MOUSE_BUTTON_DOWN:

			if (event->mouse.button == MOUSE_MIDDLE) {

				// Start dragging ith the middle mouse button.
				m_isDragging = true;
			}
			else if (event->mouse.button == MOUSE_LEFT) {

				// Select the sprite clicked by the user.
				SelectSprite(GetSpriteAtCursorPosition());
			}

			m_cursor = vec2i(event->mouse.x, event->mouse.y);
			break;

		case INPUT_MOUSE_BUTTON_UP:

			m_cursor = vec2i(event->mouse.x, event->mouse.y);

			if (event->mouse.button == MOUSE_MIDDLE) {
				m_isDragging = false;
			}
			else if (event->mouse.button == MOUSE_LEFT) {
				
				if (GetEditor()->WasDoubleClicked(MOUSE_LEFT)) {
					CreateNewSpriteAtCursorPosition();
				}
			}

			break;

		case INPUT_MOUSE_MOVE:

			if (m_isDragging) {
				HandleDrag(event->mouse.x - m_cursor.x, event->mouse.y - m_cursor.y);
			}

			m_cursor = vec2i(event->mouse.x, event->mouse.y);
			break;

		case INPUT_MOUSE_WHEEL:

			// Process mouse wheel only if the cursor is on the sheet.
			if (!widget_is_point_inside(m_texturePanel, m_cursor)) {
				break;
			}

			// Use wheel to change scale level.
			if (event->mouse.wheel == MWHEEL_UP && m_viewScale < MAX_SCALE_LEVEL) {
				HandleZoom(true);
			}
			else if (event->mouse.wheel == MWHEEL_DOWN && m_viewScale > 1) {
				HandleZoom(false);
			}
			break;

		default:
			break;
	}
}

void SpriteEditor::HandleDrag(int16_t dx, int16_t dy)
{
	if (m_spriteSheet == nullptr) {
		return;
	}

	m_viewOffset.x -= dx;
	m_viewOffset.y += dy;

	// Clamp the position to the panel.
	/*vec2i_t panelSize = widget_get_size(m_texturePanel);

	int16_t min = -VIEW_MARGIN;
	int16_t maxX = (m_viewScale * m_spriteSheet->width - panelSize.x) + VIEW_MARGIN;
	int16_t maxY = (m_viewScale * m_spriteSheet->height - panelSize.y) + VIEW_MARGIN;

	if (m_viewOffset.x > maxX) m_viewOffset.x = maxX;
	if (m_viewOffset.x < min) m_viewOffset.x = min;
	if (m_viewOffset.y > maxY) m_viewOffset.y = maxY;
	if (m_viewOffset.y < min) m_viewOffset.y = min;*/

	m_isViewDirty = true;
}

void SpriteEditor::HandleZoom(bool zoomIn)
{
	// Get the current centre of the view (in sprite sheet pixels).
	vec2i_t panelSize = widget_get_size(m_texturePanel);

	float centreX = (m_viewOffset.x + 0.5f * panelSize.x) / m_viewScale;
	float centreY = (m_viewOffset.y + 0.5f * panelSize.y) / m_viewScale;

	// Calculate a new view offset based on the centre of the texture and the new scale level.
	m_viewScale += (zoomIn ? 1 : -1);

	m_viewOffset.x = centreX * m_viewScale - 0.5 * panelSize.x;
	m_viewOffset.y = centreY * m_viewScale - 0.5 * panelSize.y;

	m_isViewDirty = true;
}

void SpriteEditor::OnClickClose(widget_t *widget)
{
	UNUSED(widget);
	Instance()->SetVisible(false);
}

void SpriteEditor::OnInputEvent(widget_event_t *event)
{
	Instance()->HandleInputEvent(event);
}
