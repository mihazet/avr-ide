#include "ProjectBuilderBurner.h"
#include "BurnerPanel.h"
#include "DefsExt.h"
#include "Error.h"

#include <wx/regex.h>
#include <wx/ffile.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>

// --- Small Helper

class WorkingDirectoryChanger
{
public:
	WorkingDirectoryChanger(const wxString& newDir)
	{
		m_dir = wxGetCwd();
		wxSetWorkingDirectory(newDir);
		wxLogVerbose("Changed dir to %s", newDir.c_str());
	}

	~WorkingDirectoryChanger()
	{
		wxSetWorkingDirectory(m_dir);
		wxLogVerbose("Changed dir to %s", m_dir.c_str());
	}

private:
	wxString m_dir;
};


bool ProjectBuilder::m_avrToolsInstalled = false;

// --- ProjectBuilder Constructor

ProjectBuilder::ProjectBuilder(	AVRProject *project, wxTextCtrl *outputTextCtrl, wxListCtrl *errorList)
{
	m_project = project;
	m_origFileLsit = m_project->FileList();
	m_outputTextCtrl = outputTextCtrl;
	m_errorList = errorList;
	hasError = 0;
	m_workingProject = 0;
//	m_workingFileList = 0;

	m_outputTextCtrl->Clear();

	m_errorList->ClearAll();
	m_errorList->InsertColumn(0, "File");
	m_errorList->InsertColumn(1, "Line");
	m_errorList->InsertColumn(2, "Message");

	PrepProject();
}

ProjectBuilder::~ProjectBuilder()
{
	delete m_workingProject;
//	delete m_workingFileList;
}

// --- public functions

bool ProjectBuilder::StartBuild()
{
	hasError = 0;
	Error::LogMessage(m_outputTextCtrl, "Build Starting...");
	//ListViewModify(m_errorList, wxListItem());
	//ListViewModify(m_errorOnlyList, wxListItem());
	PrepProject();

	//TextBoxModify(m_outputTextCtrl, "");

	return Build();
}

void ProjectBuilder::StartMake()
{
	wxFAIL_MSG("Not Implemented!");
}

