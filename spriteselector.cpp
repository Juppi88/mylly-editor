#include "spriteselector.h"
#include "editor.h"
#include <mylly/mgui/widget.h>
#include <mylly/resources/resources.h>
#include <mylly/renderer/texture.h>
#include <mylly/scene/sprite.h>

// -------------------------------------------------------------------------------------------------

SpriteSelector::SpriteSelector(Editor *editor)
	: EditorWindow(editor)
{
	m_selectedCallback = nullptr;

	// Create UI widgets.
	CreateEditorWindow();

	// Hide the widgets until needed.
	SetVisible(false);
}

SpriteSelector::~SpriteSelector(void)
{
	// Destroy the widgets and all its children recursively.
	widget_destroy(m_window);
}

void SpriteSelector::SetVisible(bool isVisible)
{
	EditorWindow::SetVisible(isVisible);

	// Toggle UI widget visibility.
	widget_set_visible(m_window, IsVisible());
}

void SpriteSelector::Select(const char *title, OnSelected_t callback)
{
	// Update title text.
	if (title != nullptr) {
		widget_set_text_s(m_gridTitle, title);
	}

	// Destroy all previous items on the grid.
	widget_t *child, *tmp;

	list_foreach_safe(m_grid->children, child, tmp) {
		widget_destroy(child);
	}

	// Save callback method for selecting an object and display the window.
	m_selectedCallback = callback;

	SetVisible(true);
}

void SpriteSelector::AddOption(sprite_t *icon, void *context)
{
	widget_t *button = button_create(m_grid);

	widget_set_sprite(button, icon);
	button_set_colours(button, col(255, 255, 255), col(200, 200, 200), col(220, 220, 220));
	widget_set_user_context(button, context);
	button_set_clicked_handler(button, OnSelected);

	// Flag the grid for repositioning.
	grid_reposition(m_grid);
}

void SpriteSelector::CreateEditorWindow(void)
{
	// Window panel and close button.
	widget_t *closeButton;
	m_window = GetEditor()->CreateWindow(vec2i(700, 200), vec2i(600, 400), closeButton);
	widget_set_user_context(closeButton, this);
	button_set_clicked_handler(closeButton, OnClickClose);

	// Selection grid title label.
	m_gridTitle = GetEditor()->CreateLabel(m_window, "",
		ANCHOR_MIN, 40,
		ANCHOR_MAX, -40,
		ANCHOR_MIN, 0,
		ANCHOR_MIN, 30
	);

	// Selection grid and background panel.
	widget_t *selectionPanel = GetEditor()->CreatePanel(m_window,
		ANCHOR_MIN, 5,
		ANCHOR_MAX, -5,
		ANCHOR_MIN, 35,
		ANCHOR_MAX, -5
	);

	m_grid = grid_create(selectionPanel);
	grid_set_margin(m_grid, vec2i(10, 10));
	grid_set_item_size(m_grid, vec2i(48, 48));

	widget_set_anchors(m_grid,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0
	);
}

void SpriteSelector::OnClickClose(widget_t *widget)
{
	UNUSED(widget);
	Instance()->SetVisible(false);
}

void SpriteSelector::OnSelected(widget_t *widget)
{
	// Call handler for selected icon.
	if (Instance()->m_selectedCallback != nullptr) {
		Instance()->m_selectedCallback(widget->user_context);
	}
}
