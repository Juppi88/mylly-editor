#pragma once

#include "editordefs.h"
#include <mylly/math/vector.h>
#include <mylly/mgui/widget.h>

// -------------------------------------------------------------------------------------------------

template<class T>
class EditorWindow
{
public:
	static T *Instance(Editor *editor = nullptr)
	{
		static T *m_instance;

		if (m_instance == nullptr) {
			m_instance = new T(editor);
		}

		return m_instance;
	}

	virtual void Create(void) = 0;
	virtual void Process(void) = 0;

	bool IsVisible(void) const { return m_isVisible; }

	virtual void SetVisible(bool isVisible)
	{
		m_isVisible = isVisible;
		widget_set_visible(m_panel, isVisible);
	}

	widget_t *GetPanel(void) const { return m_panel; }

	virtual void OnSceneLoad(scene_t *scene) { UNUSED(scene); }
	virtual void OnSceneUnload(void) {}

protected:
	virtual ~EditorWindow(void) {}

	EditorWindow(Editor *editor) : m_editor(editor) {}
	Editor *GetEditor(void) const { return m_editor; }

	void SetPanel(widget_t *panel) { m_panel = panel; }

private:
	Editor *m_editor = nullptr;
	bool m_isVisible = true;
	widget_t *m_panel = nullptr;
};