bool ProjectBuilder::Compile(ProjectFile *file)
{
	wxString outputAbsPath = m_workingProject->DirPath() + wxFILE_SEP_PATH + m_workingProject->m_outputDir;

	// создаем каталог
	if (!wxDirExists(outputAbsPath))
		if (!wxMkdir(outputAbsPath))
			return false;

	wxLogVerbose(outputAbsPath);

	wxString objectFileAbsPath = outputAbsPath + wxFILE_SEP_PATH + file->FileNameNoExt() + ".o";
	wxLogVerbose(objectFileAbsPath);

	// ужал€ем объектный файл если есть
	if (wxFileExists(objectFileAbsPath))
	{
		if (!wxRemoveFile(objectFileAbsPath))
		{
			Error::LogMessage(m_outputTextCtrl, "####Error: object file could not be deleted at " + objectFileAbsPath);
		}
	}

	// конструируем опции и аргументы дл€ avr-gcc

	wxString args = "";
	for (size_t i = 0; i < m_workingProject->IncludeDirList().GetCount(); ++i)
	{
		wxString path = m_workingProject->IncludeDirList()[i];
		if(!path.IsEmpty())
			args += "-I\"" + path + "\" ";
	}
	//args += "-I\"" + file->FileDir + "\" "; // ?????

	wxString checklist = "";

	if (m_project->m_packStructs)
		checklist += "-fpack-struct ";

	if (m_project->m_shortEnums)
		checklist += "-fshort-enums ";

	if (m_project->m_unsignedChars)
		checklist += "-funsigned-char ";

	if (m_project->m_unsignedBitfields)
		checklist += "-funsigned-bitfields ";

	if (m_project->m_functSects)
		checklist += "-ffunction-sections ";

	//if (m_project->m_dataSections)
	//	checklist += "-fdata-sections ";

	wxString asmflags = "";

	if (file->FileExt() == "S")
		asmflags += "-x assembler-with-cpp -Wa,-gdwarf2";

	wxString fileTypeOptions = "";
	if (file->FileExt() == "c")
		fileTypeOptions = m_workingProject->m_otherOptForC;
	else if (file->FileExt() == "cpp" || file->FileExt() == "cxx")
		fileTypeOptions = m_workingProject->m_otherOptForCPP;
	else if (file->FileExt() == "S")
		fileTypeOptions = m_workingProject->m_otherOptForS;

	wxString clkStr = " ";
	if (m_project->m_clkFreq != 0)
		clkStr = wxString::Format(" -DF_CPU=%dUL ", m_project->m_clkFreq);

	args += wxString::Format(" -mmcu=%s%s%s %s %s -MD -MP -MT %s.o %s -c %s %s \"%s\"",
                m_workingProject->m_device.c_str(),
                clkStr.c_str(),
                m_workingProject->m_optimization.c_str(),
                checklist.c_str(),
                m_workingProject->m_otherOpt.c_str(),
                file->FileNameNoExt().c_str(),
                asmflags.c_str(),
                file->m_options.c_str(),
                fileTypeOptions.c_str(),
                file->m_fileAbsPath.c_str()
            );

	wxArrayString output, error;

	WorkingDirectoryChanger dirChanger(outputAbsPath);

	// c++ uses avr-g++ while c uses avr-gcc, duh
	if (file->FileExt() == "cpp" || file->FileExt() == "cxx")
	{
		Error::LogMessage(m_outputTextCtrl, "Compiling: avr-g++ " + args);
		wxExecute("avr-g++ " + args, output, error);
	}
	else
	{
		Error::LogMessage(m_outputTextCtrl, "Compiling: avr-gcc " + args);
		wxExecute("avr-gcc " + args, output, error);
	}

	ReadErrAndWarning(output);
	ReadErrAndWarning(error);


	if (wxFileExists(objectFileAbsPath))
	{
		return true;
	}
	else
	{
		Error::LogMessage(m_outputTextCtrl, "####Error: object file not created at " + objectFileAbsPath);
		return false;
	}
}


// --- static public functions

bool ProjectBuilder::CheckForWinAVR()
{
	return true;
}

bool ProjectBuilder::CheckForWinAVR2()
{
	return true;
}


void ProjectBuilder::SetEnviroVarsForProc(/*ProcessStartInfo psi*/)
{

}

wxString ProjectBuilder::GetToolPath(wxString toolName)
{
	return wxEmptyString;
}

// --- Implementation build, compile, elf, hex, size ...

bool ProjectBuilder::Build()
{
	bool result = true;

	wxLogVerbose("ProjectBuilder::Build");

	// добавл€ем рабочий каталог проекта в список
	// каталогов
	m_workingProject->IncludeDirList().Add(m_workingProject->DirPath());
	wxString objFiles = "";
	wxString avr_ar_targets = "";
	wxString mainFileName = "";

	// компилируем все source-файлы в проекте
	ProjectFileHash::iterator it;
	for( it = m_workingFileList.begin(); it != m_workingFileList.end(); ++it )
	{
		wxString key = it->first;
		ProjectFile *file = it->second;


		if (file->Exists() && file->ToCompile()
			&& (file->FileExt() == "c" || file->FileExt() == "cpp" || file->FileExt() == "cxx" || file->FileExt() == "S") )
		{
			wxLogVerbose(file->m_fileAbsPath);
			//bool fileHasMain = IsFileMain(file);
			bool objRes = Compile(file);
			result &= objRes;
			if (objRes)
			{
				objFiles += file->FileNameNoExt() + ".o ";
				//if (!fileHasMain)
				//{
				//	avr_ar_targets += file->FileNameNoExt() + ".o ";
				//}
				//else
				//{
				//	mainFileName = file->FileNameNoExt() + ".o ";
				//}
			}
		}
	}

	// если есть ошибки при компил€ции
	// выходим
	if (!result)
		return false;

	// все объектные файлы сделаны, собираем их вместе в ELF файл
	bool elfRes = false;

	//if (mainFileName.IsEmpty())
	//{
		elfRes = CreateELF(objFiles, false);
	//}
	//else
	//{
	//	bool archiveRes = CreateLib(avr_ar_targets);
	//	if (archiveRes)
	//	{
	//		m_workingProject->m_linkerOpt += " lib" + m_workingProject->SafeFileNameNoExt() + ".a";
	//	}

	//	elfRes = CreateELF(objFiles, false);
	//}

	result &= elfRes;

	// удал€ем все мусорные файлы
	CleanOD(objFiles/* + " lib" + m_workingProject->SafeFileNameNoExt() + ".a"*/);

	// если все хорошо, генерируем hex дл€ прошивки
	if (elfRes)
	{
		result &= CreateHex();
		CreateEEP();
		//CreateLST();
		ReadSize();
	}

	return result;
}

