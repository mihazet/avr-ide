#include "ProjectManagment.h"
//#include "Settings.h"

#include <wx/xml/xml.h>
#include <wx/confbase.h>

#include "XmlUtils.h"

// --- ProjectFile

ProjectFile::ProjectFile(const wxString fileAbsPath, AVRProject *project)
{
	m_project = project;
	m_fileAbsPath = fileAbsPath;
	if (FileExt() == "c" || FileExt() == "S" || FileExt() == "cpp" || FileExt() == "cxx")
		m_toCompile = true;
	m_options = "";
	m_isOpen = false;
	m_cursorPos = 0;
}

// --- AVRProject

AVRProject::AVRProject()
{
	Reset();
}

bool AVRProject::Save()
{
	if (!m_filePath.IsEmpty())
	{
		SetFilePath(m_filePath);
		return Save(m_filePath);
	}
	else
	{
		wxLogVerbose("OOPS: Empty project file name");
		return false;
	}
}

bool AVRProject::Save(const wxString& filepath)
{
	wxXmlDocument xmlWriter;

	wxXmlNode *node, *node1;
	wxXmlNode *rootNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Project");

	//SetFilePath(filepath);

	XmlUtils::WriteElementString(rootNode, "DirPath", m_dirPath);
	XmlUtils::WriteElementString(rootNode, "HasBeenConfigged", m_hasBeenConfigged ? "true": "false");
	XmlUtils::WriteElementString(rootNode, "Device", m_device);
	XmlUtils::WriteElementString(rootNode, "ClockFreq", wxString::Format("%d", m_clkFreq));
	XmlUtils::WriteElementString(rootNode, "LinkerOpt", m_linkerOpt);
	XmlUtils::WriteElementString(rootNode, "OtherOpt", m_otherOpt);
	XmlUtils::WriteElementString(rootNode, "OtherOptionsForC", m_otherOptForC);
	XmlUtils::WriteElementString(rootNode, "OtherOptionsForCPP", m_otherOptForCPP);
	XmlUtils::WriteElementString(rootNode, "OtherOptionsForS", m_otherOptForS);
	XmlUtils::WriteElementString(rootNode, "OutputDir", m_outputDir);
	XmlUtils::WriteElementString(rootNode, "Optimization", m_optimization);
	XmlUtils::WriteElementString(rootNode, "UseInitStack", m_useInitStack ? "true": "false");
	XmlUtils::WriteElementString(rootNode, "InitStackAddr", wxString::Format("0x%04X", m_initStackAddr));
	XmlUtils::WriteElementString(rootNode, "PackStructs", m_packStructs ? "true": "false");
	XmlUtils::WriteElementString(rootNode, "ShortEnums", m_shortEnums ? "true": "false");
	XmlUtils::WriteElementString(rootNode, "UnsignedBitfields", m_unsignedBitfields ? "true": "false");
	XmlUtils::WriteElementString(rootNode, "UnsignedChars", m_unsignedChars ? "true": "false");
	XmlUtils::WriteElementString(rootNode, "FunctionSections", m_functSects ? "true": "false");
	XmlUtils::WriteElementString(rootNode, "DataSections", m_dataSects ? "true": "false");

	node = XmlUtils::WriteElement(rootNode, "IncludeDirList");
	for (size_t i = 0; i < m_includeDirList.GetCount(); ++i)
		XmlUtils::WriteElementString(node, "DirPath", m_includeDirList[i]);

	node = XmlUtils::WriteElement(rootNode, "LibraryDirList");
	for (size_t i = 0; i < m_libraryDirList.GetCount(); ++i)
		XmlUtils::WriteElementString(node, "DirPath", m_libraryDirList[i]);

	node = XmlUtils::WriteElement(rootNode, "LinkObjList");
	for (size_t i = 0; i < m_linkObjList.GetCount(); ++i)
		XmlUtils::WriteElementString(node, "Obj", m_linkObjList[i]);

	node = XmlUtils::WriteElement(rootNode, "LinkLibList");
	for (size_t i = 0; i < m_linkObjList.GetCount(); ++i)
		XmlUtils::WriteElementString(node, "Lib", m_linkObjList[i]);

	node = XmlUtils::WriteElement(rootNode, "MemorySegList");
	MemorySegmentHash::iterator it;
	for( it = m_memorySegList.begin(); it != m_memorySegList.end(); ++it )
	{
		wxString key = it->first;
		MemorySegment *seg = it->second;

		node1 = XmlUtils::WriteElement(node, "Segment");
		XmlUtils::WriteElementString(node1, "Name", seg->name);
		XmlUtils::WriteElementString(node1, "Type", seg->type);
		XmlUtils::WriteElementString(node1, "Addr", wxString::Format("0x%X", seg->addr));
	}


	XmlUtils::WriteElementString(rootNode, "BurnPart", m_burnPart);
	XmlUtils::WriteElementString(rootNode, "BurnProgrammer", m_burnProgrammer);
	XmlUtils::WriteElementString(rootNode, "BurnOptions", m_burnOptions);
	XmlUtils::WriteElementString(rootNode, "BurnPort", m_burnPort);
	XmlUtils::WriteElementString(rootNode, "BurnFuseBox", m_burnFuseBox);
	XmlUtils::WriteElementString(rootNode, "BurnBaud", wxString::Format("%d", m_burnBaud));
	XmlUtils::WriteElementString(rootNode, "BurnAutoReset", m_burnAutoReset ? "true": "false");
	XmlUtils::WriteElementString(rootNode, "LastFile", m_lastFile);

	node = XmlUtils::WriteElement(rootNode, "FileList");

	// iterate over all the elements in the class
	ProjectFileHash::iterator it1;
	for( it1 = m_fileList.begin(); it1 != m_fileList.end(); ++it1 )
	{
		wxString key = it1->first;
		ProjectFile *file = it1->second;

		node1 = XmlUtils::WriteElement(node, "File");
		XmlUtils::WriteElementString(node1, "RelPath", file->FileRelPathTo(m_dirPath));
		XmlUtils::WriteElementString(node1, "ToCompile", file->ToCompile() ? "true": "false");
		XmlUtils::WriteElementString(node1, "Options", "");
		XmlUtils::WriteElementString(node1, "WasOpen", file->IsOpen() ? "true": "false");
		XmlUtils::WriteElementString(node1, "CursorPos", wxString::Format("%d", file->m_cursorPos));
		XmlUtils::WriteElementString(node1, "Bookmarks", "");
	}

	xmlWriter.SetRoot(rootNode);
	xmlWriter.SetFileEncoding("windows-1251");
	xmlWriter.SetEncoding("windows-1251");

	if (!xmlWriter.Save(filepath))
		return false;

	return true;
}

