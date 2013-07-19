#include "main.h"
#include "IDEFrame.h"
#include "ProjectManagment.h"



IMPLEMENT_APP(IDEApp)

bool IDEApp::OnInit()
{
	AVRProject *project = new AVRProject();
	IDEFrame *ide = new IDEFrame(project);
	ide->Show();
	return true;
}
/*
wxString IDEApp::RelativePath(wxString dirPath, wxString filePath)
{
	wxString relPath = "";
	//string[] curDirList = CleanFilePath(dirPath).Split(new char[] { Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar, });
	//string[] pathDirList = CleanFilePath(filePath).Split(new char[] { Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar, });

	wxFileName

	size_t i = 0;
	for (i = 0; i < dirPath.size() && i < filePath.Len(); i++)
	{
		if (dirPath[i] != filePath[i])
		{
			break;
		}
	}

	for (size_t j = dirPath.Len(); j > i; j--)
	{
		relPath += (".." + wxFILE_SEP_PATH);
	}

	for (size_t j = i; i < filePath.Len(); i++)
	{
		relPath += filePath[i] + wxFILE_SEP_PATH;
	}
	return relPath;
}
*/