bool ProjectBuilder::CreateLib(const wxString& avr_ar_targets)
{
	wxString outputAbsPath = m_workingProject->DirPath() + wxFILE_SEP_PATH + m_workingProject->m_outputDir;

	// создаем каталог
	if (!wxDirExists(outputAbsPath))
		if (!wxMkdir(outputAbsPath))
			return false;

	wxString libFilePath = outputAbsPath + wxFILE_SEP_PATH + "lib" + m_workingProject->SafeFileNameNoExt() + ".a";

	// удал€ем объектный файл, если есть
	if (wxFileExists(libFilePath))
	{
		if (!wxRemoveFile(libFilePath))
		{
			Error::LogMessage(m_outputTextCtrl, "####Error: library file could not be deleted at " + libFilePath);
		}
	}

	// конструируем опции и аргументы дл€ avr-gcc
	wxString args = "rcs lib" + m_workingProject->SafeFileNameNoExt() + ".a " + avr_ar_targets;
	Error::LogMessage(m_outputTextCtrl, "Execute: avr-ar " + args);
	wxArrayString output, error;
	WorkingDirectoryChanger dirChanger(outputAbsPath);
	wxExecute("avr-ar " + args, output, error);

	ReadErrAndWarning(output);
	ReadErrAndWarning(error);

	if (wxFileExists(libFilePath))
	{
		return true;
	}
	else
	{
		Error::LogMessage(m_outputTextCtrl, "####Error: library file not created at " + libFilePath);
		return false;
	}
}

bool ProjectBuilder::IsFileMain(ProjectFile *file)
{
/*
	wxString fileContents = GetFileContents(file);
	fileContents = CodePreProcess::StripComments(fileContents);
	fileContents.Replace("\r", " ");
	fileContents.Replace("\n", " ");

	wxString re1 = "(?:[a-z][a-z0-9_]*)";	// return type
	wxString re2 = "(\\s+)";	// White Space
	wxString re3 = "(main)";	// main
	wxString re4 = "(\\s*)";	// optional white space
	wxString re5 = "(\\()";	// (
	wxString re6 = "([^)]*)";	// anything goes
	wxString re7 = "(\\))";	// )
	wxString re8 = "(\\s*)";	// optional white space
	wxString re9 = "(\\{)";	// {

	wxRegEx r(re1 + re2 + re3 + re4 + re5 + re6 + re7 + re8 + re9);
	if (r.Matches(fileContents))
	{
		return true;
	}
*/
	return false;
}

