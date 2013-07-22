#include "RecentFiles.h"

#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>


// Default
wxString					RecentFiles::m_recentFilePath;
wxArrayString				RecentFiles::m_recentFileList;
RecentLookUpFileListHash 	RecentFiles::m_recentLookupList;

wxString RecentFiles::FilePathFromListBoxIndex(int index)
{
	if (m_recentLookupList[index])
		return m_recentLookupList[index];
	return "";
}

void RecentFiles::AddFileAsMostRecent(wxString filePath)
{
	while (m_recentFileList.Index(filePath) != wxNOT_FOUND)
	{
		m_recentFileList.Remove(filePath);
	}
	m_recentFileList.Insert(filePath, 0);
}

void RecentFiles::FillListBox(wxListBox *toFill)
{
	toFill->Clear();
	m_recentLookupList.clear();
	for (size_t i = 0; i < m_recentFileList.GetCount(); ++i)
	{
		wxString filePath = m_recentFileList[i];
		wxString displayName = filePath;
		/*
		wxArrayString folders = wxStringTokenize(filePath, wxFILE_SEP_PATH);

		if (folders.GetCount() - 1 >= 0)
		{
			displayName = wxFILE_SEP_PATH + folders[folders.GetCount() - 1];
		}

		if (folders.GetCount() - 2 >= 0)
		{
			displayName = wxFILE_SEP_PATH + folders[folders.GetCount() - 2] + displayName;
		}
		else
		{
			displayName = folders[0] + displayName;
		}

		if (folders.GetCount() - 3 >= 1)
		{
			displayName = folders[0] + "\\~~~\\" + folders[folders.GetCount() - 3] + displayName;
		}
		else
		{
			displayName = folders[0] + displayName;
		}
		*/
		int index = toFill->Append(displayName);
		m_recentLookupList[index] = filePath;
	}
}


void RecentFiles::SaveRecentList()
{

	int cnt = 0;
	for (size_t i = 0; i < m_recentFileList.GetCount(); ++i)
	{
		wxString file = m_recentFileList[i];
		if (wxFileExists(file))
		{
			wxConfigBase::Get()->Write(wxString::Format("/recent_files/%d", i), file);
			cnt++;
		}

		if (cnt > 15)
			break;
	}
}

void RecentFiles::LoadRecentList()
{
	m_recentFileList.Clear();
	for (size_t i = 0; ; ++i)
	{
		wxString entry = wxString::Format("/recent_files/%d", i);
		if (!wxConfigBase::Get()->HasEntry(entry))
        	break;
        wxString file = wxConfigBase::Get()->Read(entry, "");
        if (wxFileExists(file))
   			m_recentFileList.Add(file);
    }
}
