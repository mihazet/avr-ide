#include "StyledTextCtrl.h"

#include <wx/confbase.h>
#include <wx/xml/xml.h>
#include "XmlUtils.h"



BEGIN_EVENT_TABLE(StyledTextCtrl, wxStyledTextCtrl)
	// stc
    EVT_STC_MARGINCLICK(wxID_ANY,	StyledTextCtrl::OnMarginClick)
	EVT_STC_CHARADDED(wxID_ANY,		StyledTextCtrl::OnCharAdded)
END_EVENT_TABLE()

StyledTextCtrl::StyledTextCtrl(wxWindow *parent, wxWindowID id)
	: wxStyledTextCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxVSCROLL)
{
	m_LineNrID = 0;
	m_DividerID = 1;
	m_FoldingID = 2;

	// default font for all styles
	SetViewEOL(0);
	SetIndentationGuides(0);
	SetEdgeMode(wxSTC_EDGE_NONE);
	SetViewWhiteSpace(wxConfigBase::Get()->Read("/editor/whitespace", 0l) ? wxSTC_WS_VISIBLEALWAYS: wxSTC_WS_INVISIBLE);
	SetOvertype(0);
	SetReadOnly(0);
	SetWrapMode(wxSTC_WRAP_NONE);
	//wxFont font (8, wxMODERN, wxNORMAL, wxNORMAL);
	//StyleSetFont (wxSTC_STYLE_DEFAULT, font);
	//StyleSetForeground (wxSTC_STYLE_DEFAULT, *wxBLACK);
	//StyleSetBackground (wxSTC_STYLE_DEFAULT, *wxWHITE);
	//StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (_T("DARK GREY")));
	//StyleSetBackground (wxSTC_STYLE_LINENUMBER, *wxWHITE);
	//StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour (_T("DARK GREY")));
	InitializePrefs(DEFAULT_LANGUAGE);


	// set visibility
	//SetVisiblePolicy (wxSTC_VISIBLE_STRICT|wxSTC_VISIBLE_SLOP, 1);
	//SetXCaretPolicy (wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);
	//SetYCaretPolicy (wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);

	// markers

	MarkerDefine (wxSTC_MARKNUM_FOLDER,        wxSTC_MARK_BOXPLUS,	 	_T("WHITE"), _T("BLACK"));
	MarkerDefine (wxSTC_MARKNUM_FOLDEROPEN,    wxSTC_MARK_BOXMINUS,  	_T("WHITE"), _T("BLACK"));
	MarkerDefine (wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_VLINE,     	_T("BLACK"), _T("BLACK"));

	MarkerDefine (wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_DOTDOTDOT, 	_T("BLACK"), _T("WHITE"));
	MarkerDefine (wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN, 	_T("BLACK"), _T("WHITE"));
	MarkerDefine (wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_LCORNER, 		_T("BLACK"), _T("BLACK"));
	MarkerDefine (wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_LCORNER,   	_T("BLACK"), _T("BLACK"));

	// miscelaneous
	m_LineNrMargin = TextWidth (wxSTC_STYLE_LINENUMBER, _T("_999999"));
	m_FoldingMargin = 16;



	SetCaretLineBackground(wxColour(wxConfigBase::Get()->Read("/editor/highlight_carret_line_color", "rgb(255, 255, 128)")));
	SetCaretLineVisible(wxConfigBase::Get()->Read("/editor/highlight_carret_line", 1));


	//CmdKeyClear (wxSTC_KEY_TAB, 0); // this is done by the menu accelerator key
	SetLayoutCache (wxSTC_CACHE_PAGE);
}

