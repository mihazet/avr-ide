#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/clrpicker.h>


class SettingsDialog : public wxDialog
{
public:
	SettingsDialog(wxWindow *parent);

private:
	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& event);
	void OnStyleSelect(wxCommandEvent& event);
	void OnLanguageSelect(wxCommandEvent& event);
	void OnForeColourPicker(wxColourPickerEvent& event);
	void OnBackColourPicker(wxColourPickerEvent& event);
	void OnBoldCheck(wxCommandEvent& event);
	void OnItalicCheck(wxCommandEvent& event);
	void OnFontSizeSpin(wxSpinEvent& event);

	void UpdateInfo();
	void UpdateStyles();

	void CreateControls();

	wxChoice			*m_languageChoice;
	wxListBox			*m_styleListBox;
	wxColourPickerCtrl	*m_foreColourPicker;
	wxColourPickerCtrl	*m_backColourPicker;
	wxSpinCtrl			*m_fontSizeSpin;

	wxCheckBox	*m_boldCheckBox;
	wxCheckBox	*m_italicCheckBox;

	wxCheckBox	*m_autocompleteCheckBox;
	wxCheckBox	*m_wordWrappingCheckBox;
	wxCheckBox	*m_indentationGuideLinesCheckBox;
	wxCheckBox	*m_lineNumbersCheckBox;
	wxCheckBox	*m_useTabsCheckBox;
	wxCheckBox	*m_tabIndentCheckBox;
	wxCheckBox	*m_backspaceUnindentsCheckBox;
	wxCheckBox	*m_whitespaceCheckBox;
	wxCheckBox	*m_highlightCurrentLineCheckBox;

	wxSpinCtrl	*m_tabWidthSpin;
	wxSpinCtrl	*m_indentWidthSpin;

};
