#ifndef	XML_UTILS_H
#define	XML_UTILS_H

class XmlUtils
{
public:
	// helper function for read/write xml
	static wxString ReadElementStringByTag(wxXmlNode *parent, const wxString& tag)
	{
		wxXmlNode *children = parent->GetChildren();
		while (children)
		{
			if (children->GetName() == tag)
				return children->GetNodeContent();
			children = children->GetNext();
		}
		return wxEmptyString;
	}

	static wxXmlNode *ReadElementByTag(wxXmlNode *parent, const wxString& tag)
	{
		wxXmlNode *children = parent->GetChildren();
		while (children)
		{
			if (children->GetName() == tag)
				return children;
			children = children->GetNext();
		}
		return 0;
	}

	static void WriteElementString(wxXmlNode *parent, const wxString& key, const wxString& value)
	{
		wxXmlNode *node = new wxXmlNode(wxXML_ELEMENT_NODE, key);
		new wxXmlNode(node, wxXML_TEXT_NODE, "", value);
		parent->AddChild(node);
	}

	static wxXmlNode *WriteElement(wxXmlNode *parent, const wxString& name)
	{
		wxXmlNode *node = new wxXmlNode(wxXML_ELEMENT_NODE, name);
		parent->AddChild(node);
		return node;
	}

};

#endif