bool ProjectBuilder::CreateELF(const wxString& OBJECTS, bool suppressErrors)
{
	wxString outputAbsPath = m_workingProject->DirPath() + wxFILE_SEP_PATH + m_workingProject->m_outputDir;

	// make sure folder exists, delete existing files

	if (!wxDirExists(outputAbsPath))
		return false;

	wxString elfFileAbsPath = outputAbsPath + wxFILE_SEP_PATH + m_workingProject->SafeFileNameNoExt() + ".elf";

	if (wxFileExists(elfFileAbsPath))
	{
		if (!wxRemoveFile(elfFileAbsPath))
		{
			Error::LogMessage(m_outputTextCtrl, "####Error: ELF file could not be deleted at " + elfFileAbsPath);
		}
	}

	wxString mapFileAbsPath = outputAbsPath + wxFILE_SEP_PATH + m_workingProject->SafeFileNameNoExt() + ".map";

	if (wxFileExists(mapFileAbsPath))
	{
		if (!wxRemoveFile(mapFileAbsPath))
		{
			Error::LogMessage(m_outputTextCtrl, "####Error: MAP file could not be deleted at " + mapFileAbsPath);
		}
	}

	// construct options and arguments

	wxString LDSFLAGS = "-mmcu=" + m_workingProject->m_device + " ";

	MemorySegmentHash::iterator it;
	for( it = m_workingProject->MemorySegList().begin(); it != m_workingProject->MemorySegList().end(); ++it )
	{
		wxString key = it->first;
		MemorySegment *seg = it->second;

		int addr = (int)seg->addr;
		if (seg->type == "sram")
		{
			addr += 0x800000;
		}
		else if (seg->type == "eeprom")
		{
			addr += 0x810000;
		}
		LDSFLAGS += "-Wl,-section-start=" + seg->name + "=0x" + wxString::Format("%X", addr) + " ";
	}


	if (m_workingProject->m_useInitStack)
	{
		LDSFLAGS += "-Wl,--defsym=__stack=0x" + wxString::Format("%X", m_workingProject->m_initStackAddr) + " ";
	}

	LDSFLAGS += "-Wl,-Map=" + m_workingProject->SafeFileNameNoExt() + ".map ";
	LDSFLAGS += "-Wl,--gc-sections ";
	LDSFLAGS += m_workingProject->m_optimization + " ";
	LDSFLAGS += m_workingProject->m_linkerOpt;

	wxString LINKONLYOBJECTS = "";
	for (size_t i = 0; i < m_workingProject->LinkObjList().GetCount(); ++i)
	{
		wxString obj = m_workingProject->LinkObjList()[i];
		if (!obj.IsEmpty())
		{
			LINKONLYOBJECTS += "\"" + obj + "\" ";
		}
	}

	wxString LIBS = "";
	for (size_t i = 0; i < m_workingProject->LinkLibList().GetCount(); ++i)
	{
		wxString obj = m_workingProject->LinkLibList()[i];
		if (!obj.IsEmpty())
		{
			if (obj.StartsWith("lib"))
			{
				LIBS += "-l" + obj.Mid(3).BeforeLast('.') + " ";
			}
			else
			{
				wxString libName = wxFileName(obj).GetName();

				if (libName.StartsWith("lib"))
					libName = libName.Mid(3);

				wxString libPath = wxFileName(obj).GetPath();

				LIBS += "-l" + libName + " ";

				if (m_workingProject->LibraryDirList().Index(libPath) == wxNOT_FOUND)
					m_workingProject->LibraryDirList().Add(libPath);
			}
		}
	}

	wxString LIBDIRS = "";
	for (size_t i = 0; i < m_workingProject->LibraryDirList().GetCount(); ++i)
	{
		wxString obj = m_workingProject->LibraryDirList()[i];
		if (!obj.IsEmpty())
		{
			LIBDIRS += "-L\"" + obj + "\" ";
		}
	}

	wxString args = wxString::Format("%s %s %s %s %s -o %s.elf",
		LDSFLAGS.Trim().c_str(),
		OBJECTS.c_str(),
		LINKONLYOBJECTS.Trim().c_str(),
		LIBDIRS.Trim().c_str(),
		LIBS.Trim().c_str(),
		m_workingProject->SafeFileNameNoExt().c_str()
	);

	Error::LogMessage(m_outputTextCtrl, "Linking elf: avr-gcc " + args);

	// link object files together to get .elf file

	wxArrayString output, error;

 	WorkingDirectoryChanger dirChanger(outputAbsPath);
	wxExecute("avr-gcc " + args, output, error);

	ReadErrAndWarning(output);
	ReadErrAndWarning(error);

	if (wxFileExists(elfFileAbsPath))
	{
		if (!wxFileExists(mapFileAbsPath))
		{
			if (suppressErrors == false)
				Error::LogMessage(m_outputTextCtrl, "####Error: MAP file not created at " + elfFileAbsPath);
		}

		return true;
	}
	else
	{
		if (suppressErrors == false)
			Error::LogMessage(m_outputTextCtrl, "####Error: ELF file not created at " + elfFileAbsPath);
		return false;
	}
}

