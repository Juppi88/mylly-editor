#pragma once

#include "editorwindow.h"

// -------------------------------------------------------------------------------------------------

class SpriteSelector : public EditorWindow<SpriteSelector>
{
	friend class EditorWindow;

public:
	virtual ~SpriteSelector(void);

	virtual void Process(void) override {}
	virtual void SetVisible(bool isVisible) override;

	typedef void (*OnSelected_t)(void *context);
	void Select(const char *title, OnSelected_t callback);
	void AddOption(sprite_t *icon, void *context);
	
private:
	SpriteSelector(Editor *editor);

	void CreateEditorWindow(void);

	static void OnClickClose(widget_t *widget);
	static void OnSelected(widget_t *widget);

	widget_t *m_window;
	widget_t *m_grid;
	widget_t *m_gridTitle;

	OnSelected_t m_selectedCallback;
};
