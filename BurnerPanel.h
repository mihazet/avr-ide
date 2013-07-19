#include <wx/wx.h>

class AVRProject;
class ProjectBurner;

class BurnerPanel : public wxPanel
{
public:
	BurnerPanel(AVRProject *prj, wxWindow *parent);
	void ProjToForm();
	void FormToProj();
	static void GetPortOverride(wxString& args, AVRProject *project);
	static wxString GetArgs(AVRProject *project);

private:
	DECLARE_EVENT_TABLE()
	void SetButtonText();

	void OnBurnOnlyOpt(wxCommandEvent& event);
	void OnChoicProgSelected(wxCommandEvent& event);
	void OnChoiceSelected(wxCommandEvent& event);

	wxString GetArgs();

	void OnTextChanged(wxCommandEvent& event);

	wxChoice	*m_partChoice;
	wxChoice	*m_progChoice;
	wxTextCtrl	*m_burnOptText;
	wxTextCtrl	*m_portOverrideText;
	wxChoice	*m_baudChoice;
	wxButton	*m_burnOnlyOptButton;

	AVRProject *m_project;
	//ProjectBurner *projBurner;
};
