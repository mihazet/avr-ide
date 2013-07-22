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


// --------------------- Common -----------------------------------------------

// ----------------------------------------------------------------------------
// --- ID's
// ----------------------------------------------------------------------------

enum
{
	// --- IDEFrame
	// Controls
	ID_TREE = wxID_HIGHEST,
	ID_LIST_BUILD,
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
	ID_EDIT_FIND_PREVIOUS,
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

// ----------------------------------------------------------------------------
// --- Predefined Chips
// ----------------------------------------------------------------------------

const wxString g_chips =
	"AT86RF401 AT89S51 AT89S52 AT90CAN128 AT90CAN32 AT90CAN64 AT90PWM1 AT90PWM2 AT90PWM216 "
	"AT90PWM2B AT90PWM3 AT90PWM316 AT90PWM3B AT90PWM81 AT90S1200 AT90S2313 AT90S2323 AT90S2343 "
	"AT90S4414 AT90S4433 AT90S4434 AT90S8515 AT90S8515comp AT90S8535 AT90S8535comp AT90SCR100H "
	"AT90USB1286 AT90USB1287 AT90USB162 AT90USB646 AT90USB647 AT90USB82 ATA6289 ATmega103 ATmega103comp "
	"ATmega128 ATmega1280 ATmega1281 ATmega1284 ATmega1284P ATmega128A ATmega128RFA1 ATmega16 "
	"ATmega161 ATmega161comp ATmega162 ATmega163 ATmega164A ATmega164P ATmega164PA ATmega165 "
	"ATmega165A ATmega165P ATmega165PA ATmega168 ATmega168A ATmega168P ATmega168PA ATmega169 "
	"ATmega169A ATmega169P ATmega169PA ATmega16A ATmega16HVA ATmega16HVA2 ATmega16HVB ATmega16M1 "
	"ATmega16U2 ATmega16U4 ATmega2560 ATmega2561 ATmega32 ATmega323 ATmega324A ATmega324P "
	"ATmega324PA ATmega325 ATmega3250 ATmega3250P ATmega325A ATmega325P ATmega328 ATmega328P "
	"ATmega329 ATmega3290 ATmega3290P ATmega329A ATmega329P ATmega329PA ATmega32A ATmega32C1 "
	"ATmega32HVB ATmega32M1 ATmega32U2 ATmega32U4 ATmega32U6 ATmega406 ATmega48 ATmega48A "
	"ATmega48P ATmega48PA ATmega64 ATmega640 ATmega644 ATmega644A ATmega644P ATmega644PA "
	"ATmega645 ATmega6450 ATmega6450A ATmega645A ATmega649 ATmega6490 ATmega6490A ATmega649A "
	"ATmega649P ATmega64A ATmega64C1 ATmega64HVE ATmega64M1 ATmega8 ATmega8515 ATmega8535 "
	"ATmega88 ATmega88A ATmega88P ATmega88PA ATmega8A ATmega8HVA ATmega8U2 ATtiny10 ATtiny11 "
	"ATtiny12 ATtiny13 ATtiny13A ATtiny15 ATtiny167 ATtiny20 ATtiny22 ATtiny2313 ATtiny2313A "
	"ATtiny24 ATtiny24A ATtiny25 ATtiny26 ATtiny261 ATtiny261A ATtiny28 ATtiny4 ATtiny40 ATtiny4313 "
	"ATtiny43U ATtiny44 ATtiny44A ATtiny45 ATtiny461 ATtiny461A ATtiny48 ATtiny5 ATtiny84 "
	"ATtiny85 ATtiny861 ATtiny861A ATtiny87 ATtiny88 ATtiny9 ATxmega128A1 ATxmega128A1U "
	"ATxmega128A3 ATxmega128D3 ATxmega16A4 ATxmega16D4 ATxmega192A3 ATxmega192D3 ATxmega256A3 "
	"ATxmega256A3B ATxmega256D3 ATxmega32A4 ATxmega32D4 ATxmega64A1 ATxmega64A3 ATxmega64D3 ";


#endif