bool StyledTextCtrl::InitializePrefs (const wxString& name)
{
	LoadColours();

	// initialize styles
	StyleClearAll();
	LanguageInfo const* curInfo = NULL;

	// determine language
	bool found = false;
	int languageNr;
	for (languageNr = 0; languageNr < g_LanguagePrefsSize; languageNr++)
	{
		curInfo = &g_LanguagePrefs [languageNr];
		if (curInfo->name == name)
		{
			found = true;
			break;
		}
	}
	if (!found) return false;

	// set lexer and language
	SetLexer (curInfo->lexer);
	m_language = curInfo;

	// set margin for line numbers
	SetMarginType (m_LineNrID, wxSTC_MARGIN_NUMBER);
	//StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (_T("DARK GREY")));
	//StyleSetBackground (wxSTC_STYLE_LINENUMBER, *wxWHITE);
	SetMarginWidth (m_LineNrID, wxConfigBase::Get()->Read("/editor/line_number", 1l) ? m_LineNrMargin: 0); // start visible

	// default fonts for all styles!
	int Nr;
	for (Nr = 0; Nr < wxSTC_STYLE_LASTPREDEFINED; Nr++)
	{
		wxFont font (8, wxMODERN, wxNORMAL, wxNORMAL);
		StyleSetFont (Nr, font);
	}

	// set common styles
	//StyleSetForeground (wxSTC_STYLE_DEFAULT, wxColour (_T("DARK GREY")));
	//StyleSetForeground (wxSTC_STYLE_INDENTGUIDE, wxColour (_T("DARK GREY")));

	// set margin as unused
	SetMarginType (m_DividerID, wxSTC_MARGIN_SYMBOL);
	SetMarginWidth (m_DividerID, 16);
	SetMarginSensitive (m_DividerID, false);

	// initialize settings
	if (1)
	{
		int keywordnr = 0;
		for (Nr = 0; Nr < STYLE_TYPES_COUNT; Nr++)
        {
			int type = curInfo->styles[Nr].type;
			if (type == -1) continue;
			const StyleInfo &curType = curInfo->styles[Nr];
			wxFont font (	curType.fontsize, wxMODERN, wxNORMAL, wxNORMAL, false,
							curType.fontname);
			StyleSetFont (type, font);
			if (!curType.foreground.IsEmpty())
			{
				StyleSetForeground (type, wxColour (curType.foreground));
			}
			if (!curType.background.IsEmpty())
			{
				StyleSetBackground (type, wxColour (curType.background));
			}
			StyleSetBold (type, (curType.fontstyle & mySTC_STYLE_BOLD) > 0);
			StyleSetItalic (type, (curType.fontstyle & mySTC_STYLE_ITALIC) > 0);
			StyleSetUnderline (type, (curType.fontstyle & mySTC_STYLE_UNDERL) > 0);
			StyleSetVisible (type, (curType.fontstyle & mySTC_STYLE_HIDDEN) == 0);
			StyleSetCase (type, curType.lettercase);
			wxString pwords = curInfo->styles[Nr].words;
			if (!pwords.IsEmpty())
			{
				SetKeyWords (keywordnr, pwords);
				keywordnr += 1;
			}
		}
	}

    // set margin as unused
    SetMarginType (m_DividerID, wxSTC_MARGIN_SYMBOL);
    SetMarginWidth (m_DividerID, 0);
    SetMarginSensitive (m_DividerID, false);

	// folding
	SetMarginType (m_FoldingID, wxSTC_MARGIN_SYMBOL);
	SetMarginMask (m_FoldingID, wxSTC_MASK_FOLDERS);
	StyleSetBackground (m_FoldingID, *wxWHITE);
	SetMarginWidth (m_FoldingID, 0);
	SetMarginSensitive (m_FoldingID, false);
	if (wxConfigBase::Get()->Read("/editor/fold", 1))
	{
		SetMarginWidth (m_FoldingID, m_FoldingMargin);
		SetMarginSensitive (m_FoldingID, 1);
		SetProperty (_T("fold"), wxConfigBase::Get()->Read("/editor/fold", 1) ? _T("1"): _T("0"));
		SetProperty (_T("fold.comment"), wxConfigBase::Get()->Read("/editor/fold_comment", 1) ? _T("1"): _T("0"));
		SetProperty (_T("fold.compact"), wxConfigBase::Get()->Read("/editor/fold_compact", 1) ? _T("1"): _T("0"));
		SetProperty (_T("fold.preprocessor"), wxConfigBase::Get()->Read("/editor/fold_preprocessor", 1) ? _T("1"): _T("0"));
		SetProperty (_T("fold.html"), wxConfigBase::Get()->Read("/editor/fold_html", 1) ? _T("1"): _T("0"));
		SetProperty (_T("fold.html.preprocessor"), wxConfigBase::Get()->Read("/editor/fold_html_preprocessor", 1) ? _T("1"): _T("0"));
		SetProperty (_T("fold.comment.python"), wxConfigBase::Get()->Read("/editor/fold_comment_python", 1) ? _T("1"): _T("0"));
		SetProperty (_T("fold.quotes.python"), wxConfigBase::Get()->Read("/editor/fold_quotes_python", 1) ? _T("1"): _T("0"));
	}

	// set spaces and indention
	SetTabWidth(wxConfigBase::Get()->Read("/editor/tab_width", 4));
	SetUseTabs(wxConfigBase::Get()->Read("/editor/use_tab", 1));
	SetTabIndents(wxConfigBase::Get()->Read("/editor/tab_indents", 1));
	SetBackSpaceUnIndents(wxConfigBase::Get()->Read("/editor/back_space_un_indents", 0l));
	SetIndent(wxConfigBase::Get()->Read("/editor/indent", 4));

	// others
	SetViewEOL(wxConfigBase::Get()->Read("/editor/view_eol", 0l));
	SetIndentationGuides(wxConfigBase::Get()->Read("/editor/indentation_guides", 0l));
	SetEdgeColumn(wxConfigBase::Get()->Read("/editor/edge_column", 80l));
	SetEdgeMode(wxConfigBase::Get()->Read("/editor/edge_mode", 0l) ? wxSTC_EDGE_LINE: wxSTC_EDGE_NONE);
	SetViewWhiteSpace(wxConfigBase::Get()->Read("/editor/white_space", 0l) ? wxSTC_WS_VISIBLEALWAYS: wxSTC_WS_INVISIBLE);
	SetOvertype(wxConfigBase::Get()->Read("/editor/overtype", 0l));
	SetReadOnly(wxConfigBase::Get()->Read("/editor/read_only", 0l));
	SetWrapMode(wxConfigBase::Get()->Read("/editor/wrap_mode", 0l) ? wxSTC_WRAP_WORD: wxSTC_WRAP_NONE);

	return true;
}

