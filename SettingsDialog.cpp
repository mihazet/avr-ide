#include "SettingsDialog.h"
#include "DefsExt.h"
#include "StyledTextCtrl.h"

StyleInfo *g_currentStyleInfo = &g_LanguagePrefs[0].styles[0];
LanguageInfo *g_currentLang = &g_LanguagePrefs[0];

#include <wx/confbase.h>

BEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
	EVT_BUTTON(wxID_OK,				SettingsDialog::OnOk)
	EVT_LISTBOX(ID_LIST_STYLES,		SettingsDialog::OnStyleSelect)
	EVT_CHOICE(ID_CHOICE_LANGUAGES,	SettingsDialog::OnLanguageSelect)
	EVT_CHECKBOX(ID_CHECK_FOLD, 	SettingsDialog::OnBoldCheck)
	EVT_CHECKBOX(ID_CHECK_ITALIC, 	SettingsDialog::OnItalicCheck)
	EVT_COLOURPICKER_CHANGED(ID_PICKER_FORE_COLOUR,	SettingsDialog::OnForeColourPicker)
	EVT_COLOURPICKER_CHANGED(ID_PICKER_BACK_COLOUR,	SettingsDialog::OnBackColourPicker)
	EVT_SPINCTRL(ID_SPIN_FONTSIZE,	SettingsDialog::OnFontSizeSpin)
END_EVENT_TABLE()


SettingsDialog::SettingsDialog(wxWindow *parent)
	: wxDialog(parent, wxID_ANY, wxString("Settings"))
{
	CreateControls();
	// common settings
	m_autocompleteCheckBox->SetValue(wxConfigBase::Get()->Read("/editor/autocomplete", 1));
	m_wordWrappingCheckBox->SetValue(wxConfigBase::Get()->Read("/editor/wrap_mode", 0l));
	m_indentationGuideLinesCheckBox->SetValue(wxConfigBase::Get()->Read("/editor/indentation_guides", 0l));
	m_lineNumbersCheckBox->SetValue(wxConfigBase::Get()->Read("/editor/line_number", 1l));
	m_useTabsCheckBox->SetValue(wxConfigBase::Get()->Read("/editor/use_tab", 1l));
	m_tabIndentCheckBox->SetValue(wxConfigBase::Get()->Read("/editor/tab_indents", 1l));
	m_backspaceUnindentsCheckBox->SetValue(wxConfigBase::Get()->Read("/editor/back_space_un_indents", 0l));
	m_whitespaceCheckBox->SetValue(wxConfigBase::Get()->Read("/editor/whitespace", 0l));
	m_highlightCurrentLineCheckBox->SetValue(wxConfigBase::Get()->Read("/editor/highlight_carret_line", 1l));

	m_tabWidthSpin->SetValue(wxConfigBase::Get()->Read("/editor/tab_width", 4));
	m_indentWidthSpin->SetValue(wxConfigBase::Get()->Read("/editor/indent", 4));

	LoadColours();

	for (int i = 0; i < g_LanguagePrefsSize; ++i)
	{
		m_languageChoice->Append(g_LanguagePrefs[i].name);
	}
	m_languageChoice->SetSelection(0);
	UpdateStyles();
	//m_styleListBox->SetSelection(0);
	//UpdateInfo();
}

void SettingsDialog::OnOk(wxCommandEvent& event)
{

	wxConfigBase::Get()->Write("/editor/autocomplete", m_autocompleteCheckBox->GetValue());
	wxConfigBase::Get()->Write("/editor/wrap_mode", m_wordWrappingCheckBox->GetValue());
	wxConfigBase::Get()->Write("/editor/indentation_guides", m_indentationGuideLinesCheckBox->GetValue());
	wxConfigBase::Get()->Write("/editor/line_number", m_lineNumbersCheckBox->GetValue());
	wxConfigBase::Get()->Write("/editor/use_tab", m_useTabsCheckBox->GetValue());
	wxConfigBase::Get()->Write("/editor/tab_indents", m_tabIndentCheckBox->GetValue());
	wxConfigBase::Get()->Write("/editor/back_space_un_indents", m_backspaceUnindentsCheckBox->GetValue());
	wxConfigBase::Get()->Write("/editor/whitespace", m_whitespaceCheckBox->GetValue());
	wxConfigBase::Get()->Write("/editor/highlight_carret_line", m_highlightCurrentLineCheckBox->GetValue());

	wxConfigBase::Get()->Write("/editor/tab_width", m_tabWidthSpin->GetValue());
	wxConfigBase::Get()->Write("/editor/indent", m_indentWidthSpin->GetValue());


	SaveColours();
	event.Skip();
}

