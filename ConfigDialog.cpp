#include "ConfigDialog.h"
#include "ProjectManagment.h"
#include "BurnerPanel.h"
#include "DefsExt.h"

#include <wx/notebook.h>
#include <wx/dir.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/confbase.h>


BEGIN_EVENT_TABLE(ConfigDialog, wxDialog)
	EVT_BUTTON(wxID_OK,	ConfigDialog::OnOk)

	EVT_LISTBOX_DCLICK(ID_INCLUDE_PATH, 	ConfigDialog::OnIncludePathsDClick)
	EVT_LISTBOX_DCLICK(ID_LIBRARY_PATH, 	ConfigDialog::OnLibraryPathsDClick)

	EVT_BUTTON(ID_INCLUDE_PATH_ADD,			ConfigDialog::OnIncludeDirAdd)
	EVT_BUTTON(ID_LIBRARY_PATH_ADD,			ConfigDialog::OnLibraryDirAdd)
END_EVENT_TABLE()


ConfigDialog::ConfigDialog(AVRProject *prj, wxWindow *parent, const wxString& title)
	: wxDialog(parent, wxID_ANY, title)
{
	project = prj->Clone();
	originalProject = prj;

	CreateControls();

	if (orderedDevices.GetCount() == 0)
	{
		for (size_t i = 0; i < dropDevices->GetCount(); ++i)
		{
			wxString s = dropDevices->GetString(i);
			if (orderedDevices.Index(s) == wxNOT_FOUND)
			{
				orderedDevices.Add(s);
			}
		}

/*
		wxString pathToXmls = wxGetCwd() + wxFILE_SEP_PATH + "chip_xml" + wxFILE_SEP_PATH;
		if (wxDirExists(pathToXmls))
		{
			wxDir dir(pathToXmls);
			wxString filename;
			bool cont = dir.GetFirst(&filename, "*.xml", wxDIR_FILES);
			while (cont)
			{
				if (wxFileName(filename).GetFullName() != "interruptvectors.xml")
				{
					if (filename.EndsWith(".xml"))
					{
						wxString name = wxFileName(filename).GetName();
						if (orderedDevices.Index(name) == wxNOT_FOUND)
							orderedDevices.Add(name);
					}
				}
				cont = dir.GetNext(&filename);
			}
		}
*/
		//wxString fileChips = "chips.txt";
		//if (wxFileExists(fileChips))
		//{
		//	wxFileInputStream fis(fileChips);
		//	wxTextInputStream text(fis);

			//while (fis.IsOk())
			//{
			//	wxString name = text.ReadLine().Lower();
			//	if (!name.IsEmpty() && orderedDevices.Index(name) == wxNOT_FOUND)
			//		orderedDevices.Add(name);
			//}
		//}

		wxString chips = g_chips + wxConfigBase::Get()->Read("/chips/list", "");

		orderedDevices = wxStringTokenize(chips, " ");
		orderedDevices.Sort();
	}

	dropDevices->Clear();
	for (size_t i = 0; i < orderedDevices.GetCount(); ++i)
		dropDevices->Append(orderedDevices[i]);

	project->m_hasBeenConfigged = true;
	originalProject->m_hasBeenConfigged = true;

	PopulateForm();
	Center();
}

ConfigDialog::~ConfigDialog()
{
	delete project;
}

// --- Events

void ConfigDialog::OnOk(wxCommandEvent& event)
{

	ApplyChanges();

	if (!originalProject->Save())
		wxMessageBox("Error saving project");
	else
		event.Skip();
 }

void ConfigDialog::OnIncludePathsDClick(wxCommandEvent& event)
{
	wxString def = listIncludePaths->GetStringSelection();

	wxString path = wxGetTextFromUser("Enter path:", "Include", def, this);
	if (!path.IsEmpty())
	{
		if (listIncludePaths->GetSelection() >= 0)
		{
			listIncludePaths->SetString(listIncludePaths->GetSelection(), path);
		}
	}
}

void ConfigDialog::OnLibraryPathsDClick(wxCommandEvent& event)
{
	wxString def = listLibraryPaths->GetStringSelection();

	wxString path = wxGetTextFromUser("Enter path:", "Library", def, this);
	if (!path.IsEmpty())
	{
		if (listLibraryPaths->GetSelection() >= 0)
		{
			listLibraryPaths->SetString(listLibraryPaths->GetSelection(), path);
		}
	}
}

void ConfigDialog::OnIncludeDirAdd(wxCommandEvent& event)
{
	wxString path = wxGetTextFromUser("Enter path:", "Include", "", this);
	if (!path.IsEmpty())
	{
		listIncludePaths->Append(path);
	}
}

