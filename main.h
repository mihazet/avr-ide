#include <wx/wx.h>

class IDEApp : public wxApp
{
public:
	static wxString RelativePath(wxString dirPath, wxString filePath);

private:
	virtual bool OnInit();
};
