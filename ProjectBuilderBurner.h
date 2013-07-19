#include <wx/wx.h>
#include <wx/listctrl.h>

#include "ProjectManagment.h"

// класс дл€ компил€ции и сборки проекта под avr-gcc

class ProjectBuilder
{
public:
	ProjectBuilder(AVRProject *project, wxTextCtrl *outputTextCtrl, wxListCtrl *errorList);
	~ProjectBuilder();

	bool StartBuild();
	void StartMake();
	bool Compile(ProjectFile *file);

	static bool CheckForWinAVR();
	static bool CheckForWinAVR2();

	static void SetEnviroVarsForProc(/*ProcessStartInfo psi*/);
	static wxString GetToolPath(wxString toolName);

private:
	bool Build();
	bool CreateLib(const wxString& avr_ar_targets);
	bool IsFileMain(ProjectFile *file);
	bool CreateELF(const wxString& OBJECTS, bool suppressErrors);
	void CleanOD(const wxString& OBJECTS);
	bool CreateHex();
	bool CreateEEP();
	bool CreateLST();
	void ReadSize();
	void ReadErrAndWarning(wxArrayString reader);
	wxArrayString GetAllFunctProto();
//	void GetCompilableFiles(wxString folder, ProjectFileHash *fileList, bool isLibrary);

//	wxString GetFileContents(ProjectFile *file);
 	void PrepProject();



	AVRProject		*m_project;
	AVRProject		*m_workingProject;
	ProjectFileHash	m_origFileLsit;
	ProjectFileHash	m_workingFileList;
	wxTextCtrl		*m_outputTextCtrl;
	wxListCtrl		*m_errorList;
	wxListCtrl		*m_errorOnlyList;

	int				hasError;
	static bool		m_avrToolsInstalled;
};

class ProjectBurner
{
public:
	ProjectBurner(AVRProject *project);

	void BurnCMD(bool onlyOptions, bool burnFuses, wxPanel *formObj);
	static wxArrayString GetAvailParts(wxString progname, bool suppressErr);
	static wxArrayString GetAvailProgrammers(bool suppressErr);

private:
	static wxArrayString GetPartWords(wxString output, wxArrayString& res);
	static wxArrayString GetFirstWords(wxString output, wxArrayString& res);
	AVRProject		*m_project;
};