void SettingsDialog::OnStyleSelect(wxCommandEvent& event)
{
	int sel = m_styleListBox->GetSelection();
	//wxString styleName = m_styleListBox->GetStringSelection();
	g_currentStyleInfo = &g_currentLang->styles[sel];
	UpdateInfo();
}

void SettingsDialog::OnLanguageSelect(wxCommandEvent& event)
{
	//int sel = m_languageChoice->GetSelection();
	wxString langName = m_languageChoice->GetStringSelection();
	//wxMessageBox(langName);
	for (int i = 0; i < g_LanguagePrefsSize; ++i)
	{
		if (g_LanguagePrefs[i].name == langName)
		{
			g_currentLang = &g_LanguagePrefs[i];
			UpdateStyles();
			g_currentStyleInfo = 0;
			UpdateInfo();
			break;
		}
	}
}

void SettingsDialog::OnForeColourPicker(wxColourPickerEvent& event)
{
	if (g_currentStyleInfo)
	{
		g_currentStyleInfo->foreground = m_foreColourPicker->GetColour().GetAsString();
	}
}

void SettingsDialog::OnBackColourPicker(wxColourPickerEvent& event)
{
	if (g_currentStyleInfo)
	{
		g_currentStyleInfo->background = m_backColourPicker->GetColour().GetAsString();
	}
}

void SettingsDialog::OnBoldCheck(wxCommandEvent& event)
{
	if (g_currentStyleInfo)
	{
		if (m_boldCheckBox->IsChecked())
			g_currentStyleInfo->fontstyle |= mySTC_STYLE_BOLD;
		else
			g_currentStyleInfo->fontstyle &= ~mySTC_STYLE_BOLD;
	}
}

void SettingsDialog::OnItalicCheck(wxCommandEvent& event)
{
	if (g_currentStyleInfo)
	{
		if (m_italicCheckBox->IsChecked())
			g_currentStyleInfo->fontstyle |= mySTC_STYLE_ITALIC;
		else
			g_currentStyleInfo->fontstyle &= ~mySTC_STYLE_ITALIC;
	}
}

void SettingsDialog::OnFontSizeSpin(wxSpinEvent& event)
{
	if (g_currentStyleInfo)
	{
		g_currentStyleInfo->fontsize = m_fontSizeSpin->GetValue();
	}
}


void SettingsDialog::UpdateInfo()
{
	if (g_currentStyleInfo)
	{
		m_foreColourPicker->SetColour( wxColour(g_currentStyleInfo->foreground) );
		m_backColourPicker->SetColour( wxColour(g_currentStyleInfo->background) );
		m_boldCheckBox->SetValue( g_currentStyleInfo->fontstyle & mySTC_STYLE_BOLD );
		m_italicCheckBox->SetValue( g_currentStyleInfo->fontstyle & mySTC_STYLE_ITALIC );
		m_fontSizeSpin->SetValue( g_currentStyleInfo->fontsize );
	}
	else
	{
		m_foreColourPicker->SetColour( *wxBLACK );
		m_backColourPicker->SetColour( *wxWHITE );
		m_boldCheckBox->SetValue( false );
		m_italicCheckBox->SetValue( false );
		m_fontSizeSpin->SetValue( 0 );
	}
}

void SettingsDialog::UpdateStyles()
{
	m_styleListBox->Clear();
	for (int i = 0; i < STYLE_TYPES_COUNT; ++i)
	{
		if (g_currentLang->styles[i].type == -1) continue;
		m_styleListBox->Append(g_currentLang->styles[i].name);
	}
}

