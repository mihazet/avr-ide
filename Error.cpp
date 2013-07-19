#include "Error.h"
#include <wx/regex.h>


// common regex that can be used by the different compiler for matching compiler output
// it can be used in the patterns for warnings, errors, ...
// NOTE : it is an approximation (for example the ':' can appear anywhere and several times)
const wxString Error::FilePathWithSpaces = _T("[][{}() \t#%$~[:alnum:]&_:+/\\.-]+");

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(RegExArray);


Error::Error(wxTextCtrl *out, wxListCtrl *msg)
{
	m_buildLog = out;
	m_buildMessages = msg;
	m_RegExes.Clear();

	m_buildMessages->SetColumnWidth(0, 100);
	m_buildMessages->SetColumnWidth(1, 50);


	LoadDefaultRegExArray();
}

void Error::Check(wxArrayString lines)
{
	for (size_t i = 0; i < lines.GetCount(); i++)
	{
		wxString line = lines[i];

		CompilerLineType clt = CheckForWarningsAndErrors(line);

		LogMessage(m_buildLog, line, clt);
		if (clt != cltNormal)
		{
			LogWarningOrError(clt, m_ErrorFilename, m_ErrorLine, m_Error);
		}


	}
}


CompilerLineType Error::CheckForWarningsAndErrors(const wxString& line)
{
    m_ErrorFilename.Clear();
    m_ErrorLine.Clear();
    m_Error.Clear();

    for (size_t i = 0; i < m_RegExes.Count(); ++i)
    {
        RegExStruct& rs = m_RegExes[i];
        if (rs.regex.IsEmpty())
            continue;
        wxRegEx regex(rs.regex);
        if (regex.Matches(line))
        {
            if (rs.filename > 0)
                 m_ErrorFilename = regex.GetMatch(line, rs.filename);
            if (rs.line > 0)
                m_ErrorLine = regex.GetMatch(line, rs.line);
            for (int x = 0; x < 3; ++x)
            {
                if (rs.msg[x] > 0)
                {
                    if (!m_Error.IsEmpty())
                        m_Error << _T(" ");
                    m_Error << regex.GetMatch(line, rs.msg[x]);
                }
            }
            return rs.lt;
        }
    }

    wxLogVerbose("Error::CheckForWarningsAndErrors: %s %s %s ", m_ErrorFilename.c_str(), m_ErrorLine.c_str(), m_Error.c_str());

    return cltNormal; // default return value
}

void Error::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Fatal error"), cltError, _T("FATAL:[ \t]*(.*)"), 1));
    m_RegExes.Add(RegExStruct(_("'In function...' info"), cltInfo, _T("(") + FilePathWithSpaces + _T("):[ \t]+") + _T("([iI]n ([cC]lass|[cC]onstructor|[dD]estructor|[fF]unction|[mM]ember [fF]unction).*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("'Skipping N instantiation contexts' info (2)"), cltInfo, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t]+(\\[[ \t]+[Ss]kipping [0-9]+ instantiation contexts[ \t]+\\])"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("'Skipping N instantiation contexts' info"), cltInfo, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t]+(\\[[ \t]+[Ss]kipping [0-9]+ instantiation contexts[ \t]+\\])"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("'In instantiation' warning"), cltWarning, _T("(") + FilePathWithSpaces + _T("):[ \t]+([Ii]n [Ii]nstantiation.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("'Required from' warning"), cltWarning, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t]+([Rr]equired from.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("'Instantiated from' info (2)"), cltInfo, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t]+([Ii]nstantiated from .*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("'Instantiated from' info"), cltInfo, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t]+([Ii]nstantiated from .*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Resource compiler error"), cltError, _T("windres.exe:[ \t](") + FilePathWithSpaces + _T("):([0-9]+):[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Resource compiler error (2)"), cltError, _T("windres.exe:[ \t](.*)"), 1));
    m_RegExes.Add(RegExStruct(_("Preprocessor warning"), cltWarning, _T("(") + FilePathWithSpaces + _T("):([0-9]+):([0-9]+):[ \t]([Ww]arning:[ \t].*)"), 4, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler note (2)"), cltInfo, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t]([Nn]ote:[ \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler note"), cltInfo, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t]([Nn]ote:[ \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("General note"), cltInfo, _T("([Nn]ote:[ \t].*)"), 1));
    m_RegExes.Add(RegExStruct(_("Preprocessor error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler warning (2)"), cltWarning, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t]([Ww]arning:[ \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t]([Ww]arning:[ \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Undefined reference (2)"), cltError, FilePathWithSpaces + _T("\\.o:(") + FilePathWithSpaces + _T("):([0-9]+):[ \t](undefined reference.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error (2)"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker warning"), cltWarning, _T("(") + FilePathWithSpaces + _T("):\\(\\.text\\+[0-9a-fA-FxX]+\\):[ \t]([Ww]arning:[ \t].*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Linker error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker error (2)"), cltError, FilePathWithSpaces + _T("\\(.text\\+[0-9A-Za-z]+\\):([ \tA-Za-z0-9_:+/\\.-]+):[ \t](.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Linker error (3)"), cltError, _T("(") + FilePathWithSpaces + _T("):\\(\\.text\\+[0-9a-fA-FxX]+\\):(.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Linker error (lib not found)"), cltError, _T(".*(ld.*):[ \t](cannot find.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Linker error (cannot open output file)"), cltError, _T(".*(ld.*):[ \t](cannot open output file.*):[ \t](.*)"), 2, 1, 0, 3));
    m_RegExes.Add(RegExStruct(_("Linker error (unrecognized option)"), cltError, _T(".*(ld.*):[ \t](unrecognized option.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("No such file or directory"), cltError, _T(".*:(.*):[ \t](No such file or directory.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Undefined reference"), cltError, _T("(") + FilePathWithSpaces + _T("):[ \t](undefined reference.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("General warning"), cltWarning, _T("([Ww]arning:[ \t].*)"), 1));
    m_RegExes.Add(RegExStruct(_("Auto-import info"), cltInfo, _T("([Ii]nfo:[ \t].*)\\(auto-import\\)"), 1));
}

void Error::LogMessage(wxTextCtrl *buildLog, const wxString& output, CompilerLineType clt)
{
	if (clt == cltNormal)
		buildLog->SetDefaultStyle( wxTextAttr(*wxBLACK) );
	else if (clt == cltWarning)
		buildLog->SetDefaultStyle( wxTextAttr(*wxBLUE) );
	else if (clt == cltError)
		buildLog->SetDefaultStyle( wxTextAttr(*wxRED) );

	buildLog->AppendText(output + "\n");
	buildLog->Refresh();

}

void Error::LogWarningOrError(CompilerLineType clt, const wxString& file, const wxString& line, const wxString& msg)
{
	wxColour textColour;

	if (clt == cltNormal)
		textColour = *wxBLACK;
	else if (clt == cltWarning)
		textColour = *wxBLUE;
	else if (clt == cltError)
		textColour = *wxRED;

	long index = m_buildMessages->GetItemCount();
	m_buildMessages->InsertItem(index, file);
	m_buildMessages->SetItem(index, 1, line);
	m_buildMessages->SetItem(index, 2, msg);
	m_buildMessages->SetItemTextColour(index, textColour);
	m_buildMessages->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
}
