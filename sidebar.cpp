#include "sidebar.h"
#include "menubar.h"
#include "editor.h"
#include <mylly/mgui/widget.h>
#include <mylly/mgui/text.h>

// -------------------------------------------------------------------------------------------------

SideBar::SideBar(Editor *editor) :
	EditorWindow(editor)
{
}

SideBar::~SideBar(void)
{
	// Destroy the main widget. This will destroy all child widgets recursively.
	// Note that this includes all the sub-editors which are children of this panel!
	widget_destroy(GetPanel());
}

void SideBar::Create(void)
{
	// Get the panel of the top menubar and align the sidebar with it.
	widget_t *menuPanel = GetEditor()->GetMenuBar()->GetPanel();
	vec2i_t menuSize = widget_get_size(menuPanel);

	widget_t *panel = GetEditor()->CreatePanel(
		nullptr,
		ANCHOR_MIN, 0,
		ANCHOR_MIN, 300,
		ANCHOR_MIN, menuSize.y,
		ANCHOR_MAX, 0
	);

	SetPanel(panel);

	// Hide the menu bar until needed.
	SetVisible(false);
}

void SideBar::Process(void)
{
}