void SettingsDialog::CreateControls()
{
	m_languageChoice = new wxChoice(this, ID_CHOICE_LANGUAGES);
	m_styleListBox = new wxListBox(this, ID_LIST_STYLES);
	m_foreColourPicker = new wxColourPickerCtrl(this, ID_PICKER_FORE_COLOUR);
	m_backColourPicker = new wxColourPickerCtrl(this, ID_PICKER_BACK_COLOUR);

	m_boldCheckBox = new wxCheckBox(this, ID_CHECK_FOLD, "Bold");
	m_italicCheckBox = new wxCheckBox(this, ID_CHECK_ITALIC, "Italic");

	m_fontSizeSpin = new wxSpinCtrl(this, wxID_ANY);

	m_autocompleteCheckBox = new wxCheckBox(this, wxID_ANY, "Enable Autocomplete");
	m_wordWrappingCheckBox = new wxCheckBox(this, wxID_ANY, "Enable Word Wrapping");
	m_indentationGuideLinesCheckBox = new wxCheckBox(this, wxID_ANY, "Show Indentation Guide Lines");
	m_lineNumbersCheckBox = new wxCheckBox(this, wxID_ANY, "Show Line Numbers");
	m_useTabsCheckBox = new wxCheckBox(this, wxID_ANY, "Use Tabs");
	m_tabIndentCheckBox = new wxCheckBox(this, wxID_ANY, "Tab Indents");
	m_backspaceUnindentsCheckBox = new wxCheckBox(this, wxID_ANY, "Backspace Unindents");
	m_whitespaceCheckBox = new wxCheckBox(this, wxID_ANY, "Show Whitespace");
	m_highlightCurrentLineCheckBox = new wxCheckBox(this, wxID_ANY, "Highlight Current Line");

	m_tabWidthSpin = new wxSpinCtrl(this, wxID_ANY);
	m_indentWidthSpin = new wxSpinCtrl(this, wxID_ANY);

	wxFlexGridSizer *spinSizer = new wxFlexGridSizer(2, 5, 5);
	spinSizer->AddGrowableCol(1);
	spinSizer->Add(new wxStaticText(this, wxID_ANY, "Tab Width:"), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);
	spinSizer->Add(m_tabWidthSpin, 1, wxEXPAND);
	spinSizer->Add(new wxStaticText(this, wxID_ANY, "Indent Width:"), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);
	spinSizer->Add(m_indentWidthSpin, 1, wxEXPAND);

	wxStaticBoxSizer *leftSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Common");
	leftSizer->Add(m_autocompleteCheckBox, 0, wxALL, 5);
	leftSizer->Add(m_wordWrappingCheckBox, 0, wxALL, 5);
	leftSizer->Add(m_indentationGuideLinesCheckBox, 0, wxALL, 5);
	leftSizer->Add(m_lineNumbersCheckBox, 0, wxALL, 5);
	leftSizer->Add(m_useTabsCheckBox, 0, wxALL, 5);
	leftSizer->Add(m_tabIndentCheckBox, 0, wxALL, 5);
	leftSizer->Add(m_backspaceUnindentsCheckBox, 0, wxALL, 5);
	leftSizer->Add(m_whitespaceCheckBox, 0, wxALL, 5);
	leftSizer->Add(m_highlightCurrentLineCheckBox, 0, wxALL, 5);
	leftSizer->Add(spinSizer, 0, wxEXPAND|wxALL, 5);

	wxFlexGridSizer *pickerSizer = new wxFlexGridSizer(2, 5, 5);
	pickerSizer->AddGrowableCol(1);
	pickerSizer->Add(new wxStaticText(this, wxID_ANY, "Fore Colour:"), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);
	pickerSizer->Add(m_foreColourPicker, 1, wxEXPAND);
	pickerSizer->Add(new wxStaticText(this, wxID_ANY, "Back Colour:"), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);
	pickerSizer->Add(m_backColourPicker, 1, wxEXPAND);

	wxStaticBoxSizer *rightSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Colour and Font");
	rightSizer->Add(m_languageChoice, 0,  wxEXPAND|wxALL, 5);
	rightSizer->Add(m_styleListBox, 1,  wxEXPAND|wxALL, 5);
	rightSizer->Add(pickerSizer, 0, wxEXPAND|wxALL, 5);
	rightSizer->Add(m_boldCheckBox, 0, wxALL, 5);
	rightSizer->Add(m_italicCheckBox, 0, wxALL, 5);
	rightSizer->Add(m_fontSizeSpin, 0, wxALL, 5);

	wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);
	topSizer->Add(leftSizer, 1, wxEXPAND|wxRIGHT, 5);
	topSizer->Add(rightSizer, 1, wxEXPAND);

	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(topSizer, 1, wxEXPAND|wxALL, 5);
	mainSizer->AddSpacer(5);
	mainSizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxALIGN_RIGHT|wxBOTTOM|wxRIGHT, 10);
	SetSizerAndFit(mainSizer);
	Center();
}
