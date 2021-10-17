///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "oplaydo1gui.h"

///////////////////////////////////////////////////////////////////////////

m_dialog::m_dialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 3, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticTextLocation = new wxStaticText( this, wxID_ANY, _("Location:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextLocation->Wrap( -1 );
	m_staticTextLocation->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	fgSizer1->Add( m_staticTextLocation, 0, wxALL, 5 );

	m_staticTextLat = new wxStaticText( this, wxID_ANY, _("Latitude (N/S, Use +/-)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextLat->Wrap( -1 );
	fgSizer1->Add( m_staticTextLat, 0, wxALL, 5 );

	m_staticTextLon = new wxStaticText( this, wxID_ANY, _("Longitude (E/W, Use +/-)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextLon->Wrap( -1 );
	fgSizer1->Add( m_staticTextLon, 0, wxALL|wxEXPAND, 5 );

	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer1->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );

	m_staticline5 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer1->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );

	m_staticline6 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer1->Add( m_staticline6, 0, wxEXPAND | wxALL, 5 );

	m_staticTextStart = new wxStaticText( this, wxID_ANY, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextStart->Wrap( -1 );
	fgSizer1->Add( m_staticTextStart, 0, wxALL, 5 );

	m_textCtrlStartLat = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlStartLat, 0, wxALL, 5 );

	m_textCtrlStartLon = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlStartLon, 0, wxALL, 5 );

	m_staticTextFinish = new wxStaticText( this, wxID_ANY, _("Finish"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFinish->Wrap( -1 );
	fgSizer1->Add( m_staticTextFinish, 0, wxALL, 5 );

	m_textCtrlFinishLat = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlFinishLat, 0, wxALL, 5 );

	m_textCtrlFinishLon = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlFinishLon, 0, wxALL, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer1->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer1->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );

	m_staticline7 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer1->Add( m_staticline7, 0, wxEXPAND | wxALL, 5 );

	m_buttonGenerate = new wxButton( this, wxID_ANY, _("Generate the GPX file"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonGenerate, 0, wxALL, 5 );

	m_staticline8 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer1->Add( m_staticline8, 0, wxEXPAND | wxALL, 5 );


	bSizer4->Add( fgSizer1, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer4 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_buttonGenerate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_dialog::OnGenerate ), NULL, this );
}

m_dialog::~m_dialog()
{
	// Disconnect Events
	m_buttonGenerate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_dialog::OnGenerate ), NULL, this );

}
