#include "ideframe.h"
#include "FileTreePanel.h"
#include "EditorPanel.h"
#include "ProjectManagment.h"
#include "WelcomeWindow.h"
#include "ProjectBuilderBurner.h"
#include "ConfigDialog.h"
#include "SettingsDialog.h"
#include "DefsExt.h"
#include "Error.h"


#include <wx/notebook.h>
#include <wx/fileconf.h>

IMPLEMENT_APP(IDEApp)

bool IDEApp::OnInit()
{
	wxConfigBase::Set(new wxFileConfig("avride", "MZ Home"));

	AVRProject *project = new AVRProject();
	IDEFrame *ide = new IDEFrame(project);
	ide->Show();
	//ide->Maximize();
	return true;
}

BEGIN_EVENT_TABLE(IDEFrame, wxFrame)
	EVT_TREE_ITEM_ACTIVATED(ID_TREE,			IDEFrame::OnTreePanelDClick)
	EVT_CLOSE(									IDEFrame::OnCloseWindow)
	// Aui
	EVT_AUINOTEBOOK_PAGE_CHANGED(ID_AUI_NOTEBOOK,	IDEFrame::OnAuiNotebookPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(ID_AUI_NOTEBOOK,		IDEFrame::OnAuiNotebookCloseButton)
	// Menu Events
	// File
	EVT_MENU(ID_FILE_OPEN_PROJECT,				IDEFrame::OnFileOpenProject)
	EVT_MENU(ID_FILE_SAVE_FILE,					IDEFrame::OnFileSaveFile)
	EVT_MENU(ID_FILE_SAVE_FILE_AS,				IDEFrame::OnFileSaveFileAs)
	EVT_MENU(ID_FILE_SAVE_ALL_FILES,			IDEFrame::OnFileSaveAllFiles)
	EVT_MENU(ID_FILE_SAVE_PROJECT_AS,			IDEFrame::OnFileSaveProjectAs)
	EVT_MENU(wxID_EXIT,							IDEFrame::OnFileExit)
	// Edit
	EVT_MENU(ID_EDIT_UNDO,						IDEFrame::OnEditUndo)
	EVT_MENU(ID_EDIT_REDO,						IDEFrame::OnEditRedo)
	EVT_MENU(ID_EDIT_SELECT_ALL,				IDEFrame::OnEditSelectAll)
	EVT_MENU(ID_EDIT_CUT,						IDEFrame::OnEditCut)
	EVT_MENU(ID_EDIT_COPY,						IDEFrame::OnEditCopy)
	EVT_MENU(ID_EDIT_PASTE,						IDEFrame::OnEditPaste)
	EVT_MENU(ID_EDIT_FIND_AND_REPLACE,			IDEFrame::OnEditFindAndReplace)
	EVT_MENU(ID_EDIT_FIND_NEXT,					IDEFrame::OnEditFindNext)
	EVT_MENU(ID_EDIT_SEARCH_IN_PROJECT,			IDEFrame::OnEditSearchInProject)
	EVT_MENU(ID_EDIT_GOTO_LINE,					IDEFrame::OnEditGotoLine)
	EVT_MENU(ID_EDIT_INDENT,					IDEFrame::OnEditIndent)
	EVT_MENU(ID_EDIT_UNINDENT,					IDEFrame::OnEditUnindent)
	EVT_MENU(ID_EDIT_COMMENT,					IDEFrame::OnEditComment)
	EVT_MENU(ID_EDIT_UNCOMMENT,					IDEFrame::OnEditUncomment)
	EVT_MENU(ID_EDIT_CLEAR_HIGHLIGHT,			IDEFrame::OnEditClearHighLights)
	EVT_MENU(ID_EDIT_TOGGLE_BOOKMARK,			IDEFrame::OnEditToggleBookmark)
	EVT_MENU(ID_EDIT_DELETE_ALL_BOOKMARKS,		IDEFrame::OnEditDeleteAllBookmarks)
	EVT_MENU(ID_EDIT_GOTO_PREV_BOOKMARK,		IDEFrame::OnEditGotoPrevBookmark)
	EVT_MENU(ID_EDIT_GOTO_NEXT_BOOKMARK,		IDEFrame::OnEditGotoNextBookmark)

	// Tools
	EVT_MENU(ID_TOOLS_CONFIGURE_PROJECT,		IDEFrame::OnToolsConfigureProject)
	EVT_MENU(ID_TOOLS_BUILD_PROJECT,			IDEFrame::OnToolsBuildProject)
	EVT_MENU(ID_TOOLS_COMPILE_CURRENT_FILE,		IDEFrame::OnToolsCompileCurrentFile)
	EVT_MENU(ID_TOOLS_PROGRAM_CHIP,				IDEFrame::OnToolsProgramChip)
	EVT_MENU(ID_TOOLS_BUILD_AND_BURN,			IDEFrame::OnToolsBuildnBurn)
	EVT_MENU(ID_TOOLS_AVRDUDE_TOOL,				IDEFrame::OnToolsAVRDUDETool)
	EVT_MENU(ID_TOOLS_FUSE_TOOL,				IDEFrame::OnToolsFuseTool)
	EVT_MENU(ID_TOOLS_EXPORT_MAKEFILE,			IDEFrame::OnToolsExportMakefile)
	EVT_MENU(ID_TOOLS_EXPORT_AVRSTUDIO_APS,		IDEFrame::OnToolsExportAVRStudioAPS)
	EVT_MENU(ID_TOOLS_OPEN_COMMAND_LINE_WINDOW,	IDEFrame::OnToolsOpenCommandLineWindow)
	EVT_MENU(ID_TOOLS_EDITOR_SETTINGS,			IDEFrame::OnToolsEditorSettings)

END_EVENT_TABLE()

IDEFrame::IDEFrame(AVRProject *project)
	: wxFrame(0, wxID_ANY, "AVR IDE")
{
	CreateMenubar();

	//GetNotebook()->SetWindowStyleFlag(wxAUI_NB_TOP|
	//	wxAUI_NB_TAB_MOVE |
	//	wxAUI_NB_SCROLL_BUTTONS |
	//	wxNO_BORDER);

	m_auiNotebook = new wxAuiNotebook(this, ID_AUI_NOTEBOOK);

	m_fileTreePanel = new FileTreePanel(this, ID_TREE);

	m_auiLogBook = new wxAuiNotebook(this, wxID_ANY);

	wxTextCtrl *logText = new wxTextCtrl(	m_auiLogBook, wxID_ANY, "",
											wxDefaultPosition, wxDefaultSize,
											wxTE_MULTILINE|wxTE_READONLY);
	wxLog::SetActiveTarget( new wxLogTextCtrl(logText));
	wxLog::SetVerbose(true);


	m_buildLog = new wxTextCtrl(	m_auiLogBook, wxID_ANY, "",
									wxDefaultPosition, wxDefaultSize,
									wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH2);



	m_buildList = new wxListCtrl(	m_auiLogBook, wxID_ANY,
									wxDefaultPosition, wxDefaultSize,
									wxLC_SINGLE_SEL|wxLC_REPORT);

	wxFont font8(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	m_buildLog->SetFont(font8);
	logText->SetFont(font8);
	m_buildList->SetFont(font8);

	m_auiLogBook->AddPage(logText, "AVR IDE", true);
	m_auiLogBook->AddPage(m_buildLog, "Build log");
	m_auiLogBook->AddPage(m_buildList, "Build messages");


	m_auiManager.SetManagedWindow(this);
	m_auiManager.AddPane(m_fileTreePanel, wxAuiPaneInfo().Left().Caption("Files").MinSize(200, 150).Layer(1));
	m_auiManager.AddPane(m_auiLogBook, wxAuiPaneInfo().Bottom().Caption("Logs & others").MinSize(200, 150).Layer(0));
	m_auiManager.AddPane(m_auiNotebook, wxAuiPaneInfo().CenterPane());
	m_auiManager.Update();

	SetSize(wxSize(800, 600));

	m_project = project;
	m_lastEditor = 0;

	//project->m_fileList["filetreepanel.cpp"] = new ProjectFile( "d:\filetreepanel.cpp", project);
	//project->m_fileList["filetreepanel.h"] = new ProjectFile( "d:\filetreepanel.h", project);

	//m_fileTreePanel->PopulateList(project, &m_editorList);
	//m_fileTreePanel->AddFile("Test.h");
	//m_fileTreePanel->AddFile("Test.cpp");


	//m_project->Save("Test.xml");

	Center();
}

IDEFrame::~IDEFrame()
{
	m_auiManager.UnInit();
	wxConfigBase::Get()->Write("/Test", "Ok");
	delete wxConfigBase::Set(0);
}

void IDEFrame::CreateMenubar()
{
	wxMenu *file_menu = new wxMenu();
	file_menu->Append(ID_FILE_OPEN_PROJECT, "&Open / New Project...\tCtrl-O");
	file_menu->Append(ID_FILE_SAVE_FILE, "&Save File\tCtrl-S");
	file_menu->Append(ID_FILE_SAVE_FILE_AS, "Save File &As...");
	file_menu->Append(ID_FILE_SAVE_ALL_FILES, "Save All &Files");
	file_menu->Append(ID_FILE_SAVE_PROJECT_AS, "Save Project As...");
	file_menu->Append(wxID_EXIT, "E&xit");

	wxMenu *edit_menu = new wxMenu();
	edit_menu->Append(ID_EDIT_UNDO, "Undo\tCtrl-Z");
	edit_menu->Append(ID_EDIT_REDO, "Redo\tCtrl-Y");
	edit_menu->AppendSeparator();
	edit_menu->Append(ID_EDIT_SELECT_ALL, "Select All\tCtrl-A");
	edit_menu->Append(ID_EDIT_CUT, "Cut\tCtrl-X");
	edit_menu->Append(ID_EDIT_COPY, "Copy\tCtrl-C");
	edit_menu->Append(ID_EDIT_PASTE, "Paste\tCtrl-V");
	edit_menu->AppendSeparator();
	edit_menu->Append(ID_EDIT_FIND_AND_REPLACE, "Find and Replace\tCtrl-F");
	edit_menu->Append(ID_EDIT_FIND_NEXT, "Find Next\tF3");
	edit_menu->Append(ID_EDIT_SEARCH_IN_PROJECT, "Search in Project");
	edit_menu->Append(ID_EDIT_GOTO_LINE, "Goto Line\tCtrl-G");
	edit_menu->AppendSeparator();

	wxMenu *edit_menu_block_edit = new wxMenu();
	edit_menu_block_edit->Append(ID_EDIT_INDENT, "Indent");
	edit_menu_block_edit->Append(ID_EDIT_UNINDENT, "Unindent");
	edit_menu_block_edit->Append(ID_EDIT_COMMENT, "Comment");
	edit_menu_block_edit->Append(ID_EDIT_UNCOMMENT, "Uncomment");
	edit_menu->AppendSubMenu(edit_menu_block_edit, "Block Edit");

	edit_menu->Append(ID_EDIT_CLEAR_HIGHLIGHT, "Clear HighLights");

	wxMenu *edit_menu_bookmarks = new wxMenu();
	edit_menu_bookmarks->Append(ID_EDIT_TOGGLE_BOOKMARK, "Toggle Bookmark\tCtrl-F2");
	edit_menu_bookmarks->Append(ID_EDIT_DELETE_ALL_BOOKMARKS, "Delete All Bookmarks\tCtrl-Shift-F2");
	edit_menu_bookmarks->Append(ID_EDIT_GOTO_PREV_BOOKMARK, "Coto Prev Bookmark\tShift-F2");
	edit_menu_bookmarks->Append(ID_EDIT_GOTO_NEXT_BOOKMARK, "Coto Next Bookmark\tF2");
	edit_menu->AppendSubMenu(edit_menu_bookmarks, "Bookmarks");


	wxMenu *tools_menu = new wxMenu();
	tools_menu->Append(ID_TOOLS_CONFIGURE_PROJECT, "Con&figure Project...");
	tools_menu->Append(ID_TOOLS_BUILD_PROJECT, "&Build Project\tF7");
	tools_menu->Append(ID_TOOLS_COMPILE_CURRENT_FILE, "Compile Current File");
	tools_menu->Append(ID_TOOLS_PROGRAM_CHIP, "&Program Chip\tF8");
	tools_menu->Append(ID_TOOLS_BUILD_AND_BURN, "Build and Burn\tF9");
	//tools_menu->Append(ID_TOOLS_AVRDUDE_TOOL, "AVRDUDE Tool");
	//tools_menu->Append(ID_TOOLS_FUSE_TOOL, "Fuse Tool");
	//tools_menu->Append(ID_TOOLS_EXPORT_MAKEFILE, "Export Makefile");
	//tools_menu->Append(ID_TOOLS_EXPORT_AVRSTUDIO_APS, "Export AVRStudio .aps");
	//tools_menu->Append(ID_TOOLS_OPEN_COMMAND_LINE_WINDOW, "Open Command Line Window");
	tools_menu->Append(ID_TOOLS_EDITOR_SETTINGS, "Editor Settings");

	wxMenu *help_menu = new wxMenu();
	help_menu->Append(wxID_ANY, "About");

	wxMenuBar *menu_bar = new wxMenuBar();
	menu_bar->Append(file_menu, "File");
	menu_bar->Append(edit_menu, "Edit");
	menu_bar->Append(tools_menu, "Tools");
	menu_bar->Append(help_menu, "Help");
	SetMenuBar(menu_bar);
}


bool IDEFrame::SaveAll()
{
	wxLogVerbose("IDEFrame::SaveAll");
	if (!m_project->m_isReady)
		return true;

	bool success  = true;

	// iterate over all the elements in the class
	EditorListHash::iterator it;
	for( it = m_editorList.begin(); it != m_editorList.end(); ++it )
	{
		//wxString key = it->first;
		EditorPanel *editor = it->second;
		success &= editor->Save();
	}

	if (!success)
		wxMessageBox("Error While Saving One or More Files");

	return success;
}

bool IDEFrame::SaveProj()
{
	wxLogVerbose("IDEFrame::SaveProj");

	if (!m_project->m_isReady)
		return true;

	if (m_lastEditor)
	{
		m_project->m_lastFile = m_lastEditor->File()->FileName();
	}

	if (m_project->Save())
	{
		return true;
	}
	else
	{
		wxMessageBox("Error While Saving Project Configuration");
		return false;
	}
}

bool IDEFrame::HasChanged()
{
	wxLogVerbose("IDEFrame::HasChanged");

	// iterate over all the elements in the class
	EditorListHash::iterator it;
	for( it = m_editorList.begin(); it != m_editorList.end(); ++it )
	{
		//wxString key = it->first;
		EditorPanel *editor = it->second;
		if (editor->HasChanged())
			return true;
	}
	return false;
}

void IDEFrame::CloseAll()
{
	wxLogVerbose("IDEFrame::CloseAll");

	// iterate over all the elements in the class
	EditorListHash::iterator it;
	for( it = m_editorList.begin(); it != m_editorList.end(); ++it )
	{
		//wxString key = it->first;
		EditorPanel *editor = it->second;
		editor->Close();
		// note that calling close on the editor will trigger an event that removes it from the list
		// that's why it's not removed here
	}
}

void IDEFrame::UpdateUI()
{
	wxLogVerbose("IDEFrame::UpdateUI");

	m_fileTreePanel->PopulateList(m_project, &m_editorList);
	SetTitle( m_project->FileNameNoExt() + " - AVR IDE" );
}

void IDEFrame::GotoEditor(const wxString& filename)
{
	EditorPanel *editor = m_editorList[filename];
	if (!editor)
	{
		editor = OpenEditor(filename);
	}

	if (editor)
	{
		// делаем активным
		int indexPage = m_auiNotebook->GetPageIndex(editor);
		m_auiNotebook->SetSelection(indexPage);
		m_lastEditor = editor;
	}
}

EditorPanel *IDEFrame::OpenEditor(const wxString& filename)
{
	ProjectFile *file = m_project->FileList()[filename];

	if (file)
	{
		// файл в проекте, так что открываем редактор
		EditorPanel *editor = new EditorPanel(file, m_project, m_auiNotebook);
		m_auiNotebook->AddPage(editor, filename);
		// добавляем редактор в список и показываем его
		m_editorList[filename] = editor;

		return editor;
	}

	return 0;
}



// --- Events

void IDEFrame::OnTreePanelDClick(wxTreeEvent& event)
{
	wxLogVerbose("IDEFrame::OnTreePanelDClick");
	wxTreeItemId selId = event.GetItem();
	GotoEditor( m_fileTreePanel->GetItemText(selId) );
}

void IDEFrame::OnCloseWindow(wxCloseEvent& event)
{
	bool exit = true;

	exit &= SaveProj();

	for (size_t i = 0; i < m_auiNotebook->GetPageCount(); i++)
		exit &= wxDynamicCast(m_auiNotebook->GetPage(i), EditorPanel)->Close();

	if (exit)
		event.Skip();
}

// --- Aui events

void IDEFrame::OnAuiNotebookPageChanged(wxAuiNotebookEvent& event)
{
	int sel = event.GetSelection();
	m_lastEditor = (EditorPanel *)m_auiNotebook->GetPage(sel);
}

void IDEFrame::OnAuiNotebookCloseButton(wxAuiNotebookEvent& event)
{
	if (m_lastEditor)
	{
		if (m_lastEditor->Close())
		{
			EditorListHash::iterator it;
			for( it = EditorList().begin(); it != EditorList().end(); ++it )
			{
				if ((it->second) == m_lastEditor)
				{
					EditorList().erase(it);
					break;
				}
			}
			m_lastEditor = 0;
		}
		else
			event.Veto();
	}
}


void IDEFrame::OnFileOpenProject(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnFileOpenProject");
	if (!SaveWarning())
		return;

	AVRProject *newProj = new AVRProject();

	WelcomeWindow newWelcome(newProj, this);
	newWelcome.ShowModal();

	// m_isReady == false means that the user closed
	//the welcome window without opening a project
	if (newProj->m_isReady)
	{
		HandleNewOpenProj(newProj);
	}
}

void IDEFrame::OnFileSaveFile(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnFileSaveFile");
	if (m_lastEditor)
	{
		m_lastEditor->Save();
	}
}

void IDEFrame::OnFileSaveFileAs(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnFileSaveFileAs");
	if (m_lastEditor)
	{
		m_lastEditor->SaveAs();
	}

}

void IDEFrame::OnFileSaveAllFiles(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnFileSaveAllFiles");
	SaveProj();
	SaveAll();
}

void IDEFrame::OnFileSaveProjectAs(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnFileSaveProjectAs");

	wxString filename = wxFileSelector(	"Select a Project File", "", "", "avrproj",
										"AVR Project (*.avrproj)|*.avrproj",
										wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	if (!filename.IsEmpty())
	{
		if (m_project->Save(filename))
		{
			// update ui
			UpdateUI();
		}
		else
			wxMessageBox("Error While Saving Project Configuration File");
	}
}

void IDEFrame::OnFileExit(wxCommandEvent& event)
{
	Close();
}

// --- Events Menu Edit

void IDEFrame::OnEditUndo(wxCommandEvent& event)
{
	if (m_lastEditor)
		m_lastEditor->STC()->Undo();
}

void IDEFrame::OnEditRedo(wxCommandEvent& event)
{
	if (m_lastEditor)
		m_lastEditor->STC()->Redo();
}

void IDEFrame::OnEditSelectAll(wxCommandEvent& event)
{
	if (m_lastEditor)
		m_lastEditor->STC()->SelectAll();
}

void IDEFrame::OnEditCut(wxCommandEvent& event)
{
	if (m_lastEditor)
		m_lastEditor->STC()->Cut();
}

void IDEFrame::OnEditCopy(wxCommandEvent& event)
{
	if (m_lastEditor)
		m_lastEditor->STC()->Copy();
}

void IDEFrame::OnEditPaste(wxCommandEvent& event)
{
	if (m_lastEditor)
		m_lastEditor->STC()->Paste();
}

void IDEFrame::OnEditFindAndReplace(wxCommandEvent& event)
{
	if (m_lastEditor)
		;
}

void IDEFrame::OnEditFindNext(wxCommandEvent& event)
{
	if (m_lastEditor)
		;
}

void IDEFrame::OnEditSearchInProject(wxCommandEvent& event)
{
	if (m_lastEditor)
		;
}

void IDEFrame::OnEditGotoLine(wxCommandEvent& event)
{
	if (m_lastEditor)
		;
}

void IDEFrame::OnEditIndent(wxCommandEvent& event)
{
}

void IDEFrame::OnEditUnindent(wxCommandEvent& event)
{
}

void IDEFrame::OnEditComment(wxCommandEvent& event)
{
	if (m_lastEditor)
		;
}

void IDEFrame::OnEditUncomment(wxCommandEvent& event)
{
	if (m_lastEditor)
		;
}

void IDEFrame::OnEditClearHighLights(wxCommandEvent& event)
{
	if (m_lastEditor)
		;
}

void IDEFrame::OnEditToggleBookmark(wxCommandEvent& event)
{
	if (m_lastEditor)
		;
}

void IDEFrame::OnEditDeleteAllBookmarks(wxCommandEvent& event)
{
}

void IDEFrame::OnEditGotoPrevBookmark(wxCommandEvent& event)
{
}

void IDEFrame::OnEditGotoNextBookmark(wxCommandEvent& event)
{
}



// --- Events Menu Tools

void IDEFrame::OnToolsConfigureProject(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnToolsConfigureProject");

	if (!m_project->m_isReady)
		return;
	ConfigDialog configDialog(m_project, this, "Project Configuration");
	configDialog.ShowModal();
}

void IDEFrame::OnToolsBuildProject(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnToolsBuildProject");
	m_toBurnAfterBuild = false;

	if (!m_project->m_isReady)
		return;

	if (ProjectBuilder::CheckForWinAVR())
	{
		m_auiLogBook->SetSelection( m_auiLogBook->GetPageIndex(m_buildLog) );
		SaveAll();

		ProjectBuilder projBuilder(m_project, m_buildLog,  m_buildList);
		if (!projBuilder.StartBuild())
			m_auiLogBook->SetSelection( m_auiLogBook->GetPageIndex(m_buildList) );
		wxLogVerbose("End");
	}

}

void IDEFrame::OnToolsCompileCurrentFile(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnToolsCompileCurrentFile");
	if (!m_project->IsReady())
		return;

	ProjectFile *file = 0;

	if (!m_lastEditor)
	{
		m_lastEditor->Save();
		file = m_lastEditor->File();
	}

	if (!file)
	{
		if (file->FileExt() == "c" || file->FileExt() == "cpp" || file->FileExt() == "S")
		{
			ProjectBuilder projBuilder(m_project, m_buildLog,  m_buildList);
			if (projBuilder.Compile(file))
			{
				Error::LogMessage(m_buildLog, "Compilation of " + file->FileName() + " Successful");
			}
			else
			{
				Error::LogMessage(m_buildLog, "Compilation of " + file->FileName() + " Failed", cltError);
				m_auiLogBook->SetSelection( m_auiLogBook->GetPageIndex(m_buildList) );
			}

		}
		else
			wxMessageBox("You can only compile C or C++ or S files");
	}
}

void IDEFrame::OnToolsProgramChip(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnToolsProgramChip");

	if (!m_project->IsReady())
		return;

	if (!ProjectBuilder::CheckForWinAVR())
		return;

	//if (serialWin.IsConnected && serialWin.CurrentPort == project.BurnPort)
	//	serialWin.Disconnect();
	ProjectBurner projBurner(m_project);

	projBurner.BurnCMD(false, false, 0);

//	if (serialWin.CurrentPort == project.BurnPort)
//	{
//		serialWin.Activate();
//		serialWin.BringToFront();
//	}
}

void IDEFrame::OnToolsBuildnBurn(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnToolsBuildnBurn");
	OnToolsBuildProject(event);
	OnToolsProgramChip(event);
}

void IDEFrame::OnToolsAVRDUDETool(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnToolsAVRDUDETool");
  /*         if (project.IsReady)
            {
                AvrDudeWindow adw = new AvrDudeWindow(project);
                adw.Show();
            }
 */
}

void IDEFrame::OnToolsFuseTool(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnToolsFuseTool");
  /*         if (project.IsReady)
                new FuseCalculator(project).Show();
 */
}

void IDEFrame::OnToolsExportMakefile(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnToolsExportMakefile");
}

void IDEFrame::OnToolsExportAVRStudioAPS(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnToolsExportAVRStudioAPS");
}

void IDEFrame::OnToolsOpenCommandLineWindow(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnToolsOpenCommandLineWindow");
}

void IDEFrame::OnToolsEditorSettings(wxCommandEvent& event)
{
	wxLogVerbose("IDEFrame::OnToolsEditorSettings");
	SettingsDialog settingsDialog(this);
	settingsDialog.ShowModal();
}

// --- private functions

void IDEFrame::HandleNewOpenProj(AVRProject *newProj)
{
	wxLogVerbose("IDEFrame::HandleNewOpenProj");

	for (size_t i = 0; i < m_auiNotebook->GetPageCount(); i++)
		m_auiNotebook->DeletePage(i);

	AVRProject *keep = m_project;
	m_project = newProj;
	delete keep;
	m_editorList.clear();
	// update ui
	UpdateUI();
}

bool IDEFrame::SaveWarning()
{
	wxLogVerbose("IDEFrame::SaveWarning");

	if (!SaveProj())
	{
		return false;
	}

	if (HasChanged())
	{
		int res = wxMessageBox(	"You have unsaved changes. Do you want to save?",
								"Unsaved Project", wxICON_QUESTION|wxYES_NO|wxCANCEL, this);
		if (res == wxYES)
		{
			return SaveAll();
		}
		else if (res == wxCANCEL)
		{
			return false;
		}
	}
	return true;
}