bool StyledTextCtrl::FindNext(wxString text, int flags)
{
	// set the search anchor
	int pos = GetCurrentPos();
	if (pos != GetLength())
	{
		SetCurrentPos(pos + 1);
	}
	SearchAnchor();
	int spos = SearchNext(flags, text);
	EnsureCaretVisible();
	if (spos == -1)
	{
		SetCurrentPos(pos);
		return false;
	}
	else
	{
		return true;
	}
}

bool StyledTextCtrl::FindPrevious(wxString text, int flags)
{
	// set the search anchor
	int pos = GetCurrentPos();
	if (pos != GetLength())
	{
		SetCurrentPos(pos - 1);
	}
	SearchAnchor();
	int spos = SearchPrev(flags, text);
	EnsureCaretVisible();
	if (spos == -1)
	{
		SetCurrentPos(pos);
		return false;
	}
	else
	{
		return true;
	}
}

void StyledTextCtrl::Comment()
{
	// Comment out the selected lines
	int startPos, endPos;
	GetSelection(&startPos, &endPos);
	int start = LineFromPosition(startPos);
	int end = LineFromPosition(endPos);
	if (start > end)	// swap around
	{
		int tmp = start;
		start = end;
		end = tmp;
	}
	// start an undo mark
	BeginUndoAction();
	for	(int ln = start; ln < end + 1; ln++)
	{
		int linestart = PositionFromLine(ln);
		InsertText(linestart, "//");
	}
	// finish the undo mark
	EndUndoAction();
}

void StyledTextCtrl::UnComment()
{
	// UnComment out the selected lines"""
	int startPos, endPos;
	GetSelection(&startPos, &endPos);
	int start = LineFromPosition(startPos);
	int end = LineFromPosition(endPos);
	if (start > end)	// swap around
	{
		int tmp = start;
		start = end;
		end = tmp;
	}
	// start an undo mark
	BeginUndoAction();
	for	(int ln = start; ln < end + 1; ln++)
	{
		int linestart = PositionFromLine(ln);
		if (char(GetCharAt(linestart)) == '/' && char(GetCharAt(linestart + 1)) == '/')
		{
			// set cursor to the right of the //
			SetCurrentPos(linestart + 2);
			// delete to the beginning of th line
			DelLineLeft();
		}
	}
	// finish the undo mark
	EndUndoAction();
}


