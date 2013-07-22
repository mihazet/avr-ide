#include "WelcomeWindow.h"
#include "ProjectManagment.h"
#include "RecentFiles.h"
#include "DefsExt.h"

BEGIN_EVENT_TABLE(WelcomeWindow, wxDialog)
	EVT_LISTBOX_DCLICK(	ID_LIST,	WelcomeWindow::OnListDClick		)
	EVT_BUTTON(			ID_OPEN,	WelcomeWindow::OnButtonOpen		)
	EVT_BUTTON(			ID_NEW,		WelcomeWindow::OnButtonNew		)
	EVT_BUTTON(			ID_FIND,	WelcomeWindow::OnButtonFind		)
	EVT_BUTTON(			ID_BROWSE,	WelcomeWindow::OnButtonBrowse	)
	EVT_UPDATE_UI(		ID_NEW,		WelcomeWindow::OnButtonNewUpdate)
	EVT_CLOSE(						WelcomeWindow::OnCloseWindow	)
END_EVENT_TABLE()

WelcomeWindow::WelcomeWindow(AVRProject *project, wxWindow *parent)
	: wxDialog(	parent, wxID_ANY, wxString("AVR Project IDE - Welcome"),
				wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
	// create gui
	m_listRecentFiles = new wxListBox(this, ID_LIST, wxDefaultPosition, wxSize(300, 100), 0, 0, wxLB_HSCROLL);
	wxButton *btnOpen = new wxButton(this, ID_OPEN, "Open"/*, wxDefaultPosition, wxSize(100, 100)*/);
	wxButton *btnNew = new wxButton(this, ID_NEW, "New"/*, wxDefaultPosition, wxSize(100, 100)*/);
	wxButton *btnFind = new wxButton(this, ID_FIND, "Find"/*, wxDefaultPosition, wxSize(100, 100)*/);
	wxButton *btnBrowse = new wxButton(this, ID_BROWSE, "...", wxDefaultPosition, wxSize(25, 20));

	m_projectTitle = new wxTextCtrl(this, wxID_ANY, "");
	m_projectFolder = new wxTextCtrl(this, wxID_ANY, wxConfigBase::Get()->Read("/project/last_project_folder", wxGetCwd()));

	wxBoxSizer *btnBrowseSizer = new wxBoxSizer(wxHORIZONTAL);
	btnBrowseSizer->Add(m_projectFolder, 1, wxEXPAND);
	btnBrowseSizer->Add(btnBrowse, 0, wxEXPAND);

	wxBoxSizer *btnSizer = new wxBoxSizer(wxVERTICAL);
	btnSizer->Add(btnOpen, 0, wxEXPAND|wxBOTTOM, 10);
	btnSizer->Add(btnFind, 0, wxEXPAND);

	wxStaticBoxSizer *recentSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Recent Projects");
	recentSizer->Add(btnSizer, 0, wxLEFT|wxRIGHT, 10);
	recentSizer->Add(m_listRecentFiles, 1, wxEXPAND|wxRIGHT|wxBOTTOM, 10);

	wxStaticBoxSizer *newSizer = new wxStaticBoxSizer(wxVERTICAL, this, "New Project");
	newSizer->Add(new wxStaticText(this, wxID_ANY, "Project Title:"), 0, wxLEFT|wxRIGHT, 10);
	newSizer->Add(m_projectTitle, 0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 10);
	newSizer->Add(new wxStaticText(this, wxID_ANY, "Folder to create project in:"), 0, wxLEFT|wxRIGHT, 10);
	newSizer->Add(btnBrowseSizer, 0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 10);
	newSizer->Add(btnNew, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 10);


	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(recentSizer, 1, wxEXPAND|wxALL, 10);
	sizer->Add(newSizer, 0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 10);
	SetSizerAndFit(sizer);

	m_project = project;
	RecentFiles::LoadRecentList();
	RecentFiles::FillListBox(m_listRecentFiles);

	Center();
}

void WelcomeWindow::OnListDClick(wxCommandEvent& event)
{
	if (event.GetSelection() >= 0)
	{
		wxString file = RecentFiles::FilePathFromListBoxIndex(event.GetSelection());
		if (m_project->Open(file))
		{
			RecentFiles::AddFileAsMostRecent(file);
			Close();
		}
	}
	else
	{
		wxMessageBox("Error, Could Not Load Project");
	}
}

void WelcomeWindow::OnButtonOpen(wxCommandEvent& event)
{
	if (m_listRecentFiles->GetSelection() >= 0)
	{
		wxString file = RecentFiles::FilePathFromListBoxIndex(m_listRecentFiles->GetSelection());
		if (m_project->Open(file))
		{
			RecentFiles::AddFileAsMostRecent(file);
			Close();
		}
		else
		{
			wxMessageBox("Error, Could Not Load Project");
		}
	}
	else
	{
		OnButtonFind(event);
	}

}

void WelcomeWindow::OnButtonNew(wxCommandEvent& event)
{
	wxString fullPath = m_projectFolder->GetValue() + wxFILE_SEP_PATH + m_projectTitle->GetValue();
	if (!wxDirExists(fullPath))
		wxMkDir(fullPath);

	fullPath += wxFILE_SEP_PATH + m_projectTitle->GetValue() + ".avrproj";
	wxLogVerbose(fullPath);
	if (m_project->CreateNew(fullPath))
	{
		RecentFiles::AddFileAsMostRecent(m_project->FilePath());
		Close();
	}
	else
	{
		wxMessageBox("Error While Saving Project Configuration");
	}
}

void WelcomeWindow::OnButtonFind(wxCommandEvent& event)
{
	wxString filename = wxFileSelector(	"Select a Project File", "", "", "avrproj",
										"AVR Project (*.avrproj)|*.avrproj", wxFD_OPEN);

	if (!filename.IsEmpty())
	{
		if (m_project->Open(filename))
		{
			RecentFiles::AddFileAsMostRecent(filename);
			Close();
		}
		else
		{
			wxMessageBox("Error, Could Not Load Project");
		}
	}
}

void WelcomeWindow::OnButtonBrowse(wxCommandEvent& event)
{
	wxString dir = wxDirSelector("Select a dir", m_projectFolder->GetValue());

	if (!dir.IsEmpty())
	{
		wxConfigBase::Get()->Write("/project/last_project_folder", dir);
		m_projectFolder->SetValue(dir);
	}
}

void WelcomeWindow::OnButtonNewUpdate(wxUpdateUIEvent& event)
{
	event.Enable( !m_projectTitle->GetValue().IsEmpty() );
}

void WelcomeWindow::OnCloseWindow(wxCloseEvent& event)
{
	if (m_project->m_isReady)
	{
		RecentFiles::SaveRecentList();
	}
	wxLogVerbose("WelcomeWindow::OnCloseWindow");
	EndModal(wxID_OK);
}
