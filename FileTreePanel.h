#include <wx/wx.h>
#include <wx/treectrl.h>

class AVRProject;
class EditorPanel;
class EditorListHash;


class FileTreePanel : public wxTreeCtrl
{
public:
	FileTreePanel(wxWindow *parent, wxWindowID id);
	void InitializeTree();
	void PopulateList(AVRProject *newProj, EditorListHash *newList);
	bool AddNewFile();
	bool AddExistingFile();
	bool AddFile(const wxString& filePath);

private:
	DECLARE_EVENT_TABLE()
	void OnShowContextMenu(wxContextMenuEvent& event);
	void OnMenu(wxCommandEvent& event);

	void PopulateList();

	wxString GetSaveFileFilters()
	{
		wxString filter = "";
		filter += "C Source Code (*.c)|*.c" "|";
		filter += "CPP Source Code (*.cpp)|*.cpp" "|";
		filter += "Assembly Source Code (*.S)|*.S" "|";
		filter += "Arduino Source Code (*.pde)|*.pde" "|";
		filter += "H Header File (*.h)|*.h" "|";
		filter += "HPP Header File (*.hpp)|*.hpp" "|";
		filter += "Any File (*.*)|*.*";
		return filter;
	}

	wxTreeItemId	m_rootItemId;
	wxTreeItemId	m_sourceItemId;
	wxTreeItemId	m_headerItemId;
	wxTreeItemId	m_otherItemId;

	AVRProject		*m_project;
	EditorListHash	*m_editorList;
};
