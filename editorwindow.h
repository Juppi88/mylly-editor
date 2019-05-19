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

	virtual ~EditorWindow(void) {}
	virtual void Process(void) = 0;

	bool IsVisible(void) const { return m_isVisible; }
	virtual void SetVisible(bool isVisible) { m_isVisible = isVisible; }

protected:
	EditorWindow(Editor *editor) : m_editor(editor), m_isVisible(true) {}
	Editor *GetEditor(void) const { return m_editor; }

private:
	Editor *m_editor;
	bool m_isVisible;
};