void ConfigDialog::OnLibraryDirAdd(wxCommandEvent& event)
{
	wxString path = wxGetTextFromUser("Enter path:", "Library", "", this);
	if (!path.IsEmpty())
	{
		listLibraryPaths->Append(path);
	}
}

// --- Private Metods

void ConfigDialog::CreateControls()
{
	wxNotebook *book = new wxNotebook(this, wxID_ANY);

	// panel 1
	wxPanel *panel1 = new wxPanel(book);
	book->AddPage(panel1, "General");

	txtOutputPath = new wxTextCtrl(panel1, wxID_ANY, "");
	dropDevices = new wxChoice(panel1, wxID_ANY);
	numClockFreq = new wxSpinCtrl(panel1, wxID_ANY);
	listOptimization = new wxChoice(panel1, wxID_ANY);
	txtOtherOptions = new wxTextCtrl(panel1, wxID_ANY, "");
	txtCOptions = new wxTextCtrl(panel1, wxID_ANY, "");
	txtCPPOptions = new wxTextCtrl(panel1, wxID_ANY, "");
	txtSOptions = new wxTextCtrl(panel1, wxID_ANY, "");

	chkUnsignedChars = new wxCheckBox(panel1, wxID_ANY, "Unsigned Chars (-funsigned-char)");
	chkUnsignedBitfields = new wxCheckBox(panel1, wxID_ANY, "Unsigned Bitfields (-funsigned-bitfields)");
	chkPackStructureMembers = new wxCheckBox(panel1, wxID_ANY, "Pack Structure Members (-fpack-struct)");
	chkShortEnums = new wxCheckBox(panel1, wxID_ANY, "Short Enums (-fshort-enums)");
	chkFunctionSections = new wxCheckBox(panel1, wxID_ANY, "Function Sections (-function-sections)");
	chkDataSections = new wxCheckBox(panel1, wxID_ANY, "Data Sections (-fdata-sections)");

	burnerPanel = new BurnerPanel(project, panel1);

	// panel 1 layout
	wxBoxSizer *sizerPanel1 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *topSizerPanel1 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *rightSizerPanel1 = new wxBoxSizer(wxVERTICAL);
	wxFlexGridSizer *gridszr1Panel1 = new wxFlexGridSizer(2, 5, 5);
	wxFlexGridSizer *gridszr2Panel1 = new wxFlexGridSizer(2, 5, 5);
	wxStaticBoxSizer *szrBurnerPanel1 = new wxStaticBoxSizer(wxVERTICAL, panel1, "AVRDUDE Command Builder");

	szrBurnerPanel1->Add(burnerPanel, 1, wxEXPAND|wxALL, 5);

	gridszr1Panel1->Add(new wxStaticText(panel1, wxID_ANY, "Output Path:"), 0, wxALIGN_RIGHT);
	gridszr1Panel1->Add(txtOutputPath, 0, wxEXPAND);
	gridszr1Panel1->Add(new wxStaticText(panel1, wxID_ANY, "Device:"), 0, wxALIGN_RIGHT);
	gridszr1Panel1->Add(dropDevices, 0, wxEXPAND);
	gridszr1Panel1->Add(new wxStaticText(panel1, wxID_ANY, "Clock Freq (Hz):"), 0, wxALIGN_RIGHT);
	gridszr1Panel1->Add(numClockFreq, 0, wxEXPAND);
	gridszr1Panel1->Add(new wxStaticText(panel1, wxID_ANY, "Optimization:"), 0, wxALIGN_RIGHT);
	gridszr1Panel1->Add(listOptimization, 0, wxEXPAND);

	gridszr2Panel1->AddGrowableCol(1);
	gridszr2Panel1->Add(new wxStaticText(panel1, wxID_ANY, "Other Options:"), 0, wxALIGN_RIGHT);
	gridszr2Panel1->Add(txtOtherOptions, 1, wxEXPAND);
	gridszr2Panel1->Add(new wxStaticText(panel1, wxID_ANY, "C Only Options:"), 0, wxALIGN_RIGHT);
	gridszr2Panel1->Add(txtCOptions, 1, wxEXPAND);
	gridszr2Panel1->Add(new wxStaticText(panel1, wxID_ANY, "CPP Only Options:"), 0, wxALIGN_RIGHT);
	gridszr2Panel1->Add(txtCPPOptions, 1, wxEXPAND);
	gridszr2Panel1->Add(new wxStaticText(panel1, wxID_ANY, "S Only Options:"), 0, wxALIGN_RIGHT);
	gridszr2Panel1->Add(txtSOptions, 1, wxEXPAND);

	//rightSizerPanel1->Add(new wxStaticText(panel1, wxID_ANY, "Options:"), 0, wxALIGN_CENTER_HORIZONTAL);
	rightSizerPanel1->Add(chkUnsignedChars, 0, wxEXPAND|wxBOTTOM, 5);
	rightSizerPanel1->Add(chkUnsignedBitfields, 0, wxEXPAND|wxBOTTOM, 5);
	rightSizerPanel1->Add(chkPackStructureMembers, 0, wxEXPAND|wxBOTTOM, 5);
	rightSizerPanel1->Add(chkShortEnums, 0, wxEXPAND|wxBOTTOM, 5);
	rightSizerPanel1->Add(chkFunctionSections, 0, wxEXPAND|wxBOTTOM, 5);
	rightSizerPanel1->Add(chkDataSections, 0, wxEXPAND);

	topSizerPanel1->Add(gridszr1Panel1, 0, wxEXPAND|wxALL, 5);
	topSizerPanel1->Add(rightSizerPanel1, 0, wxEXPAND|wxALL, 5);

	sizerPanel1->Add(topSizerPanel1, 0, wxEXPAND|wxALL, 5);
	sizerPanel1->Add(gridszr2Panel1, 0, wxEXPAND|wxALL, 5);
	sizerPanel1->Add(szrBurnerPanel1, 0, wxEXPAND|wxALL, 5);

	panel1->SetSizerAndFit(sizerPanel1);


	// panel 2 (a, b)
	wxNotebook *panel2book = new wxNotebook(book, wxID_ANY);
	book->AddPage(panel2book, "Include Dir");

	// panel 2a
	wxPanel *panel2a = new wxPanel(panel2book);
	panel2book->AddPage(panel2a, "Include Path");
	listIncludePaths = new wxListBox(panel2a, ID_INCLUDE_PATH);
	btnIncDirAdd = new wxButton(panel2a, ID_INCLUDE_PATH_ADD, "Add");
	//btnIncPathMoveUp = new wxButton(panel2a, wxID_ANY, "Move Up");
	//btnIncPathMoveDown = new wxButton(panel2a, wxID_ANY, "Move Down");

	// panel 2a layout
	wxBoxSizer *sizerPanel2a = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *btnSizerPanel2a = new wxBoxSizer(wxHORIZONTAL);

	btnSizerPanel2a->Add(btnIncDirAdd, 0, wxEXPAND|wxRIGHT, 5);
	//btnSizerPanel2a->Add(btnIncPathMoveUp, 0, wxEXPAND|wxRIGHT, 5);
	//btnSizerPanel2a->Add(btnIncPathMoveDown, 0, wxEXPAND|wxRIGHT, 5);

	sizerPanel2a->Add(listIncludePaths, 1, wxEXPAND|wxALL, 5);
	sizerPanel2a->Add(btnSizerPanel2a, 0, wxEXPAND|wxALL, 5);

	panel2a->SetSizerAndFit(sizerPanel2a);

	// panel 2b
	wxPanel *panel2b = new wxPanel(panel2book);
	panel2book->AddPage(panel2b, "Library Path");
	listLibraryPaths = new wxListBox(panel2b, ID_LIBRARY_PATH);
	btnLibDirAdd = new wxButton(panel2b, ID_LIBRARY_PATH_ADD, "Add");
	//btnLibPathMoveUp = new wxButton(panel2b, wxID_ANY, "Move Up");
	//btnLibPathMoveDown = new wxButton(panel2b, wxID_ANY, "Move Down");

	// panel 2b layout
	wxBoxSizer *sizerPanel2b = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *btnSizerPanel2b = new wxBoxSizer(wxHORIZONTAL);

	btnSizerPanel2b->Add(btnLibDirAdd, 0, wxEXPAND|wxRIGHT, 5);
	//btnSizerPanel2b->Add(btnLibPathMoveUp, 0, wxEXPAND|wxRIGHT, 5);
	//btnSizerPanel2b->Add(btnLibPathMoveDown, 0, wxEXPAND|wxRIGHT, 5);

	sizerPanel2b->Add(listLibraryPaths, 1, wxEXPAND|wxALL, 5);
	sizerPanel2b->Add(btnSizerPanel2b, 0, wxEXPAND|wxALL, 5);

	panel2b->SetSizerAndFit(sizerPanel2b);

	// panel 3
	wxPanel *panel3 = new wxPanel(book);
	book->AddPage(panel3, "Libraries");
	listAvailLibs = new wxListBox(panel3, wxID_ANY);
	listLinkObj = new wxListBox(panel3, wxID_ANY);
	txtLinkerOptions = new wxTextCtrl(panel3, wxID_ANY, "");
	btnAddLib = new wxButton(panel3, wxID_ANY, "Add ->");
	btnAddLibFile = new wxButton(panel3, wxID_ANY, "Add File ->");
	btnLibRemove = new wxButton(panel3, wxID_ANY, "Remove");

	// panel 3 layout
	wxBoxSizer *sizerPanel3 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *topSizerPanel3 = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBoxSizer *leftSizerPanel3 = new wxStaticBoxSizer(wxVERTICAL, panel3, "Available Link Objects");
	wxStaticBoxSizer *rightSizerPanel3 = new wxStaticBoxSizer(wxVERTICAL, panel3, "Included");
	wxStaticBoxSizer *bottonSizerPanel3 = new wxStaticBoxSizer(wxVERTICAL, panel3, "Linker Options:");
	wxBoxSizer *btnSizerPanel3 = new wxBoxSizer(wxVERTICAL);

	leftSizerPanel3->Add(listAvailLibs, 1, wxEXPAND|wxALL, 5);
	rightSizerPanel3->Add(listLinkObj, 1, wxEXPAND|wxALL, 5);
	bottonSizerPanel3->Add(txtLinkerOptions, 0, wxEXPAND|wxALL, 5);

	btnSizerPanel3->Add(btnAddLib, 0, wxEXPAND|wxBOTTOM, 5);
	btnSizerPanel3->Add(btnAddLibFile, 0, wxEXPAND|wxBOTTOM, 5);
	btnSizerPanel3->Add(btnLibRemove, 0, wxEXPAND|wxBOTTOM, 5);

	topSizerPanel3->Add(leftSizerPanel3, 1, wxEXPAND|wxALL, 5);
	topSizerPanel3->Add(btnSizerPanel3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	topSizerPanel3->Add(rightSizerPanel3, 2, wxEXPAND|wxALL, 5);

	sizerPanel3->Add(topSizerPanel3, 1, wxEXPAND|wxALL, 5);
	sizerPanel3->Add(bottonSizerPanel3, 0, wxEXPAND|wxALL, 5);
	panel3->SetSizerAndFit(sizerPanel3);


	// main layout
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(book, 1, wxEXPAND|wxALL, 5);
	sizer->AddSpacer(5);
	sizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxALIGN_RIGHT|wxBOTTOM|wxRIGHT, 10);
	SetSizerAndFit(sizer);
}

