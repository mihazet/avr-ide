#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/listctrl.h>

class AVRProject;
class FileTreePanel;
class wxTreeEvent;
class EditorPanel;

WX_DECLARE_HASH_MAP(wxString, EditorPanel*, wxStringHash, wxStringEqual, EditorListHash);

class IDEFrame : public wxFrame
{
public:
	IDEFrame(AVRProject *project);
	~IDEFrame();

	void CreateMenubar();

	bool SaveAll();
	bool SaveProj();
	bool HasChanged();
	void CloseAll();

	void UpdateUI();

	void GotoEditor(const wxString& filename);
	EditorPanel *OpenEditor(const wxString& filename);

	EditorListHash& EditorList() { return m_editorList; }

private:
	DECLARE_EVENT_TABLE()
	void OnTreePanelDClick(wxTreeEvent& event);
	void OnCloseWindow(wxCloseEvent& event);
	// Aui NoteBook
	void OnAuiNotebookPageChanged(wxAuiNotebookEvent& event);
	void OnAuiNotebookCloseButton(wxAuiNotebookEvent& event);
	// Menu "File"
	void OnFileNewProject(wxCommandEvent& event);
	void OnFileOpenProject(wxCommandEvent& event);
	void OnFileSaveFile(wxCommandEvent& event);
	void OnFileSaveFileAs(wxCommandEvent& event);
	void OnFileSaveAllFiles(wxCommandEvent& event);
	void OnFileSaveProjectAs(wxCommandEvent& event);
	void OnFileExit(wxCommandEvent& event);
	// Menu "Edit"
	void OnEditUndo(wxCommandEvent& event);
	void OnEditRedo(wxCommandEvent& event);
	void OnEditSelectAll(wxCommandEvent& event);
	void OnEditCut(wxCommandEvent& event);
	void OnEditCopy(wxCommandEvent& event);
	void OnEditPaste(wxCommandEvent& event);
	void OnEditFindAndReplace(wxCommandEvent& event);
	void OnEditFindNext(wxCommandEvent& event);
	void OnEditSearchInProject(wxCommandEvent& event);
	void OnEditGotoLine(wxCommandEvent& event);
	void OnEditIndent(wxCommandEvent& event);
	void OnEditUnindent(wxCommandEvent& event);
	void OnEditComment(wxCommandEvent& event);
	void OnEditUncomment(wxCommandEvent& event);
	void OnEditClearHighLights(wxCommandEvent& event);
	void OnEditToggleBookmark(wxCommandEvent& event);
	void OnEditDeleteAllBookmarks(wxCommandEvent& event);
	void OnEditGotoPrevBookmark(wxCommandEvent& event);
	void OnEditGotoNextBookmark(wxCommandEvent& event);



	// Menu "Tools"
	void OnToolsConfigureProject(wxCommandEvent& event);
	void OnToolsBuildProject(wxCommandEvent& event);
	void OnToolsCompileCurrentFile(wxCommandEvent& event);
	void OnToolsProgramChip(wxCommandEvent& event);
	void OnToolsBuildnBurn(wxCommandEvent& event);
	void OnToolsAVRDUDETool(wxCommandEvent& event);
	void OnToolsFuseTool(wxCommandEvent& event);
	void OnToolsExportMakefile(wxCommandEvent& event);
	void OnToolsExportAVRStudioAPS(wxCommandEvent& event);
	void OnToolsOpenCommandLineWindow(wxCommandEvent& event);
	void OnToolsEditorSettings(wxCommandEvent& event);


	void HandleNewOpenProj(AVRProject *newProj);
	bool SaveWarning();

	wxAuiManager	m_auiManager;
	wxAuiNotebook	*m_auiNotebook;
	wxAuiNotebook 	*m_auiLogBook;
	FileTreePanel	*m_fileTreePanel;
	EditorPanel		*m_lastEditor;
	AVRProject		*m_project;
	EditorListHash	m_editorList;

	// вывод сообщений и ошибок
	// компил€ции и линковки, прошивки
	wxTextCtrl		*m_buildLog;
	wxListCtrl		*m_buildList;

	bool			m_toBurnAfterBuild;
};

class IDEApp : public wxApp
{
	virtual bool OnInit();
};
