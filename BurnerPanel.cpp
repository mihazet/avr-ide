#include "BurnerPanel.h"
#include "ProjectManagment.h"
#include "ProjectBuilderBurner.h"
#include "DefsExt.h"


BEGIN_EVENT_TABLE(BurnerPanel, wxPanel)
	EVT_BUTTON(ID_BURN_ONLY_OPT,		BurnerPanel::OnBurnOnlyOpt)
	EVT_CHOICE(ID_DROP_PART_SELECTED,	BurnerPanel::OnChoiceSelected)
	EVT_CHOICE(ID_DROP_PROG_SELECTED,	BurnerPanel::OnChoiceSelected)
	//EVT_CHOICE(ID_DROP_PROG_SELECTED,	BurnerPanel::OnChoicProgSelected)	// ???
	EVT_CHOICE(ID_DROP_BAUD_SELECTED,	BurnerPanel::OnChoiceSelected)
	EVT_TEXT(ID_TEXT_CHANGE1,			BurnerPanel::OnTextChanged)
	EVT_TEXT(ID_TEXT_CHANGE2,			BurnerPanel::OnTextChanged)
END_EVENT_TABLE()

BurnerPanel::BurnerPanel(AVRProject *prj, wxWindow *parent)
	: wxPanel(parent)
{
	m_project = prj;

	m_partChoice = new wxChoice(this, ID_DROP_PART_SELECTED);
	m_progChoice = new wxChoice(this, ID_DROP_PROG_SELECTED);
	m_burnOptText = new wxTextCtrl(this, ID_TEXT_CHANGE1, "");
	m_portOverrideText = new wxTextCtrl(this, ID_TEXT_CHANGE2, "");
	m_baudChoice = new wxChoice(this, ID_DROP_BAUD_SELECTED);
	m_burnOnlyOptButton = new wxButton(this, ID_BURN_ONLY_OPT, "Run");

	m_baudChoice->Append("No Override");
	m_baudChoice->Append("300");
	m_baudChoice->Append("600");
	m_baudChoice->Append("1200");
	m_baudChoice->Append("2400");
	m_baudChoice->Append("4800");
	m_baudChoice->Append("9600");
	m_baudChoice->Append("14400");
	m_baudChoice->Append("19200");
	m_baudChoice->Append("28800");
	m_baudChoice->Append("38400");
	m_baudChoice->Append("57600");
	m_baudChoice->Append("76800");
	m_baudChoice->Append("115200");
	m_baudChoice->Append("230400");

	wxGridSizer *szrGrid = new wxGridSizer(4, 5, 5);
	szrGrid->Add(new wxStaticText(this, wxID_ANY, "Part:"), 0, wxALIGN_RIGHT);
	szrGrid->Add(m_partChoice, 1, wxEXPAND);
	szrGrid->Add(new wxStaticText(this, wxID_ANY, "Port Override:"), 0, wxALIGN_RIGHT);
	szrGrid->Add(m_portOverrideText, 1, wxEXPAND);
	szrGrid->Add(new wxStaticText(this, wxID_ANY, "Programmer:"), 0, wxALIGN_RIGHT);
	szrGrid->Add(m_progChoice, 1, wxEXPAND);
	szrGrid->Add(new wxStaticText(this, wxID_ANY, "Baud Rate Override:"), 0, wxALIGN_RIGHT);
	szrGrid->Add(m_baudChoice, 1, wxEXPAND);
	szrGrid->Add(new wxStaticText(this, wxID_ANY, "Options:"), 0, wxALIGN_RIGHT);
	szrGrid->Add(m_burnOptText, 1, wxEXPAND);
	szrGrid->Add(new wxPanel(this), 1, wxEXPAND);
	szrGrid->Add(new wxPanel(this), 1, wxEXPAND);

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(szrGrid, 1, wxEXPAND|wxALL, 5);
	sizer->Add(m_burnOnlyOptButton, 0, wxEXPAND|wxALL, 5);
	SetSizerAndFit(sizer);


	//projBurner = new ProjectBurner(m_project);
	m_progChoice->Clear();
	m_progChoice->Append(ProjectBurner::GetAvailProgrammers(true));

	m_partChoice->Clear();
	if (m_progChoice->GetCount() > 0)
		m_partChoice->Append(ProjectBurner::GetAvailParts(m_progChoice->GetString(0), true));

	m_baudChoice->SetSelection(0);

	ProjToForm();
}