void StyledTextCtrl::OnMarginClick (wxStyledTextEvent &event)
{
	if (event.GetMargin() == 2)
	{
		int lineClick = LineFromPosition (event.GetPosition());
		int levelClick = GetFoldLevel (lineClick);
		if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0)
		{
			ToggleFold (lineClick);
		}
	}
}

void StyledTextCtrl::OnCharAdded(wxStyledTextEvent &event)
{

	char chr = (char)event.GetKey();
	int pos = GetCurrentPos();
	int currentLine = GetCurrentLine();
	// Change this if support for mac files with \r is needed
	if (chr == '\n')
	{
		wxString indent;
		if (currentLine > 0)
		{
		    indent = GetLineIndentString(currentLine - 1);
			InsertText(pos, indent);
			GotoPos(pos + indent.Len());
		}
    }
}

wxString StyledTextCtrl::GetLineIndentString(int line)
{
	int currLine = (line == -1)
					? LineFromPosition(GetCurrentPos())
					: line;
	wxString text = GetLine(currLine);
	unsigned int len = text.Length();
	wxString indent;
	for (unsigned int i = 0; i < len; ++i)
    {
		if (text[i] == _T(' ') || text[i] == _T('\t'))
			indent << text[i];
		else
			break;
	}
	return indent;
}

//----------------------------------------------------------------------------


void LoadColours()
{
	for (int i = 0; i < g_LanguagePrefsSize; i++)
	{
		for (int j = 0; j < STYLE_TYPES_COUNT; ++j)
		{
			wxString entry = wxString::Format("/%s/%d", g_LanguagePrefs[i].name, j);
			if (wxConfigBase::Get()->HasGroup(entry))
			{
				if (wxConfigBase::Get()->Read(entry + "/type", -1) == -1) continue;
				g_LanguagePrefs[i].styles[j].background = wxConfigBase::Get()->Read(entry + "/background", "white");
				g_LanguagePrefs[i].styles[j].foreground = wxConfigBase::Get()->Read(entry + "/foreground", "black");
				g_LanguagePrefs[i].styles[j].fontsize = wxConfigBase::Get()->Read(entry + "/fontsize", 8);
				g_LanguagePrefs[i].styles[j].fontstyle = wxConfigBase::Get()->Read(entry + "/fontstyle", 0l);
				g_LanguagePrefs[i].styles[j].name = wxConfigBase::Get()->Read(entry + "/name", "Default");
				g_LanguagePrefs[i].styles[j].words = wxConfigBase::Get()->Read(entry + "/words", "for");
			}
		}
	}
}

void SaveColours()
{
	for (int i = 0; i < g_LanguagePrefsSize; i++)
	{
		for (int j = 0; j < STYLE_TYPES_COUNT; ++j)
		{
			wxString entry = wxString::Format("/%s/%d", g_LanguagePrefs[i].name, j);
			wxConfigBase::Get()->Write(entry + "/type", g_LanguagePrefs[i].styles[j].type);
			wxConfigBase::Get()->Write(entry + "/background", g_LanguagePrefs[i].styles[j].background);
			wxConfigBase::Get()->Write(entry + "/foreground", g_LanguagePrefs[i].styles[j].foreground);
			wxConfigBase::Get()->Write(entry + "/fontsize", g_LanguagePrefs[i].styles[j].fontsize);
			wxConfigBase::Get()->Write(entry + "/fontstyle", g_LanguagePrefs[i].styles[j].fontstyle);
			wxConfigBase::Get()->Write(entry + "/name", g_LanguagePrefs[i].styles[j].name);
			wxConfigBase::Get()->Write(entry + "/words", g_LanguagePrefs[i].styles[j].words);
		}
	}
}

//----------------------------------------------------------------------------
// keywordlists
// C++
const wxString CppWordlist1 =
    _T("asm auto bool break case catch char class const const_cast ")
    _T("continue default delete do double dynamic_cast else enum explicit ")
    _T("export extern false float for friend goto if inline int long ")
    _T("mutable namespace new operator private protected public register ")
    _T("reinterpret_cast return short signed sizeof static static_cast ")
    _T("struct switch template this throw true try typedef typeid ")
    _T("typename union unsigned using virtual void volatile wchar_t ")
    _T("while");
const wxString CppWordlist2 =
    _T("file");
