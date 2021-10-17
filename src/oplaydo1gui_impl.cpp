/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  oplaydo1 Plugin
 * Author:   Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2017 by Mike Rossiter                                   *
 *   $EMAIL$                                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 */

#include "oplaydo1gui_impl.h"
#include <wx/progdlg.h>
#include <wx/wx.h>
#include "wx/dir.h"
#include "oplaydo1_pi.h"


class oplaydo1_pi;

#define FAIL(X) do { error = X; goto failed; } while(0)


Dlg::Dlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : m_dialog( parent, id, title, pos, size, style )
{
    this->Fit();
    dbg=false; //for debug output set to true
}





void Dlg::OnGenerate( wxCommandEvent& event){

	bool error_occured = false;

	bool m_bPositions = GetPositions();

	if (!m_bPositions){
		wxMessageBox(_T("Invalid Positions, Operation Cancelled"));
		return;
	}

	wxString s;
    if (write_file){
        wxFileDialog dlg(this, _("Export oplaydo1 Positions in GPX file"), wxEmptyString, wxEmptyString, _T("GPX files (*.gpx)|*.gpx|All files (*.*)|*.*"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
        if (dlg.ShowModal() == wxID_CANCEL){
            error_occured=true;     // the user changed idea...
		    return;
		}

        s=dlg.GetPath();

        if (dlg.GetPath() == wxEmptyString){ error_occured=true; if (dbg) printf("Empty Path\n");}
    }

    //Start making GPX
    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
    doc.LinkEndChild( decl );
    TiXmlElement * root = new TiXmlElement( "gpx" );
    TiXmlElement * Route = new TiXmlElement( "rte" );
    TiXmlElement * RouteName = new TiXmlElement( "name" );
    TiXmlText * text4 = new TiXmlText("oPlayDo1");

    if (write_file){
        doc.LinkEndChild( root );
        root->SetAttribute("version", "0.1");
        root->SetAttribute("creator", "oplaydo1_pi by Rasbats");
        root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
        root->SetAttribute("xmlns:gpxx","http://www.garmin.com/xmlschemas/GpxExtensions/v3" );
        root->SetAttribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd");
        root->SetAttribute("xmlns:opencpn","http://www.opencpn.org");
        Route->LinkEndChild( RouteName );
        RouteName->LinkEndChild( text4 );


        TiXmlElement * Extensions = new TiXmlElement( "extensions" );

        TiXmlElement * StartN = new TiXmlElement( "opencpn:start" );
        TiXmlText * text5 = new TiXmlText( "Start" );
        Extensions->LinkEndChild( StartN );
        StartN->LinkEndChild( text5 );

        TiXmlElement * EndN = new TiXmlElement( "opencpn:end" );
        TiXmlText * text6 = new TiXmlText( "End" );
        Extensions->LinkEndChild( EndN );
        EndN->LinkEndChild( text6 );

        Route->LinkEndChild( Extensions );
    }

      if (dbg) cout<<"oplaydo1 Calculation\n";

	  double lati, loni;


	  for (std::list<Position>::iterator itOut = myPositions.begin(); itOut != myPositions.end(); itOut++){
		  //wxMessageBox((*it).lat, _T("*it.lat"));

		  double value, value1;
		  if (!(*itOut).lat.ToDouble(&value)){ /* error! */ }
		  lati = value;
		  if (!(*itOut).lon.ToDouble(&value1)){ /* error! */ }
		  loni = value1;

		  Addpoint(Route, wxString::Format(wxT("%f"), lati), wxString::Format(wxT("%f"), loni), (*itOut).wpt_num, _T("diamond"), _T("WPT"));

	  }

       if (write_file){
            root->LinkEndChild( Route );
            // check if string ends with .gpx or .GPX
            //if (!wxFileExists(s)){
            //     s = s + _T(".gpx");
           // }
            wxCharBuffer buffer=s.ToUTF8();
            if (dbg) std::cout<< buffer.data()<<std::endl;
            doc.SaveFile( buffer.data() );
	   }


    if (error_occured==true)  {
        wxMessageBox(_("Operation has been cancelled!") );
    }

}

void Dlg::Addpoint(TiXmlElement* Route, wxString ptlat, wxString ptlon, wxString ptname, wxString ptsym, wxString pttype){
	//add point
	TiXmlElement * RoutePoint = new TiXmlElement("rtept");
	RoutePoint->SetAttribute("lat", ptlat.mb_str());
	RoutePoint->SetAttribute("lon", ptlon.mb_str());


	TiXmlElement * Name = new TiXmlElement("name");
	TiXmlText * text = new TiXmlText(ptname.mb_str());
	RoutePoint->LinkEndChild(Name);
	Name->LinkEndChild(text);

	TiXmlElement * Symbol = new TiXmlElement("sym");
	TiXmlText * text1 = new TiXmlText(ptsym.mb_str());
	RoutePoint->LinkEndChild(Symbol);
	Symbol->LinkEndChild(text1);

	TiXmlElement * Type = new TiXmlElement("type");
	TiXmlText * text2 = new TiXmlText(pttype.mb_str());
	RoutePoint->LinkEndChild(Type);
	Type->LinkEndChild(text2);
	Route->LinkEndChild(RoutePoint);
	//done adding point
}

bool Dlg::GetPositions()
{
	bool error_occured = false;

	myPositions.clear();

	double lat1, lon1;
	double lat2, lon2;

	if (!this->m_textCtrlStartLat->GetValue().ToDouble(&lat1)){ lat1 = 0.0; }
	if (!this->m_textCtrlStartLon->GetValue().ToDouble(&lon1)){ lon1 = 0.0; }

	if (!this->m_textCtrlFinishLat->GetValue().ToDouble(&lat2)){ lat2 = 0.0; }
	if (!this->m_textCtrlFinishLon->GetValue().ToDouble(&lon2)){ lon2 = 0.0; }

	//Validate input ranges
		if (fabs(lat1)>90||fabs(lon1)>180||fabs(lat2)>90||fabs(lon2)>180){
			error_occured = true;
			wxMessageBox(_("Error in input range validation"));
			return false;
		}

	m_position.lat = m_textCtrlStartLat->GetValue();
	m_position.lon = m_textCtrlStartLon->GetValue();

	myPositions.push_back(m_position);

	m_position.lat = m_textCtrlFinishLat->GetValue();
	m_position.lon = m_textCtrlFinishLon->GetValue();

	myPositions.push_back(m_position);

	write_file = true;

	return true;
}


void Dlg::OnClose(wxCloseEvent& event)
{
	plugin->Onoplaydo1DialogClose();
}
