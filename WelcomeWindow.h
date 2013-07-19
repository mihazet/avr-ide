#include <wx/wx.h>

class AVRProject;

class WelcomeWindow : public wxDialog
{
public:
	WelcomeWindow(AVRProject *project, wxWindow *parent = 0);

private:
	DECLARE_EVENT_TABLE()
	void OnListDClick(wxCommandEvent& event);
	void OnButtonOpen(wxCommandEvent& event);
	void OnButtonNew(wxCommandEvent& event);
	void OnButtonFind(wxCommandEvent& event);
	void OnButtonBrowse(wxCommandEvent& event);
	void OnButtonNewUpdate(wxUpdateUIEvent& event);
	void OnCloseWindow(wxCloseEvent& event);

	AVRProject	*m_project;
	wxListBox	*m_listRecentFiles;
	wxTextCtrl	*m_projectTitle;
	wxTextCtrl	*m_projectFolder;
};

