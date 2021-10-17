///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class m_dialog
///////////////////////////////////////////////////////////////////////////////
class m_dialog : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticTextLocation;
		wxStaticText* m_staticTextLat;
		wxStaticText* m_staticTextLon;
		wxStaticLine* m_staticline4;
		wxStaticLine* m_staticline5;
		wxStaticLine* m_staticline6;
		wxStaticText* m_staticTextStart;
		wxTextCtrl* m_textCtrlStartLat;
		wxTextCtrl* m_textCtrlStartLon;
		wxStaticText* m_staticTextFinish;
		wxTextCtrl* m_textCtrlFinishLat;
		wxTextCtrl* m_textCtrlFinishLon;
		wxStaticLine* m_staticline1;
		wxStaticLine* m_staticline2;
		wxStaticLine* m_staticline3;
		wxStaticLine* m_staticline7;
		wxButton* m_buttonGenerate;
		wxStaticLine* m_staticline8;

		// Virtual event handlers, overide them in your derived class
		virtual void OnGenerate( wxCommandEvent& event ) { event.Skip(); }


	public:

		m_dialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 350,200 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~m_dialog();

};