bool AVRProject::Open(const wxString& filepath)
{
	wxLogVerbose("AVRProject::Open");

	wxXmlDocument xml;

	if (!xml.Load(filepath, "windows-1251"))
		return false;

	SetFilePath(filepath);
	m_isReady = true;

	wxXmlNode *rootNode = xml.GetRoot();

	// properties
	m_dirPath = XmlUtils::ReadElementStringByTag(rootNode, "DirPath");
	m_hasBeenConfigged = XmlUtils::ReadElementStringByTag(rootNode, "HasBeenConfigged") == "true";
	m_device = XmlUtils::ReadElementStringByTag(rootNode, "Device");
	m_clkFreq = wxAtoi(XmlUtils::ReadElementStringByTag(rootNode, "ClockFreq"));
	m_linkerOpt =XmlUtils:: ReadElementStringByTag(rootNode, "LinkerOpt");
	m_otherOpt = XmlUtils::ReadElementStringByTag(rootNode, "OtherOpt");
	m_otherOptForC = XmlUtils::ReadElementStringByTag(rootNode, "OtherOptionsForC");
	m_otherOptForCPP = XmlUtils::ReadElementStringByTag(rootNode, "OtherOptionsForCPP");
	m_otherOptForS = XmlUtils::ReadElementStringByTag(rootNode, "OtherOptionsForS");
	m_outputDir = XmlUtils::ReadElementStringByTag(rootNode, "OutputDir");
	m_optimization = XmlUtils::ReadElementStringByTag(rootNode, "Optimization");
	m_useInitStack = XmlUtils::ReadElementStringByTag(rootNode, "UseInitStack");
	m_initStackAddr = wxAtoi(XmlUtils::ReadElementStringByTag(rootNode, "InitStackAddr"));
	m_packStructs = XmlUtils::ReadElementStringByTag(rootNode, "PackStructs") == "true";
	m_shortEnums = XmlUtils::ReadElementStringByTag(rootNode, "ShortEnums") == "true";
	m_unsignedBitfields = XmlUtils::ReadElementStringByTag(rootNode, "UnsignedBitfields") == "true";
	m_unsignedChars = XmlUtils::ReadElementStringByTag(rootNode, "UnsignedChars") == "true";
	m_functSects = XmlUtils::ReadElementStringByTag(rootNode, "FunctionSections") == "true";
	m_dataSects = XmlUtils::ReadElementStringByTag(rootNode, "DataSections") == "true";

	// List
	m_includeDirList.Clear();
	m_libraryDirList.Clear();
	m_linkObjList.Clear();
	m_linkLibList.Clear();
	m_memorySegList.clear();

	wxXmlNode *includeDirListNode = XmlUtils::ReadElementByTag(rootNode, "IncludeDirList");
	if (includeDirListNode)
	{
		wxXmlNode *node = includeDirListNode->GetChildren();
		while (node)
		{
			if (node->GetName() == "DirPath")
			{
				m_includeDirList.Add(node->GetNodeContent());
			}
			node = node->GetNext();
		}
	}

	wxXmlNode *libraryDirList = XmlUtils::ReadElementByTag(rootNode, "LibraryDirList");
	if (libraryDirList)
	{
		wxXmlNode *node = libraryDirList->GetChildren();
		while (node)
		{
			if (node->GetName() == "DirPath")
			{
				m_libraryDirList.Add(node->GetNodeContent());
			}
			node = node->GetNext();
		}
	}

	wxXmlNode *linkObjListNode = XmlUtils::ReadElementByTag(rootNode, "LinkObjList");
	if (linkObjListNode)
	{
		wxXmlNode *node = linkObjListNode->GetChildren();
		while (node)
		{
			if (node->GetName() == "Obj")
			{
				m_linkObjList.Add(node->GetNodeContent());
			}
			node = node->GetNext();
		}
	}

	wxXmlNode *linkLibListNode = XmlUtils::ReadElementByTag(rootNode, "LinkLibList");
	if (linkLibListNode)
	{
		wxXmlNode *node = linkLibListNode->GetChildren();
		while (node)
		{
			if (node->GetName() == "Lib")
			{
				m_linkLibList.Add(node->GetNodeContent());
			}
			node = node->GetNext();
		}
	}

	wxXmlNode *memorySegListNode = XmlUtils::ReadElementByTag(rootNode, "MemorySegList");
	if (memorySegListNode)
	{
		wxXmlNode *node = memorySegListNode->GetChildren();
		while (node)
		{
			if (node->GetName() == "Segment")
			{
				m_linkLibList.Add(node->GetNodeContent());
				wxString name = XmlUtils::ReadElementStringByTag(node, "Name");
				wxString type = XmlUtils::ReadElementStringByTag(node, "type");
				long addr;
				XmlUtils::ReadElementStringByTag(node, "Addr").ToLong(&addr, 16);

				MemorySegment *seg = new MemorySegment(type, name, addr);
				m_memorySegList[name] = seg;
			}
			node = node->GetNext();
		}
	}



	// Burn
	m_burnPart = XmlUtils::ReadElementStringByTag(rootNode, "BurnPart");
	m_burnProgrammer = XmlUtils::ReadElementStringByTag(rootNode, "BurnProgrammer");
	m_burnOptions = XmlUtils::ReadElementStringByTag(rootNode, "BurnOptions");
	m_burnPort = XmlUtils::ReadElementStringByTag(rootNode, "BurnPort");
	m_burnFuseBox = XmlUtils::ReadElementStringByTag(rootNode, "BurnFuseBox");
	m_burnBaud = wxAtoi(XmlUtils::ReadElementStringByTag(rootNode, "BurnBaud"));
	m_burnAutoReset = XmlUtils::ReadElementStringByTag(rootNode, "BurnAutoReset") == "true";
	m_lastFile = XmlUtils::ReadElementStringByTag(rootNode, "LastFile");


	// FileList
	m_fileList.clear();

	wxXmlNode *fileListNode = XmlUtils::ReadElementByTag(rootNode, "FileList");
	if (fileListNode)
	{
		wxXmlNode *fileNode = fileListNode->GetChildren();
		while (fileNode)
		{
			if (fileNode->GetName() == "File")
			{
				wxString relPath = XmlUtils::ReadElementStringByTag(fileNode, "RelPath");
				wxFileName fileName(relPath);
				wxLogVerbose("m_dirPath: %s", m_dirPath.c_str());

				if (fileName.MakeAbsolute(m_dirPath))
				{
					wxLogVerbose("fileName.GetFullPath(): %s", fileName.GetFullPath().c_str());
					ProjectFile *newFile = new ProjectFile(fileName.GetFullPath(), this);
					newFile->SetToCompile(XmlUtils::ReadElementStringByTag(fileNode, "ToCompile") == "true");
					newFile->SetIsOpen(XmlUtils::ReadElementStringByTag(fileNode, "WasOpen") == "true");
					long pos;
					if (XmlUtils::ReadElementStringByTag(fileNode, "CursorPos").ToLong(&pos))
						newFile->m_cursorPos = pos;
					m_fileList[newFile->FileName()] = newFile;
				}
			}
			fileNode = fileNode->GetNext();
		}
	}

	return true;
}