void ProjectBuilder::CleanOD(const wxString& OBJECTS)
{
	wxString outputAbsPath = m_workingProject->DirPath() + wxFILE_SEP_PATH + m_workingProject->m_outputDir;

	// .d files are also generated by avr-gcc, so delete those too
	wxString DFILES = OBJECTS;
	DFILES.Replace(".o ", ".d ");

	wxString args = OBJECTS + " " + DFILES;

	wxArrayString deleted;

	Error::LogMessage(m_outputTextCtrl, "Deleting: " + args);

    wxArrayString fList = wxStringTokenize(args, " ");

	for (size_t i = 0; i < fList.GetCount(); ++i)
	{
		wxString file = fList[i];

		if (!file.IsEmpty())
		{
			if (wxFileExists(m_workingProject->DirPath() + wxFILE_SEP_PATH + file))
			{
				if (wxRemoveFile(m_workingProject->DirPath() + wxFILE_SEP_PATH + file))
					deleted.Add(file);
				else
					Error::LogMessage(m_outputTextCtrl, "Error Deleting '" + args);
			}

			if (wxFileExists(outputAbsPath + wxFILE_SEP_PATH + file))
			{

				if (wxRemoveFile(outputAbsPath + wxFILE_SEP_PATH + file))
					deleted.Add(file);
				else
					Error::LogMessage(m_outputTextCtrl, "Error Deleting '" + args);
			}
		}
	}
	wxString deletedStr = "";
	for (size_t i = 0; i < deleted.GetCount(); ++i)
	{
		deletedStr += "'" + deleted[i] + "', ";
	}

	if (deletedStr.IsEmpty())
		Error::LogMessage(m_outputTextCtrl, "Deleted Nothing");
	else
		Error::LogMessage(m_outputTextCtrl, "Deleted: " + deletedStr);
}

bool ProjectBuilder::CreateHex()
{
	wxString outputAbsPath = m_workingProject->DirPath() + wxFILE_SEP_PATH + m_workingProject->m_outputDir;

	if (!wxDirExists(outputAbsPath))
		return false;

	wxString hexFileAbsPath = outputAbsPath + wxFILE_SEP_PATH + m_workingProject->SafeFileNameNoExt() + ".hex";

	if (wxFileExists(hexFileAbsPath))
	{
		if (!wxRemoveFile(hexFileAbsPath))
		{
			Error::LogMessage(m_outputTextCtrl, "####Error: HEX file could not be deleted at " + hexFileAbsPath);
		}
	}

	wxString HEX_FLASH_FLAGS = "-R .eeprom -R .fuse -R .lock -R .signature ";
	wxString args = HEX_FLASH_FLAGS + "-O ihex "
		+ m_workingProject->SafeFileNameNoExt() + ".elf "
		+ m_workingProject->SafeFileNameNoExt() + ".hex";
	Error::LogMessage(m_outputTextCtrl, "Execute: avr-objcopy " + args);

	wxArrayString output, error;

 	WorkingDirectoryChanger dirChanger(outputAbsPath);
	wxExecute("avr-objcopy " + args, output, error);

	ReadErrAndWarning(output);
	ReadErrAndWarning(error);

	if (wxFileExists(hexFileAbsPath))
	{
		return true;
	}
	else
	{
		Error::LogMessage(m_outputTextCtrl, "####Error: HEX file not created at " + hexFileAbsPath);
		return false;
	}
}

