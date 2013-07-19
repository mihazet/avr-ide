#include <wx/wx.h>
#include <wx/listctrl.h>

enum LogTarget
{
    ltMessages  = 0x01,
    ltFile      = 0x02,

    ltAll       = 0xff
};

// Enum categorizing compiler's output line as warning/error/info/normal
enum CompilerLineType
{
    cltNormal = 0,
    cltWarning,
    cltError,
    cltInfo
};

// regexes array declaration
struct RegExStruct
{
    RegExStruct()
        : desc(_("Unknown")), lt(cltError), regex(_T("")), filename(0), line(0)
    {
        memset(msg, 0, sizeof(msg));
    }
    RegExStruct(const RegExStruct& rhs)
        : desc(rhs.desc), lt(rhs.lt), regex(rhs.regex), filename(rhs.filename), line(rhs.line)
    {
        memcpy(msg, rhs.msg, sizeof(msg));
    }
    RegExStruct(const wxString&  _desc,
                CompilerLineType _lt,
                const wxString&  _regex,
                int              _msg,
                int              _filename = 0,
                int              _line     = 0,
                int              _msg2     = 0,
                int              _msg3     = 0)
        : desc(_desc), lt(_lt), regex(_regex), filename(_filename), line(_line)
    {
        msg[0] = _msg;
        msg[1] = _msg2;
        msg[2] = _msg3;
    }
    bool operator!=(const RegExStruct& other)
    {
        return !(*this == other);
    }
    bool operator==(const RegExStruct& other)
    {
        return (   desc     == other.desc
                && lt       == other.lt
                && regex    == other.regex
                && msg[0]   == other.msg[0]
                && msg[1]   == other.msg[1]
                && msg[2]   == other.msg[2]
                && filename == other.filename
                && line     == other.line );
    }
    wxString         desc;     // title of this regex
    CompilerLineType lt;       // classify the line, if regex matches
    wxString         regex;    // the regex to match
    int              msg[3];   // up-to 3 sub-expression nr for warning/error message
    int              filename; // sub-expression nr for filename
    int              line;     // sub-expression nr for line number
    // if more than one sub-expressions are entered for msg,
    // they are appended to each other, with one space in between.
    // Appending takes place in the same order...
};
WX_DECLARE_OBJARRAY(RegExStruct, RegExArray);


class Error
{
public:
	Error(wxTextCtrl *out, wxListCtrl *msg);

	void Check(wxArrayString lines);
	// вывод в TextCtrl
	static void LogMessage(wxTextCtrl *buildLog, const wxString& output, CompilerLineType clt = cltNormal);

private:
	// проверить строку на ошибки
	CompilerLineType CheckForWarningsAndErrors(const wxString& line);

	// загрузить регуляные выражения для проверки
	void LoadDefaultRegExArray();

	// вывод в ListCtrl
	void LogWarningOrError(CompilerLineType clt, const wxString& file, const wxString& line, const wxString& msg);


	wxString	m_ErrorFilename;
	wxString	m_ErrorLine;
	wxString	m_Error;

	wxTextCtrl	*m_buildLog;
	wxListCtrl	*m_buildMessages;
	RegExArray	m_RegExes;

	static const wxString FilePathWithSpaces;
};