void AVRProject::Reset()
{
	//m_fileName = "";
	m_hasBeenConfigged = false;
	m_clkFreq = wxConfigBase::Get()->Read("/project/last_clock_choosen", 8000000);
	m_device = wxConfigBase::Get()->Read("/project/last_chip_choosen", "atmega16");
	m_packStructs = true;
	m_shortEnums = true;
	m_unsignedBitfields = true;
	m_unsignedChars = true;
	m_functSects = true;
	m_dataSects = true;
	m_useInitStack = false;
	m_initStackAddr = 0xffff;
	m_otherOpt = "-Wall -gdwarf-2";
	m_otherOptForC = "-std=gnu99";
	m_otherOptForCPP = "-std=c99";
	m_otherOptForS = "";
	m_linkerOpt = "";
	m_outputDir = "output";
	m_optimization = "-Os";

	m_burnPart = m_device;
	m_burnProgrammer = wxConfigBase::Get()->Read("/project/last_prog_choosen", "usbasp");
	m_burnOptions = "";
	m_burnBaud = wxConfigBase::Get()->Read("/project/last_prog_baud", 0l);
	if (wxConfigBase::Get()->Read("/project/last_prog_port_choosen", "COM1") == "nooverride")
		m_burnPort = "";
	else
		m_burnPort = wxConfigBase::Get()->Read("/project/last_prog_port_choosen", "COM1");
	m_burnFuseBox = "";
	m_burnAutoReset = wxConfigBase::Get()->Read("/project/last_prog_auto_reset", 0l);

	m_iCEHardware = wxConfigBase::Get()->Read("/project/last_ice_hardware", "JTAG ICE mkI");
	m_jTAGFreq = wxConfigBase::Get()->Read("/project/last_jtag_freq", 0l);

	m_lastFile = "";

	m_fileList.clear();
	m_memorySegList.clear();
	m_includeDirList.Clear();
	m_libraryDirList.Clear();
	m_linkObjList.Clear();
	m_linkLibList.Clear();
	//APSXmlElementList.Clear();

	m_isReady = false;
}

bool AVRProject::CreateNew(const wxString& filepath)
{
	SetFilePath(filepath);
	m_isReady = true;
	return Save();
}

