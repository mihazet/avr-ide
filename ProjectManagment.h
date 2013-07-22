#ifndef PROJECT_MANAGMENT_H
#define PROJECT_MANAGMENT_H

#include <wx/wx.h>
#include <wx/filename.h>
#include "defsext.h"

class AVRProject;

class ProjectFile
{
public:
	ProjectFile(const wxString fileAbsPath, AVRProject *project);

	wxString	FileExt() { return wxFileName(m_fileAbsPath).GetExt(); }
	wxString	FileName() { return wxFileName(m_fileAbsPath).GetFullName(); }
	wxString	FileNameNoExt() { return wxFileName(m_fileAbsPath).GetName(); }
	wxString	FileDir() { return wxFileName(m_fileAbsPath).GetPath(); }

	bool		Exists() { return wxFileExists(m_fileAbsPath); }
	bool		IsSource()
	{
		return FileExt() == "c" || FileExt() == "cpp" || FileExt() == "cxx" || FileExt() == "S";
	}

	bool		ToCompile()	{ return IsSource() && m_toCompile; }
	void		SetToCompile(bool value) { m_toCompile = value; }

	bool		IsOpen() { return m_isOpen; }
	void 		SetIsOpen(bool value) { m_isOpen = value; }

	wxString	m_fileAbsPath;

	wxString FileRelPathTo(const wxString& projDirPath)
	{
		wxString tmp = m_fileAbsPath;
		wxFileName filename(tmp);
		if (filename.MakeRelativeTo(projDirPath))
			return filename.GetFullPath();
		return m_fileAbsPath;
	}

	wxString	m_options;
	int			m_cursorPos;

private:
	AVRProject	*m_project;
	bool		m_isOpen;
	bool		m_toCompile;
};

WX_DECLARE_HASH_MAP(wxString, ProjectFile*, wxStringHash, wxStringEqual, ProjectFileHash);

class AVRProject
{
public:
	AVRProject();

	// properties
	bool m_shouldReloadFiles;
	bool m_shouldReloadDevice;
	bool m_shouldReloadClock;
	bool m_hasBeenConfigged;
	bool m_isReady;

	bool HasBeenConfigged() { return m_hasBeenConfigged; }
	bool IsReady() { return m_isReady; }


	wxString FilePath() { return m_filePath; }
	void SetFilePath(wxString file)
	{
		m_filePath = file;
		m_dirPath = wxFileName(m_filePath).GetPath();
	}
	wxString DirPath() { return m_dirPath; }
	wxString FileName() { return wxFileName(m_filePath).GetFullName(); }
	wxString FileNameNoExt() { return wxFileName(m_filePath).GetName(); }
	wxString SafeFileNameNoExt() { return FileNameNoExt(); }

	wxString		m_lastFile;

	// Compilation properties
	bool m_unsignedChars;
	bool m_unsignedBitfields;
	bool m_packStructs;
	bool m_shortEnums;
	bool m_functSects;
	bool m_dataSects;
	bool m_useInitStack;
	uint32_t m_initStackAddr;
	wxString m_otherOpt;
	wxString m_otherOptForC;
	wxString m_otherOptForCPP;
	wxString m_otherOptForS;
	wxString m_linkerOpt;
	wxString m_optimization;
	uint32_t m_clkFreq;
	wxString m_device;
	wxString m_outputDir;

	// Debugger Settings

	wxString m_iCEHardware;
	int m_jTAGFreq;

	// AVRDUDE Fields and Properties

	wxString m_burnPart;
	wxString m_burnProgrammer;
	wxString m_burnOptions;

	wxString m_burnPort;
	int m_burnBaud;
	bool m_burnAutoReset;
	wxString m_burnFuseBox;

	// Lists

	MemorySegmentHash&	MemorySegList() { return m_memorySegList; }
	ProjectFileHash&	FileList() { return m_fileList; }

	wxArrayString& IncludeDirList() { return m_includeDirList; }
	wxArrayString& LibraryDirList() { return m_libraryDirList; }
	wxArrayString& LinkObjList() { return m_linkObjList; }
	wxArrayString& LinkLibList() { return m_linkLibList; }


	// XML Saving and Loading
	bool Save();
	bool Save(const wxString& filepath);
	bool Open(const wxString& filepath);
	void Reset();

	bool CreateNew(const wxString& filepath);


	AVRProject *Clone()
	{
		AVRProject *newObject = new AVRProject();
		newObject = CopyProperties(newObject);
		return newObject;
	}

	AVRProject *CopyProperties(AVRProject *project)
	{
		project->m_filePath = m_filePath;
		project->m_dirPath = m_dirPath;
		project->m_includeDirList = m_includeDirList;
		project->m_libraryDirList = m_libraryDirList;
		project->m_linkObjList = m_linkObjList;
		project->m_linkLibList = m_linkLibList;
		project->m_shouldReloadFiles = m_shouldReloadFiles;
		project->m_shouldReloadDevice = m_shouldReloadDevice;
		project->m_shouldReloadClock = m_shouldReloadClock;
		project->m_hasBeenConfigged = m_hasBeenConfigged;
		project->m_isReady = m_isReady;
		project->m_lastFile = m_lastFile;

		// Compilation properties
		project->m_unsignedChars = m_unsignedChars;
		project->m_unsignedBitfields = m_unsignedBitfields;
		project->m_packStructs = m_packStructs;
		project->m_shortEnums = m_shortEnums;
		project->m_functSects = m_functSects;
		project->m_dataSects = m_dataSects;
		project->m_useInitStack = m_useInitStack;
		project->m_initStackAddr = m_initStackAddr;
		project->m_otherOpt = m_otherOpt;
		project->m_otherOptForC = m_otherOptForC;
		project->m_otherOptForCPP = m_otherOptForCPP;
		project->m_otherOptForS = m_otherOptForS;
		project->m_linkerOpt = m_linkerOpt;
		project->m_optimization = m_optimization;
		project->m_clkFreq = m_clkFreq;
		project->m_device = m_device;
		project->m_outputDir = m_outputDir;

		// Debugger Settings

		project->m_iCEHardware = m_iCEHardware;
		project->m_jTAGFreq = m_jTAGFreq;

		// AVRDUDE Fields and Properties

		project->m_burnPart = m_burnPart;
		project->m_burnProgrammer = m_burnProgrammer;
		project->m_burnOptions = m_burnOptions;

		project->m_burnPort = m_burnPort;
		project->m_burnBaud = m_burnBaud;
		project->m_burnAutoReset = m_burnAutoReset;
		project->m_burnFuseBox = m_burnFuseBox;

		// Lists

		project->m_fileList = ProjectFileHash(m_fileList);
		project->m_memorySegList = MemorySegmentHash(m_memorySegList);
		return project;
	}

private:
	//wxString		m_fileName;
	wxString		m_filePath;
	wxString		m_dirPath;

	MemorySegmentHash	m_memorySegList;
	ProjectFileHash		m_fileList;

	wxArrayString m_includeDirList;
	wxArrayString m_libraryDirList;
	wxArrayString m_linkObjList;
	wxArrayString m_linkLibList;
};

#endif
