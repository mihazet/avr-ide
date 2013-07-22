#include "editorpanel.h"
#include "ideframe.h"
#include "projectmanagment.h"

BEGIN_EVENT_TABLE(EditorPanel, wxPanel)
	EVT_CLOSE(EditorPanel::OnCloseWindow)
END_EVENT_TABLE()

EditorPanel::EditorPanel(	ProjectFile *file, AVRProject *project,
							wxWindow *parent)
	: wxPanel(parent, wxID_ANY)
{
	wxLogVerbose("EditorPanel::EditorPanel");

	m_hasChanged = false;

	m_file = file;
	m_project = project;

	m_stc = new StyledTextCtrl(this);
	m_stc->SetFocus();

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_stc, 1, wxEXPAND);
	SetSizer(sizer);
	Layout();



	m_stc->ClearAll();
	m_stc->LoadFile(m_file->m_fileAbsPath);
	m_stc->EmptyUndoBuffer();

	// настройка редактора
	if (m_file->FileExt() == "cpp" || m_file->FileExt() == "c" || m_file->FileExt() == "cxx"
		|| m_file->FileExt() == "h" || m_file->FileExt() == "hpp")
	{
		m_stc->InitializePrefs("C++");
	}


}

EditorPanel::~EditorPanel()
{
	wxLogVerbose("EditorPanel::~EditorPanel");

}

bool EditorPanel::Save()
{
	if (!m_file->m_fileAbsPath.IsEmpty())
		return Save(m_file->m_fileAbsPath);
	else
		return SaveAs();
}

bool EditorPanel::Save(const wxString& filename)
{
	return m_stc->SaveFile(filename);
}


bool EditorPanel::SaveAs()
{
	wxString filter = wxString::Format(	"%s File (*.%s)|*.%s|Any File (*.*)|*.*",
										m_file->FileExt().c_str(), m_file->FileExt().c_str(),
										m_file->FileExt().c_str());

	wxString filename = wxFileSelector(	"Select a File", m_file->FileDir(), "", m_file->FileExt(),
										filter,	wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	if (!filename.IsEmpty())
	{
		m_file->m_fileAbsPath = filename;
		return Save(filename);
	}
	else
		return false;
}

void EditorPanel::OnCloseWindow(wxCloseEvent& event)
{
	wxLogVerbose("EditorPanel::OnCloseWindow");
	if (HasChanged())
	{
		int ans = wxMessageBox(	"File " + m_file->m_fileAbsPath + " is modified...\n"
								"Do you want to save the changes?",
								"Save file", wxICON_QUESTION|wxYES_NO|wxCANCEL);
		if (ans == wxCANCEL)
		{
			event.Veto();
			return;
		}
		else if (ans == wxYES)
			Save();
	}
}