const wxString CppWordlist3 =
    _T("a addindex addtogroup anchor arg attention author b brief bug c ")
    _T("class code date def defgroup deprecated dontinclude e em endcode ")
    _T("endhtmlonly endif endlatexonly endlink endverbatim enum example ")
    _T("exception f$ f[ f] file fn hideinitializer htmlinclude ")
    _T("htmlonly if image include ingroup internal invariant interface ")
    _T("latexonly li line link mainpage name namespace nosubgrouping note ")
    _T("overload p page par param post pre ref relates remarks return ")
    _T("retval sa section see showinitializer since skip skipline struct ")
    _T("subsection test throw todo typedef union until var verbatim ")
    _T("verbinclude version warning weakgroup $ @ \"\" & < > # { }");


//----------------------------------------------------------------------------
// languages
LanguageInfo g_LanguagePrefs [] =
{
	// C++
	{
		_T("C++"),
		_T("*.c;*.cc;*.cpp;*.cxx;*.cs;*.h;*.hh;*.hpp;*.hxx;*.sma"),
		wxSTC_LEX_CPP,
		{
			{wxSTC_C_DEFAULT, _T("Default"), _T("BLACK"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_C_COMMENT, _T("Comment"), _T("FOREST GREEN"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_C_COMMENTLINE, _T("Comment line"), _T("FOREST GREEN"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_C_COMMENTDOC, _T("Comment (Doc)"), _T("FOREST GREEN"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_C_NUMBER, _T("Number"), _T("SIENNA"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_C_WORD, _T("Keyword"), _T("BLUE"), _T("WHITE"), _T(""), 8, mySTC_STYLE_BOLD, 0, CppWordlist1}, // KEYWORDS
			{wxSTC_C_STRING, _T("String"), _T("BROWN"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_C_CHARACTER, _T("Character"), _T("KHAKI"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_C_UUID, _T("UUID"), _T("ORCHID"), _T("WHITE"), _T(""),	8, 0, 0, wxEmptyString},
			{wxSTC_C_PREPROCESSOR, _T("Preprocessor"), _T("GREY"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_C_OPERATOR, _T("Operator"), _T("BLACK"), _T("WHITE"), _T(""), 8, mySTC_STYLE_BOLD, 0, wxEmptyString},
			{wxSTC_C_IDENTIFIER, _T("Identifier"), _T("BLACK"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_C_STRINGEOL, _T("String (EOL)"), _T("BROWN"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_C_VERBATIM, _T("Verbatim"), _T("BLACK"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString}, // VERBATIM
			{wxSTC_C_REGEX, _T("Regular expression"), _T("ORCHID"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_C_COMMENTLINEDOC, _T("Comment line doc"), _T("FOREST GREEN"), _T("WHITE"),	_T(""), 8, mySTC_STYLE_ITALIC, 0, wxEmptyString}, // DOXY
			{wxSTC_C_WORD2, _T("Keyword 2"), _T("DARK BLUE"), _T("WHITE"), _T(""), 8, 0, 0, CppWordlist2}, // EXTRA WORDS
			{wxSTC_C_COMMENTDOCKEYWORD, _T("Comment doc keyword "), _T("CORNFLOWER BLUE"), _T("WHITE"), _T(""), 8, 0, 0, CppWordlist3}, // DOXY KEYWORDS
			{wxSTC_C_COMMENTDOCKEYWORDERROR, _T("Comment doc keyword error"), _T("RED"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString}, // KEYWORDS ERROR
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
		},
		mySTC_FOLD_COMMENT | mySTC_FOLD_COMPACT | mySTC_FOLD_PREPROC
	},
	// * (any)
	{
		(wxChar *)DEFAULT_LANGUAGE,
		_T("*.*"),
		wxSTC_LEX_PROPERTIES,
		{
			{wxSTC_PROPS_DEFAULT, _T("Default"), _T("BLACK"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_PROPS_COMMENT, _T("Comment"), _T("BLACK"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_PROPS_SECTION, _T("Section"), _T("BLACK"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_PROPS_ASSIGNMENT, _T("Assignment"), _T("BLACK"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{wxSTC_PROPS_DEFVAL, _T("Defval"), _T("BLACK"), _T("WHITE"), _T(""), 8, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
			{-1, "", "", "", "", 0, 0, 0, wxEmptyString},
		},
     	0
	},
};

const int g_LanguagePrefsSize = WXSIZEOF(g_LanguagePrefs);
