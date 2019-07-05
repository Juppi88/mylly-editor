#pragma once

#include "editorwindow.h"

// -------------------------------------------------------------------------------------------------

class SpriteSelector : public EditorWindow<SpriteSelector>
{
public:
	virtual ~SpriteSelector(void);

	virtual void Create(void) override;
	virtual void Process(void) override {}

	typedef void (*OnSelected_t)(void *context);
	void Select(const char *title, OnSelected_t callback);
	void AddOption(sprite_t *icon, void *context);
	
private:
	friend class EditorWindow<SpriteSelector>;
	SpriteSelector(Editor *editor);

	void CreateEditorWindow(void);

	static void OnClickClose(widget_t *widget);
	static void OnSelected(widget_t *widget);

	widget_t *m_grid = nullptr;
	widget_t *m_gridTitle = nullptr;

	OnSelected_t m_selectedCallback = nullptr;
};
