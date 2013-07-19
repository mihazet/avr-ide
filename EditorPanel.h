#include <wx/wx.h>

#include "StyledTextCtrl.h"

class ProjectFile;
class AVRProject;

class EditorPanel : public wxPanel
{
public:
	EditorPanel(ProjectFile *file,
				AVRProject *project,
				wxWindow *parent);
	~EditorPanel();

	bool HasChanged() { return m_hasChanged || m_stc->GetModify(); }
	void SetHasChanged(bool value) { m_hasChanged = value; }

	bool Save();
	bool Save(const wxString& filename);
	bool SaveAs();

	StyledTextCtrl *STC() { return m_stc; }

	ProjectFile	*File() const { return m_file; }

private:
	DECLARE_EVENT_TABLE()
	void OnCloseWindow(wxCloseEvent& event);

	ProjectFile	*m_file;
	AVRProject	*m_project;
	bool		m_hasChanged;

	StyledTextCtrl	*m_stc;
};