bool ProjectBuilder::CreateEEP()
{
	wxString outputAbsPath = m_workingProject->DirPath() + wxFILE_SEP_PATH + m_workingProject->m_outputDir;

	if (!wxDirExists(outputAbsPath))
		return false;

	wxString eepFileAbsPath = outputAbsPath + wxFILE_SEP_PATH + m_workingProject->SafeFileNameNoExt() + ".eep";

	if (wxFileExists(eepFileAbsPath))
	{
		if (!wxRemoveFile(eepFileAbsPath))
		{
			Error::LogMessage(m_outputTextCtrl, "####Error: EEP file could not be deleted at " + eepFileAbsPath);
		}
	}

	wxString HEX_EEPROM_FLAGS = "-j .eeprom --set-section-flags=.eeprom=\"alloc,load\" --change-section-lma .eeprom=0 --no-change-warnings ";

	MemorySegmentHash::iterator it;
	for( it = m_workingProject->MemorySegList().begin(); it != m_workingProject->MemorySegList().end(); ++it )
	{
		wxString key = it->first;
		MemorySegment *seg = it->second;
		if (seg->type == "eeprom")
			HEX_EEPROM_FLAGS += "--change-section-lma " + seg->name + "=0x" + wxString::Format("%X", seg->addr) + " ";
	}

	wxString args = HEX_EEPROM_FLAGS + " -O ihex "
		+ m_workingProject->SafeFileNameNoExt() + ".elf "
		+ m_workingProject->SafeFileNameNoExt() + ".eep";

	Error::LogMessage(m_outputTextCtrl, "Execute: avr-objcopy " + args);

	wxArrayString output, error;
	WorkingDirectoryChanger dirChanger(outputAbsPath);
	wxExecute("avr-objcopy " + args, output, error);

	ReadErrAndWarning(output);
	ReadErrAndWarning(error);

	if (wxFileExists(eepFileAbsPath))
	{
		return true;
	}
	else
	{
		Error::LogMessage(m_outputTextCtrl, "####Error: EEP file not created at " + eepFileAbsPath);
		return false;
	}

}

bool ProjectBuilder::CreateLST()
{
	wxString outputAbsPath = m_workingProject->DirPath() + wxFILE_SEP_PATH + m_workingProject->m_outputDir;

	if (!wxDirExists(outputAbsPath))
		return false;

	wxString lstFileAbsPath = outputAbsPath + wxFILE_SEP_PATH + m_workingProject->SafeFileNameNoExt() + ".lst";

	if (wxFileExists(lstFileAbsPath))
	{
		if (!wxRemoveFile(lstFileAbsPath))
		{
			Error::LogMessage(m_outputTextCtrl, "####Error: LSS file could not be deleted at " + lstFileAbsPath);
		}
	}


	wxString args = "-h -S "
		+ m_workingProject->SafeFileNameNoExt() + ".elf > "
		+ m_workingProject->SafeFileNameNoExt() + ".lst";

	Error::LogMessage(m_outputTextCtrl, "Execute: avr-objdump " + args);

	wxArrayString output, error;
	WorkingDirectoryChanger dirChanger(outputAbsPath);
	wxExecute("avr-objcopy " + args, output, error);

	ReadErrAndWarning(output);
	ReadErrAndWarning(error);

	if (wxFileExists(lstFileAbsPath))
	{
		return true;
	}
	else
	{
		Error::LogMessage(m_outputTextCtrl, "####Error: LST file not created at " + lstFileAbsPath);
		return false;
	}
 }

void ProjectBuilder::ReadSize()
{
	wxString outputAbsPath = m_workingProject->DirPath() + wxFILE_SEP_PATH + m_workingProject->m_outputDir;

	wxString args = "-C --mcu="
		+ m_workingProject->m_device.Lower() + " " + m_workingProject->SafeFileNameNoExt() + ".elf";

	Error::LogMessage(m_outputTextCtrl, "Execute: avr-size " + args);
	wxArrayString output, error;
	WorkingDirectoryChanger dirChanger(outputAbsPath);
	wxExecute("avr-size " + args, output, error);

	ReadErrAndWarning(output);
	ReadErrAndWarning(error);
}

void ProjectBuilder::ReadErrAndWarning(wxArrayString reader)
{
	// в этой функции происходит основной парсинг ошибок
	// при помощи регул€рных выражений и заполн€ютс€
	// списки m_errorList и m_errorOnlyList
	// функци€ вызываетс€ после каждого вызова wxExecute
	Error error(m_outputTextCtrl, m_errorList);
	error.Check(reader);
}

