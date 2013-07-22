#include <wx/wx.h>

class FindPanel : public wxPanel
{
public:
	FindPanel(wxWindow *parent);

	wxString FindString() { return m_findTextCtrl->GetValue(); }

	bool MatchCase() { return m_matchCaseCheck->GetValue(); }
	bool WholeWorld() { return m_wholeWorldCheck->GetValue(); }

private:
	wxTextCtrl	*m_findTextCtrl;
	wxButton	*m_prevButton;
	wxButton	*m_nextButton;

	wxCheckBox	*m_matchCaseCheck;
	wxCheckBox	*m_wholeWorldCheck;


};
