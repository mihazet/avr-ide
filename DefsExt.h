#ifndef DEFSEXT_H
#define	DEFSEXT_H

// --------------------- Project ----------------------------------------------

// --- Memeory Segment

struct MemorySegment
{
	wxString type;
	wxString name;
	unsigned int addr;

	MemorySegment(wxString t, wxString n, unsigned int a)
	{
		type = t;
		name = n;
		addr = a;
	}
};

WX_DECLARE_HASH_MAP(wxString, MemorySegment*, wxStringHash, wxStringEqual, MemorySegmentHash);

// --------------------- STC --------------------------------------------------



// --------------------- Common -----------------------------------------------

// ----------------------------------------------------------------------------
// --- ID's
// ----------------------------------------------------------------------------

enum
{
	// --- IDEFrame
	// Controls
	ID_TREE = wxID_HIGHEST,
	ID_AUI_NOTEBOOK,
	// Menu File
	ID_FILE_OPEN_PROJECT,
	ID_FILE_SAVE_FILE,
	ID_FILE_SAVE_FILE_AS,
	ID_FILE_SAVE_ALL_FILES,
	ID_FILE_SAVE_PROJECT_AS,
	// Menu Edit
	ID_EDIT_UNDO,
	ID_EDIT_REDO,
	ID_EDIT_SELECT_ALL,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
	ID_EDIT_FIND_AND_REPLACE,
	ID_EDIT_FIND_NEXT,
	ID_EDIT_SEARCH_IN_PROJECT,
	ID_EDIT_GOTO_LINE,
	ID_EDIT_INDENT,
	ID_EDIT_UNINDENT,
	ID_EDIT_COMMENT,
	ID_EDIT_UNCOMMENT,
	ID_EDIT_CLEAR_HIGHLIGHT,
	ID_EDIT_TOGGLE_BOOKMARK,
	ID_EDIT_DELETE_ALL_BOOKMARKS,
	ID_EDIT_GOTO_PREV_BOOKMARK,
	ID_EDIT_GOTO_NEXT_BOOKMARK,
	// Menu Tools
	ID_TOOLS_CONFIGURE_PROJECT,
	ID_TOOLS_BUILD_PROJECT,
	ID_TOOLS_COMPILE_CURRENT_FILE,
	ID_TOOLS_PROGRAM_CHIP,
	ID_TOOLS_BUILD_AND_BURN,
	ID_TOOLS_AVRDUDE_TOOL,
	ID_TOOLS_FUSE_TOOL,
	ID_TOOLS_EXPORT_MAKEFILE,
	ID_TOOLS_EXPORT_AVRSTUDIO_APS,
	ID_TOOLS_OPEN_COMMAND_LINE_WINDOW,
	ID_TOOLS_EDITOR_SETTINGS,

	// --- BurnerPanel
	ID_BURN_ONLY_OPT,
	ID_DROP_PART_SELECTED,
	ID_DROP_PROG_SELECTED,
	ID_DROP_BAUD_SELECTED,
	ID_TEXT_CHANGE1,
	ID_TEXT_CHANGE2,

	// --- ConfigDialog
	ID_INCLUDE_PATH,
	ID_LIBRARY_PATH,
	ID_INCLUDE_PATH_ADD,
	ID_LIBRARY_PATH_ADD,

	// --- SettingsDialog
	ID_LIST_STYLES,
	ID_CHOICE_LANGUAGES,
	ID_PICKER_FORE_COLOUR,
	ID_PICKER_BACK_COLOUR,
	ID_CHECK_FOLD,
	ID_CHECK_ITALIC,
	ID_SPIN_FONTSIZE,

	// --- FileTreePanel
	// Popup Menu
	ID_POPUP_ADD_NEW_FILE,
	ID_POPUP_ADD_EXISTING_FILE,

	// --- WelcomeWindow
	ID_LIST,
	ID_OPEN,
	ID_NEW,
	ID_FIND,
	ID_BROWSE,
};


#endif
