#ifndef SETTINGS_H
#define SETTINGS_H

#include <wx/wx.h>
#include <wx/fileconf.h>

WX_DECLARE_HASH_MAP(int, wxString, wxIntegerHash, wxIntegerEqual, RecentLookUpFileListHash);

class RecentFiles
{
public:
	static wxString FilePathFromListBoxIndex(int index);
	static void AddFileAsMostRecent(wxString filePath);
	static void FillListBox(wxListBox *toFill);
	static void SaveRecentList();
	static void LoadRecentList();

private:
	static wxString	m_recentFilePath;
	static wxArrayString m_recentFileList;
	static RecentLookUpFileListHash m_recentLookupList;
};

#endif