wxArrayString ProjectBuilder::GetAllFunctProto()
{
	wxArrayString tmp;
	wxLogVerbose("ProjectBuilder::GetAllFunctProto : Not Implemented");
	return tmp;
}

/*
wxString ProjectBuilder::GetFileContents(ProjectFile *file)
{
	wxFFile ff(file->m_fileAbsPath);

	wxString content;

	if (ff.IsOpened())
	{
		ff.ReadAll(&content);
	}
	else
		TextBoxModify(m_outputTextCtrl, "####Error while retrieving file contents of " + file->FileName());

	return content;
}
*/

void ProjectBuilder::PrepProject()
{
	delete m_workingProject;
//	delete m_workingFileList;


	// clone the project
	m_workingProject = m_project->Clone();

	// make a clone of the file list
	m_workingFileList = m_workingProject->FileList();
}

// --- ProjectBurner

ProjectBurner::ProjectBurner(AVRProject *project)
{
	m_project = project;
}


void ProjectBurner::BurnCMD(bool onlyOptions, bool burnFuses, wxPanel *formObj)
{
	// construct appropriate arguments

	wxString fileStr = "";
	if (onlyOptions == false && burnFuses == false)
		fileStr = wxString::Format(	"-U flash:w:\"%s\\%s\\%s.hex\":a",
									m_project->DirPath().c_str(),
									m_project->m_outputDir.c_str(),
									m_project->SafeFileNameNoExt().c_str());

	if (burnFuses)
		fileStr = m_project->m_burnFuseBox;

	wxString overrides = "";

	BurnerPanel::GetPortOverride(overrides, m_project);

	wxString args = wxString::Format(	"avrdude -p %s -c %s %s %s %s",
										m_project->m_burnPart.Upper().c_str(),
										m_project->m_burnProgrammer.c_str(),
										overrides.c_str(),
										m_project->m_burnOptions.c_str(),
										fileStr.c_str());

	if (wxExecute("cmd /k " + args, wxEXEC_SYNC|wxEXEC_NOHIDE) < 0)
	{
		wxMessageBox("Error, Unable to Start AVRDUDE");
	}
}

wxArrayString ProjectBurner::GetAvailParts(wxString progname, bool suppressErr)
{
	wxLogVerbose("ProjectBurner::GetAvailParts");

	wxArrayString res, tmp;

	if (wxExecute("avrdude -c " + progname + " -p list", tmp, res) < 0)
	{
		if (!suppressErr)
		{
			wxMessageBox("Error, Unable to Start AVRDUDE");
		}
	}
	{
		tmp.Clear();
		for (size_t i = 0; i < res.GetCount(); ++i)
		{
			wxString prog = res[i];
			if (prog.Find("=") != wxNOT_FOUND)
			{
				prog = prog.AfterFirst('=').BeforeFirst('[').Trim().Trim(false);
				if (!prog.IsEmpty())
					tmp.Add(prog);
			}
		}
	}

	return tmp;
}

wxArrayString ProjectBurner::GetAvailProgrammers(bool suppressErr)
{
	wxLogVerbose("ProjectBurner::GetAvailProgrammers");

	wxArrayString res, tmp;

	if (wxExecute("avrdude -c list", tmp, res) < 0)
	{
		if (!suppressErr)
		{
			wxMessageBox("Error, Unable to Start AVRDUDE");
		}
	}
	else
	{
		tmp.Clear();
		for (size_t i = 0; i < res.GetCount(); ++i)
		{
			wxString prog = res[i];
			if (prog.Find("=") != wxNOT_FOUND)
			{
				prog = prog.BeforeFirst('=').Trim().Trim(false);
				if (!prog.IsEmpty())
					tmp.Add(prog);
			}
		}
	}


	return tmp;
}

wxArrayString ProjectBurner::GetPartWords(wxString output, wxArrayString& res)
{
	wxArrayString tmp;
	return tmp;
}

wxArrayString ProjectBurner::GetFirstWords(wxString output, wxArrayString& res)
{
	wxArrayString tmp;
	return tmp;
}