void ConfigDialog::ApplyChanges()
{
	FormToProj();
	originalProject = project->CopyProperties(originalProject);
}

void ConfigDialog::FormToProj()
{
	burnerPanel->FormToProj();

	project->m_outputDir = txtOutputPath->GetValue();

	wxString newDev = dropDevices->GetStringSelection();
	project->m_device = newDev;
	project->m_clkFreq = numClockFreq->GetValue();
	project->m_linkerOpt = txtLinkerOptions->GetValue();
	project->m_otherOpt = txtOtherOptions->GetValue();
	project->m_otherOptForC = txtCOptions->GetValue();
	project->m_otherOptForCPP = txtCPPOptions->GetValue();
	project->m_otherOptForS = txtSOptions->GetValue();
	project->m_optimization = listOptimization->GetStringSelection();

	project->m_packStructs = chkPackStructureMembers->GetValue();
	project->m_shortEnums = chkShortEnums->GetValue();
	project->m_unsignedBitfields = chkUnsignedBitfields->GetValue();
	project->m_unsignedChars = chkUnsignedChars->GetValue();
	project->m_functSects = chkFunctionSections->GetValue();
	project->m_dataSects = chkDataSections->GetValue();

	//project.UseInitStack = chkUseInitStack.Checked;
	//project.InitStackAddr = Convert.ToUInt32("0x" + txtInitStackAddr.Text, 16);

	project->IncludeDirList().Clear();
	for (size_t i = 0; i < listIncludePaths->GetCount(); ++i)
	{
		wxString path = listIncludePaths->GetString(i);
		if (!path.IsEmpty())
			project->IncludeDirList().Add(path);
	}

	project->LibraryDirList().Clear();
	for (size_t i = 0; i < listLibraryPaths->GetCount(); ++i)
	{
		wxString path = listLibraryPaths->GetString(i);
		if (!path.IsEmpty())
			project->LibraryDirList().Add(path);
	}

	project->LinkObjList().Clear();
	project->LinkLibList().Clear();
	for (size_t i = 0; i < listLinkObj->GetCount(); ++i)
	{
		wxString s = listLinkObj->GetString(i);
		if (!s.IsEmpty())
		{
			if(s.EndsWith(".o"))
			{
				project->LinkObjList().Add(s);
			}
			else if (s.EndsWith(".a"))
			{
				project->LinkLibList().Add(s);
			}
		}
	}

/*
	project.MemorySegList.Clear();
	foreach (DataGridViewRow i in dgvMemory.Rows)
	{
		if (string.IsNullOrEmpty((string)i.Cells[1].Value) == false)
		{
			project.MemorySegList.Add((string)i.Cells[1].Value, new MemorySegment((string)i.Cells[0].Value, (string)i.Cells[1].Value, Convert.ToUInt32("0x" + (string)i.Cells[2].Value, 16)));
		}
	}
*/
}

