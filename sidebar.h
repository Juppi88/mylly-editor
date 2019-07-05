#pragma once

#include "editordefs.h"
#include "editorwindow.h"

// -------------------------------------------------------------------------------------------------

class SideBar : public EditorWindow<SideBar>
{
public:
	virtual ~SideBar(void);

	virtual void Create(void) override;
	virtual void Process(void) override;

private:
	friend class EditorWindow<SideBar>;
	SideBar(Editor *editor);
};
