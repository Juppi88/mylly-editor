#pragma once

#include "editordefs.h"
#include "editorwindow.h"

// -------------------------------------------------------------------------------------------------

class MenuBar : public EditorWindow<MenuBar>
{
public:
	MenuBar(Editor *editor);
	~MenuBar(void);

	virtual void Create(void) override;
	virtual void Process(void) override;
	virtual void SetVisible(bool isVisible) override;

	void AddButton(const char *text);

private:
	static const uint32_t BUTTON_MARGIN = 6;
	static const uint32_t BUTTON_PADDING = 20;

	widget_t *m_menuBar = nullptr;
	uint32_t m_nextButtonPositionX = BUTTON_MARGIN;
};
