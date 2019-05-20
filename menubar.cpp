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
}

void MenuBar::Create(void)
{
	widget_t *panel = GetEditor()->CreatePanel(
		nullptr,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 0,
		ANCHOR_MIN, 40
	);

	SetPanel(panel);

	// Hide the menu bar until needed.
	SetVisible(false);
}

void MenuBar::Process(void)
{
}

void MenuBar::AddButton(const char *text, OnClickButton_t callback, void *context)
{
	// Use a dummy width until we know the width of the text.
	uint32_t buttonWidth = 100;

	widget_t *button = GetEditor()->CreateButton(GetPanel(), text,
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

	// Add a callback for clicking the button.
	button_set_clicked_handler(button, callback);
	widget_set_user_context(button, context);
}
