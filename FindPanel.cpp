#include "FindPanel.h"
#include "DefsExt.h"

FindPanel::FindPanel(wxWindow *parent)
	: wxPanel(parent)
{
	m_findTextCtrl = new wxTextCtrl(this, wxID_ANY, "");
	m_nextButton = new wxButton(this, ID_EDIT_FIND_NEXT, "Next >>");
	m_prevButton = new wxButton(this, ID_EDIT_FIND_PREVIOUS, "<< Previous");

	m_matchCaseCheck = new wxCheckBox(this, wxID_ANY, "Match Case");
	m_wholeWorldCheck = new wxCheckBox(this, wxID_ANY, "Whole World");

	wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(m_findTextCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	mainSizer->Add(m_prevButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	mainSizer->Add(m_nextButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	mainSizer->AddStretchSpacer();
	mainSizer->Add(m_matchCaseCheck, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	mainSizer->Add(m_wholeWorldCheck, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

	SetSizerAndFit(mainSizer);
}