void ConfigDialog::PopulateForm()
{
	burnerPanel->ProjToForm();

	txtOutputPath->SetValue(project->m_outputDir);

	if (dropDevices->GetCount() > 0)
	{
		dropDevices->SetSelection(0);
		if (dropDevices->FindString(project->m_device) != wxNOT_FOUND)
			dropDevices->SetSelection(dropDevices->FindString(project->m_device));
		else
			dropDevices->SetSelection( dropDevices->Append(project->m_device));
	}
	else
		dropDevices->SetSelection( dropDevices->Append(project->m_device));

	txtOtherOptions->SetValue( project->m_otherOpt );
	txtCOptions->SetValue( project->m_otherOptForC );
	txtCPPOptions->SetValue( project->m_otherOptForCPP );
	txtSOptions->SetValue( project->m_otherOptForS );
	txtLinkerOptions->SetValue( project->m_linkerOpt );
	//txtInitStackAddr->SetValue( wxString::Format("0x%X", project->m_initStackAddr) );
	numClockFreq->SetRange(0, 32000000);
	numClockFreq->SetValue( project->m_clkFreq );
	//chkUseInitStack->Checked( project->m_useInitStack );
	listOptimization->Clear();
	listOptimization->Append("-O0");
	listOptimization->Append("-O1");
	listOptimization->Append("-O2");
	listOptimization->Append("-Os");
	listOptimization->SetSelection( listOptimization->FindString(project->m_optimization) );

	chkPackStructureMembers->SetValue(project->m_packStructs);
	chkShortEnums->SetValue(project->m_shortEnums);
	chkUnsignedBitfields->SetValue(project->m_unsignedBitfields);
	chkUnsignedChars->SetValue(project->m_unsignedChars);
	chkFunctionSections->SetValue(project->m_functSects);
	chkDataSections->SetValue(project->m_dataSects);

	listLinkObj->Clear();
	listLinkObj->Append(project->LinkLibList());
	listLinkObj->Append(project->LinkObjList());

	listAvailLibs->Clear();
	listAvailLibs->Append("libc.a");
	listAvailLibs->Append("libm.a");
	listAvailLibs->Append("libobjc.a");
	listAvailLibs->Append("libprintf_flt.a");
	listAvailLibs->Append("libprintf_min.a");
	listAvailLibs->Append("libscanf_flt.a");
	listAvailLibs->Append("libscanf_min.a");

	listIncludePaths->Clear();
	for (size_t i = 0; i < project->IncludeDirList().GetCount(); ++i)
		listIncludePaths->Append(project->IncludeDirList()[i]);

	listLibraryPaths->Clear();
	for (size_t i = 0; i < project->LibraryDirList().GetCount(); ++i)
		listLibraryPaths->Append(project->LibraryDirList()[i]);

/*
	dgvMemory.Rows.Clear();
	foreach (MemorySegment m in project.MemorySegList.Values)
	{
		DataGridViewRow dgvr = new DataGridViewRow();
		string[] memStr = new string[3];
		string s = "Flash";
		if (m.Type.ToLowerInvariant().Contains("flash"))
		s = "Flash";
		else if (m.Type.ToLowerInvariant().Contains("eeprom"))
		s = "EEPROM";
		else if (m.Type.ToLowerInvariant().Contains("sram"))
		s = "SRAM";
		memStr[0] = s;
		memStr[1] = m.Name;
		memStr[2] = Convert.ToString(m.Addr, 16).ToUpper();
		dgvr.CreateCells(dgvMemory, memStr);
		int i = dgvMemory.Rows.Add(dgvr);
	}
*/
}
