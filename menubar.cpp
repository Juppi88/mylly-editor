#include "menubar.h"
#include "editor.h"
#include <mylly/mgui/widget.h>
#include <mylly/mgui/text.h>

// -------------------------------------------------------------------------------------------------

MenuBar::MenuBar(Editor *editor) :
	EditorWindow(editor)
{
}

MenuBar::~MenuBar(void)
{
	// Destroy the main widget. This will destroy all child widgets recursivecly.
	widget_destroy(m_menuBar);
}


void MenuBar::Create(void)
{
	m_menuBar = GetEditor()->CreatePanel(
		nullptr,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 0,
		ANCHOR_MIN, 40
	);

	// Hide the menu bar until needed.
	SetVisible(false);

	// TESTING: Add some buttons for testing
	AddButton("Sprite Editor");
	AddButton("Particle Editor");
	AddButton("Close");
}

void MenuBar::Process(void)
{
}

void MenuBar::SetVisible(bool isVisible)
{
	EditorWindow::SetVisible(isVisible);

	// Toggle main widget visibility.
	widget_set_visible(m_menuBar, IsVisible());
}

void MenuBar::AddButton(const char *text)
{
	// Use a dummy width until we know the width of the text.
	uint32_t buttonWidth = 100;

	widget_t *button = GetEditor()->CreateButton(m_menuBar, text,
		ANCHOR_MIN, m_nextButtonPositionX,
		ANCHOR_MIN, m_nextButtonPositionX + buttonWidth,
		ANCHOR_MIN, 5,
		ANCHOR_MAX, -5
	);

	// Update anchors now that the width of the text is known.
	buttonWidth = (uint32_t)text_calculate_width(button->text, -1) + BUTTON_PADDING;

	widget_set_anchors(button,
		ANCHOR_MIN, m_nextButtonPositionX,
		ANCHOR_MIN, m_nextButtonPositionX + buttonWidth,
		ANCHOR_MIN, 5,
		ANCHOR_MAX, -5
	);

	m_nextButtonPositionX += buttonWidth + BUTTON_MARGIN;
}
