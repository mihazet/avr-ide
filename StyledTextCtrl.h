#ifndef STYLED_TEXT_CTRL_H
#define	STYLED_TEXT_CTRL_H

#include <wx/wx.h>
#include <wx/stc/stc.h>

//============================================================================
// declarations
//============================================================================

//----------------------------------------------------------------------------
// style bits types
#define mySTC_STYLE_BOLD 1
#define mySTC_STYLE_ITALIC 2
#define mySTC_STYLE_UNDERL 4
#define mySTC_STYLE_HIDDEN 8

//----------------------------------------------------------------------------
// general folding types
#define mySTC_FOLD_COMMENT 1
#define mySTC_FOLD_COMPACT 2
#define mySTC_FOLD_PREPROC 4

#define mySTC_FOLD_HTML 16
#define mySTC_FOLD_HTMLPREP 32

#define mySTC_FOLD_COMMENTPY 64
#define mySTC_FOLD_QUOTESPY 128

//----------------------------------------------------------------------------
// flags
//----------------------------------------------------------------------------
#define mySTC_FLAG_WRAPMODE 16

#define	STYLE_TYPES_COUNT 32
#define	DEFAULT_LANGUAGE "<default>"

//----------------------------------------------------------------------------
// CommonInfo
//----------------------------------------------------------------------------

struct CommonInfo {
    // editor functionality prefs
    bool syntaxEnable;
    bool foldEnable;
    bool indentEnable;
    // display defaults prefs
    bool readOnlyInitial;
    bool overTypeInitial;
    bool wrapModeInitial;
    bool displayEOLEnable;
    bool indentGuideEnable;
    bool lineNumberEnable;
    bool longLineOnEnable;
    bool whiteSpaceEnable;
};
extern const CommonInfo g_CommonPrefs;

//----------------------------------------------------------------------------
// StyleInfo
//----------------------------------------------------------------------------
struct StyleInfo
{
	int type;
	wxString name;
	wxString foreground;
	wxString background;
	wxString fontname;
	int fontsize;
	int fontstyle;
	int lettercase;
	wxString words;
};

//----------------------------------------------------------------------------
// LanguageInfo
//----------------------------------------------------------------------------

struct LanguageInfo
{
	const wxChar *name;
	const wxChar *filepattern;
	int lexer;
	struct StyleInfo styles [STYLE_TYPES_COUNT];
	int folds;
};

extern LanguageInfo g_LanguagePrefs[];
extern const int g_LanguagePrefsSize;

void LoadColours();
void SaveColours();


//----------------------------------------------------------------------------
// StyledTextCtrl
//----------------------------------------------------------------------------

class StyledTextCtrl : public wxStyledTextCtrl
{
public:
	StyledTextCtrl(wxWindow *parent, wxWindowID id = wxID_ANY);
	bool InitializePrefs (const wxString& name);

private:
	DECLARE_EVENT_TABLE()
	void OnMarginClick (wxStyledTextEvent &event);
	void OnCharAdded(wxStyledTextEvent &event);

	wxString GetLineIndentString(int line);
	void InitLexersHash();

	int	m_LineNrID;
	int m_DividerID;
	int m_FoldingID;
	int m_LineNrMargin;
	int m_FoldingMargin;

	// lanugage properties
	LanguageInfo const* m_language;
};

#endif
