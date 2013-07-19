#ifndef CodePreProcess_H
#define CodePreProcess_H

#include <wx/wx.h>
#include <wx/regex.h>


class CodePreProcess
{
public:
	static wxString StripComments(wxString content)
	{
		bool inStreamComment = false;
		bool inLineComment = false;
		bool inString = false;
		bool inChar = false;

		content = "\n" + content;
		int originalLength = content.Len();
		content += "\n";

		wxString result = "";

		for (int i = 1; i < originalLength; i++)
		{
			char c = content[i];

			if ((inString || inChar) && c == '\\' && content[i + 1] == '\\')
			{
				content = content.Remove(i, 2);
				content = content.insert(i, "  ");
			}
			else if (c == '"' && content[i - 1] != '\\' && inStreamComment == false && inLineComment == false && inChar == false)
			{
				inString = !inString;
			}
			else if (c == '\'' && content[i - 1] != '\\' && inStreamComment == false && inLineComment == false && inString == false)
			{
				inChar = !inChar;
			}
			else if (c == '/' && content[i + 1] == '/' && inString == false && inChar == false)
			{
				inLineComment = true;
				i += 2;
			}
			else if (c == '/' && content[i + 1] == '*' && inLineComment == false && inString == false && inChar == false)
			{
				inStreamComment = true;
			}
			else if (c == '*' && content[i + 1] == '/' && inString == false && inChar == false)
			{
				inStreamComment = false;
				if (inLineComment == false)
				{
					i += 2;
				}
			}
			else if (c == '\n')
			{
				inLineComment = false;
				inString = false;
				inChar = false;
			}

			if (inStreamComment == false && inLineComment == false)
			{
				result += content[i];
			}
		}

		return result.Trim();
	}

	static wxString SingleWhiteSpace(wxString content)
	{
		content.Replace("\t", " ");
		content.Replace("\n", " ");
		content.Replace("\r", " ");
		while (content.Contains("  "))
			content.Replace("  ", " ");
		return content.Trim();
	}

	static wxString CollapseFunctions(wxString content)
	{
		content = StripComments(content);

		int braceNest = 0;
		bool inString = false;
		bool inChar = false;

		content = "\n" + content;
		int originalLength = content.Len();
		content += "\n";

		wxString result = "";

		for (int i = 1; i < originalLength; i++)
		{
			bool printFirstBrace = false;

			if ((inString || inChar) && content[i] == '\\' && content[i + 1] == '\\')
			{
				content = content.Remove(i, 2);
				content = content.insert(i, "  ");
			}
			else if (content[i] == '"' && content[i - 1] != '\\' && inChar == false)
			{
				inString = !inString;
			}
			else if (content[i] == '\'' && content[i - 1] != '\\' && inString == false)
			{
				inChar = !inChar;
			}
			else if (content[i] == '{' && inString == false && inChar == false)
			{
				braceNest++;
				printFirstBrace = true;
			}
			else if (content[i] == '}' && inString == false && inChar == false)
			{
				braceNest--;
			}

			if ((braceNest == 1 && printFirstBrace) || braceNest == 0)
			{
				result += content[i];
			}
		}

		return result.Trim();
	}

	static wxArrayString GetAllPrototypes(wxString content, wxArrayString& protoList)
	{
		content = CollapseFunctions(content);

		wxRegEx r("([\\w\\[\\]\\*]+\\s+[\\[\\]\\*\\w\\s]+\\([,\\[\\]\\*\\w\\s]*\\))(?=\\s*\\{)");

		if (r.Matches(content))
		{
			for (size_t i = 0; i < r.GetMatchCount(); ++i)
			{
				protoList.Add(r.GetMatch(content, i) + ";");
			}
		}
		return protoList;
	}

	static wxArrayString GetAllLibraries(wxString content, wxArrayString& libList)
	{
		content = StripComments(content);

		wxRegEx r("^\\s*#include\\s+((\"\\S+\")|(<\\S+>))");
		if (r.Matches(content))
		{
			for (size_t i = 0; i < r.GetMatchCount(); ++i)
			{
				wxString lib = r.GetMatch(content, i).Trim();

				lib = lib.BeforeFirst('.');

				if (libList.Index(lib) == wxNOT_FOUND)
					libList.Add(lib);
			}
		}
		return libList;
	}
};

#endif
