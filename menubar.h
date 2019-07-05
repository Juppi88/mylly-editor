#pragma once

#include "editordefs.h"
#include "editorwindow.h"

// -------------------------------------------------------------------------------------------------

class MenuBar : public EditorWindow<MenuBar>
{
public:
	virtual ~MenuBar(void);

	virtual void Create(void) override;
	virtual void Process(void) override;

	typedef void (*OnClickButton_t)(widget_t *widget);
	void AddButton(const char *text, OnClickButton_t callback, void *context = nullptr);

private:
	friend class EditorWindow<MenuBar>;
	MenuBar(Editor *editor);

private:
	static const uint32_t BUTTON_MARGIN = 6;
	static const uint32_t BUTTON_PADDING = 20;

	uint32_t m_nextButtonPositionX = BUTTON_MARGIN;
};