void BurnerPanel::ProjToForm()
{
	if (m_partChoice->GetCount() > 0)
	{
		m_partChoice->SetSelection(0);
		if (m_partChoice->FindString(m_project->m_burnPart) != wxNOT_FOUND)
			m_partChoice->SetSelection( m_partChoice->FindString(m_project->m_burnPart) );
		else
			m_partChoice->SetSelection( m_partChoice->Append(m_project->m_burnPart) );
	}
	else
		m_partChoice->SetSelection( m_partChoice->Append(m_project->m_burnPart) );

	if (m_progChoice->GetCount() > 0)
	{
		m_progChoice->SetSelection(0);
		if (m_progChoice->FindString(m_project->m_burnProgrammer) != wxNOT_FOUND)
			m_progChoice->SetSelection( m_progChoice->FindString(m_project->m_burnProgrammer) );
		else
			m_progChoice->SetSelection( m_progChoice->Append(m_project->m_burnProgrammer) );
	}
	else
		m_progChoice->SetSelection( m_progChoice->Append(m_project->m_burnProgrammer) );

	m_portOverrideText->SetValue( m_project->m_burnPort );

	m_baudChoice->SetSelection(0);
	if (m_baudChoice->FindString(wxString::Format("%d", m_project->m_burnBaud)) != wxNOT_FOUND)
	{
		m_baudChoice->SetSelection( m_baudChoice->FindString(wxString::Format("%d", m_project->m_burnBaud)));
	}
	else
	{
		if (m_project->m_burnBaud != 0)
		{
			int i = m_baudChoice->Append(wxString::Format("%d", m_project->m_burnBaud));
			m_baudChoice->SetSelection(i);
		}
	}

	m_burnOptText->SetValue( m_project->m_burnOptions );
	//chkAutoReset.Checked = m_project.BurnAutoReset;

	SetButtonText();
}

void BurnerPanel::FormToProj()
{
	m_project->m_burnOptions = m_burnOptText->GetValue();
	m_project->m_burnPart = m_partChoice->GetStringSelection();
	m_project->m_burnProgrammer = m_progChoice->GetStringSelection();
	//m_project->m_burnAutoReset = chkAutoReset.Checked;

	long baud = 0;
	wxString selectedText = m_baudChoice->GetStringSelection();
	if (selectedText.ToLong(&baud))
	{
		m_project->m_burnBaud = baud;
	}
	else
	{
		m_project->m_burnBaud = 0;
	}

	m_project->m_burnPort = m_portOverrideText->GetValue();
}

void BurnerPanel::OnBurnOnlyOpt(wxCommandEvent& event)
{
	FormToProj();
	ProjectBurner projBurner(m_project);
	projBurner.BurnCMD(true, false, this);
}

void BurnerPanel::OnChoicProgSelected(wxCommandEvent& event)
{
	if (m_progChoice->GetSelection() < 0)
		return;

	wxString prog = m_progChoice->GetStringSelection();

	if (prog == "avrdoper")
	{
		if (m_portOverrideText->GetValue().IsEmpty())
			m_portOverrideText->SetValue("avrdoper");
	}
}

void BurnerPanel::GetPortOverride(wxString& args, AVRProject *m_project)
{
	wxString res = "";

	if (!m_project->m_burnPort.IsEmpty())
	{
		res = "-P " + m_project->m_burnPort;
		if (m_project->m_burnPort.Len() > 3)
		{
			long portNum;
			if (m_project->m_burnPort.StartsWith("COM") && m_project->m_burnPort.Mid(3).ToLong(&portNum))
			{
				res = "-P //./" + m_project->m_burnPort;
			}
		}
	}

	args += res;

	res = "";

	if (m_project->m_burnBaud != 0)
		res = " -b " + wxString::Format("%d", m_project->m_burnBaud);

	args += res;
	args = args.Trim();
}

wxString BurnerPanel::GetArgs()
{
	return BurnerPanel::GetArgs(m_project);
}

wxString BurnerPanel::GetArgs(AVRProject *m_project)
{
	wxString overrides = "";
	BurnerPanel::GetPortOverride(overrides, m_project);
	wxString res = wxString::Format("-p %s -c %s %s %s",
									m_project->m_burnPart.Upper().c_str(),
									m_project->m_burnProgrammer.Upper().c_str(),
									overrides.c_str(),
									m_project->m_burnOptions.c_str());

	while (res.Find("  ") != wxNOT_FOUND)
		res.Replace("  ", " ");

	return res;
}

void BurnerPanel::OnTextChanged(wxCommandEvent& event)
{
	FormToProj();
	SetButtonText();
}

void BurnerPanel::SetButtonText()
{
	m_burnOnlyOptButton->SetLabel( "Run '" + BurnerPanel::GetArgs(m_project) + "'" );
}

void BurnerPanel::OnChoiceSelected(wxCommandEvent& event)
{
	OnTextChanged(event);
}


/*
        private void m_portOverrideText_Validating(object sender, CancelEventArgs e)
        {
            if (m_portOverrideText.Text.Length > 3)
            {
                int portNum;
                if (m_portOverrideText.Text.ToUpperInvariant().StartsWith("COM") && int.TryParse(m_portOverrideText.Text.Substring(3), out portNum))
                {
                    m_portOverrideText.Text = "COM" + portNum.ToString("0");
                }
            }
        }
*/

