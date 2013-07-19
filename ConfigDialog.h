#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>

class AVRProject;
class BurnerPanel;

class ConfigDialog : public wxDialog
{
public:
	ConfigDialog(AVRProject *prj, wxWindow *parent, const wxString& title);

private:
	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& event);
	void OnIncludePathsDClick(wxCommandEvent& event);
	void OnLibraryPathsDClick(wxCommandEvent& event);
	void OnIncludeDirAdd(wxCommandEvent& event);
	void OnLibraryDirAdd(wxCommandEvent& event);


	void CreateControls();
	void ApplyChanges();

	void FormToProj();
	void PopulateForm();

	// panel 1
	wxTextCtrl	*txtOutputPath;
	wxChoice	*dropDevices;
	wxSpinCtrl	*numClockFreq;
	wxChoice	*listOptimization;
	wxTextCtrl	*txtOtherOptions;
	wxTextCtrl	*txtCOptions;
	wxTextCtrl	*txtCPPOptions;
	wxTextCtrl	*txtSOptions;

	wxCheckBox	*chkUnsignedChars;
	wxCheckBox	*chkUnsignedBitfields;
	wxCheckBox	*chkPackStructureMembers;
	wxCheckBox	*chkShortEnums;
	wxCheckBox	*chkFunctionSections;
	wxCheckBox	*chkDataSections;

	BurnerPanel	*burnerPanel;

	// panel 2 (a, b)
	wxListBox	*listIncludePaths;
	wxListBox	*listLibraryPaths;
	wxButton	*btnIncDirAdd;
	//wxButton	*btnIncPathMoveUp;
	//wxButton	*btnIncPathMoveDown;
	wxButton	*btnLibDirAdd;
	//wxButton	*btnLibPathMoveUp;
	//wxButton	*btnLibPathMoveDown;

	// panel 3
	wxListBox	*listAvailLibs;
	wxListBox	*listLinkObj;
	wxTextCtrl	*txtLinkerOptions;
	wxButton	*btnAddLib;
	wxButton	*btnAddLibFile;
	wxButton	*btnLibRemove;



	AVRProject *project;
	AVRProject *originalProject;
	wxArrayString orderedDevices;
};

