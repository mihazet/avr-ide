#include "FileTreePanel.h"
#include "ProjectManagment.h"
#include "IDEFrame.h"
#include "DefsExt.h"

#include <wx/wfstream.h>



BEGIN_EVENT_TABLE(FileTreePanel, wxTreeCtrl)
	EVT_CONTEXT_MENU(						FileTreePanel::OnShowContextMenu	)
	EVT_MENU(	ID_POPUP_ADD_NEW_FILE,		FileTreePanel::OnMenu				)
	EVT_MENU(	ID_POPUP_ADD_EXISTING_FILE,	FileTreePanel::OnMenu				)
END_EVENT_TABLE()


FileTreePanel::FileTreePanel(wxWindow *parent, wxWindowID id)
	: wxTreeCtrl(parent, id)
{
	InitializeTree();
}

void FileTreePanel::InitializeTree()
{
	DeleteAllItems();
	m_rootItemId = AddRoot("Project");
	m_sourceItemId = AppendItem(m_rootItemId, "Source Files (c, cpp, cxx, S)");
	m_headerItemId = AppendItem(m_rootItemId, "Header Files (h, hpp)");
	m_otherItemId = AppendItem(m_rootItemId, "Other Files");
}

void FileTreePanel::PopulateList(AVRProject *newProj, EditorListHash *newList)
{
	m_project = newProj;
	m_editorList = newList;
	PopulateList();
}

bool FileTreePanel::AddNewFile()
{
	wxString filename = wxFileSelector(	"Add New File", "", "", "c",
										GetSaveFileFilters(),
										wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	if (!filename.IsEmpty())
		return AddFile(filename);
	return false;
}

bool FileTreePanel::AddExistingFile()
{
	wxString filter = "Code/Header Files (*.c;*.cpp;*.S;*.pde;*.h;*.hpp)|*.c;*.cpp;*.S;*.pde;*.h;*.hpp"  "|";
	filter += GetSaveFileFilters();

	wxString filename = wxFileSelector(	"Add New File", "", "", "c",
										filter, wxFD_OPEN);
	if (!filename.IsEmpty())
		return AddFile(filename);
	return false;
}

bool FileTreePanel::AddFile(const wxString& filePath)
{
	wxString fn = wxFileName(filePath).GetFullName();
	wxString ext = wxFileName(filePath).GetExt();

	ProjectFile *file = m_project->FileList()[fn];

	if (file)
	{
		if (file->m_fileAbsPath != filePath && file->Exists())
		{
			wxMessageBox("Error, Cannot Add File " + file->FileName() + " Due To Name Conflict");
		}
		else
		{
			PopulateList();
		}
		return false;
	}
	else
	{
		file = new ProjectFile(filePath, m_project);
		if (!file->Exists())
		{
			// создать файл на диске
			wxFileOutputStream newFile(filePath);

		}
		m_project->FileList()[fn] = file;
		PopulateList();
		return true;
	}

}

// --- events
void FileTreePanel::OnShowContextMenu(wxContextMenuEvent& event)
{
	wxLogVerbose("FileTreePanel::OnShowContextMenu");
	wxPoint pt = ScreenToClient(event.GetPosition());
	wxTreeItemId hitId = HitTest(pt);
	if (hitId.IsOk())
	{
		SelectItem(hitId);
	}

	wxMenu popupMenu;
	popupMenu.Append(ID_POPUP_ADD_NEW_FILE,			"Add New File..."		);
	popupMenu.Append(ID_POPUP_ADD_EXISTING_FILE,	"Add Existing File..."	);

	PopupMenu(&popupMenu, pt);
}

void FileTreePanel::OnMenu(wxCommandEvent& event)
{
	switch (event.GetId())
	{
		case ID_POPUP_ADD_NEW_FILE:
			if (m_project == 0)
				return;
			if (m_project->m_isReady == false)
				return;

			if (AddNewFile())
			{
				// TODO
				// открыть файл
			}
			break;

		case ID_POPUP_ADD_EXISTING_FILE:
			if (m_project == 0)
				return;
			if (m_project->m_isReady == false)
				return;

			if (AddExistingFile())
			{
				// TODO
				// открыть файл
			}
			break;
	}
}

// --- private functions
void FileTreePanel::PopulateList()
{
	SetItemText(m_rootItemId, m_project->FileName());

	DeleteChildren(m_sourceItemId);
	DeleteChildren(m_headerItemId);
	DeleteChildren(m_otherItemId);

	// iterate over all the elements in the class
	ProjectFileHash::iterator it;
	for( it = m_project->FileList().begin(); it != m_project->FileList().end(); ++it )
	{
		wxString key = it->first;
		ProjectFile *value = it->second;
		// do something useful with key and value
		if (value->IsSource())
			AppendItem(m_sourceItemId, key);
		else if (value->FileExt() == "h" || value->FileExt() == "hpp")
			AppendItem(m_headerItemId, key);
		else
			AppendItem(m_otherItemId, key);

	}
	ExpandAll();
}
