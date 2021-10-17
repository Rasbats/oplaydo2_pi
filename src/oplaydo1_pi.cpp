/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  oplaydo1 Plugin
 * Author:   Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2017 by Mike Rossiter                                *
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

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "oplaydo1_pi.h"
#include "oplaydo1gui_impl.h"
#include "oplaydo1gui.h"
#include "ocpn_plugin.h"

#include "wx/filename.h"
#include "version.h"
#include "wxWTranslateCatalog.h"


// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new oplaydo1_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    oplaydo1 PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

oplaydo1_pi::oplaydo1_pi(void *ppimgr)
      :opencpn_plugin_116 (ppimgr)
{
      // Create the PlugIn icons
      initialize_images();
      
      wxFileName fn;

    auto path = GetPluginDataDir("oplaydo1_pi");
    fn.SetPath(path);
    fn.AppendDir("data");
    fn.SetFullName("oplaydo1_pi_panel_icon.png");

    path = fn.GetFullPath();

    wxInitAllImageHandlers();

    wxLogDebug(wxString("Using icon path: ") + path);
    if (!wxImage::CanRead(path)) {
        wxLogDebug("Initiating image handlers.");
        wxInitAllImageHandlers();
    }
    wxImage panelIcon(path);
    if (panelIcon.IsOk())
        m_panelBitmap = wxBitmap(panelIcon);
    else
        wxLogWarning("oplaydo1 panel icon has NOT been loaded");
      
	  m_bShowoplaydo1 = false;
}

oplaydo1_pi::~oplaydo1_pi(void)
{
     delete _img_oplaydo1_pi;
     delete _img_oplaydo1;
     
}

int oplaydo1_pi::Init(void)
{
      AddLocaleCatalog( PLUGIN_CATALOG_NAME );

      // Set some default private member parameters
      m_route_dialog_x = 0;
      m_route_dialog_y = 0;
      ::wxDisplaySize(&m_display_width, &m_display_height);

      //    Get a pointer to the opencpn display canvas, to use as a parent for the POI Manager dialog
      m_parent_window = GetOCPNCanvasWindow();

      //    Get a pointer to the opencpn configuration object
      m_pconfig = GetOCPNConfigObject();

      //    And load the configuration items
      LoadConfig();

      //    This PlugIn needs a toolbar icon, so request its insertion
	if(m_boplaydo1ShowIcon){
#ifdef PLUGIN_USE_SVG
        m_leftclick_tool_id = InsertPlugInToolSVG(_T( "oplaydo1" ),
            _svg_oplaydo1, _svg_oplaydo1, _svg_oplaydo1_toggled,
            wxITEM_CHECK, _("oplaydo1"), _T( "" ), NULL,
            CALCULATOR_TOOL_POSITION, 0, this);
#else
      m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_oplaydo1, _img_oplaydo1, wxITEM_CHECK,
            _("oplaydo1"), _T(""), NULL,
             CALCULATOR_TOOL_POSITION, 0, this);
#endif
    }    

      m_pDialog = NULL;

      return (WANTS_OVERLAY_CALLBACK |
              WANTS_OPENGL_OVERLAY_CALLBACK |
		  
              WANTS_TOOLBAR_CALLBACK    |
              INSTALLS_TOOLBAR_TOOL     |
              WANTS_CONFIG             |
			  WANTS_PLUGIN_MESSAGING

           );
}

bool oplaydo1_pi::DeInit(void)
{
      //    Record the dialog position
      if (NULL != m_pDialog)
      {
            //Capture dialog position
            wxPoint p = m_pDialog->GetPosition();
            SetCalculatorDialogX(p.x);
            SetCalculatorDialogY(p.y);
            m_pDialog->Close();
            delete m_pDialog;
            m_pDialog = NULL;

			m_bShowoplaydo1 = false;
			SetToolbarItemState( m_leftclick_tool_id, m_bShowoplaydo1 );

      }	
    
    SaveConfig();

    RequestRefresh(m_parent_window); // refresh mainn window

    return true;
}

int oplaydo1_pi::GetAPIVersionMajor()
{
      return OCPN_API_VERSION_MAJOR;
}

int oplaydo1_pi::GetAPIVersionMinor()
{
      return OCPN_API_VERSION_MINOR;
}

int oplaydo1_pi::GetPlugInVersionMajor()
{
      return PLUGIN_VERSION_MAJOR;
}

int oplaydo1_pi::GetPlugInVersionMinor()
{
      return PLUGIN_VERSION_MINOR;
}

wxBitmap *oplaydo1_pi::GetPlugInBitmap()
{
      return &m_panelBitmap;
}

wxString oplaydo1_pi::GetCommonName()
{
      return _("oplaydo1");
}


wxString oplaydo1_pi::GetShortDescription()
{
      return _(PLUGIN_SHORT_DESCRIPTION);
}

wxString oplaydo1_pi::GetLongDescription()
{
      return _(PLUGIN_LONG_DESCRIPTION);
}

int oplaydo1_pi::GetToolbarToolCount(void)
{
      return 1;
}

void oplaydo1_pi::SetColorScheme(PI_ColorScheme cs)
{
      if (NULL == m_pDialog)
            return;

      DimeWindow(m_pDialog);
}

void oplaydo1_pi::OnToolbarToolCallback(int id)
{
    
	if(NULL == m_pDialog)
      {
            m_pDialog = new Dlg(m_parent_window);
            m_pDialog->plugin = this;
            m_pDialog->Move(wxPoint(m_route_dialog_x, m_route_dialog_y));
      }

	  m_pDialog->Fit();
	  //Toggle 
	  m_bShowoplaydo1 = !m_bShowoplaydo1;	  

      //    Toggle dialog? 
      if(m_bShowoplaydo1) {
          m_pDialog->Show();         
      } else
          m_pDialog->Hide();
     
      // Toggle is handled by the toolbar but we must keep plugin manager b_toggle updated
      // to actual status to ensure correct status upon toolbar rebuild
      SetToolbarItemState( m_leftclick_tool_id, m_bShowoplaydo1 );

      RequestRefresh(m_parent_window); // refresh main window
}

bool oplaydo1_pi::LoadConfig(void)
{
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
            pConf->SetPath ( _T( "/Settings/oplaydo1_pi" ) );
			 pConf->Read ( _T( "Showoplaydo1Icon" ), &m_boplaydo1ShowIcon, 1 );
           
            m_route_dialog_x =  pConf->Read ( _T ( "DialogPosX" ), 20L );
            m_route_dialog_y =  pConf->Read ( _T ( "DialogPosY" ), 20L );
         
            if((m_route_dialog_x < 0) || (m_route_dialog_x > m_display_width))
                  m_route_dialog_x = 5;
            if((m_route_dialog_y < 0) || (m_route_dialog_y > m_display_height))
                  m_route_dialog_y = 5;
            return true;
      }
      else
            return false;
}

bool oplaydo1_pi::SaveConfig(void)
{
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
            pConf->SetPath ( _T ( "/Settings/oplaydo1_pi" ) );
			pConf->Write ( _T ( "Showoplaydo1Icon" ), m_boplaydo1ShowIcon );
          
            pConf->Write ( _T ( "DialogPosX" ),   m_route_dialog_x );
            pConf->Write ( _T ( "DialogPosY" ),   m_route_dialog_y );
            
            return true;
      }
      else
            return false;
}

void oplaydo1_pi::Onoplaydo1DialogClose()
{
    m_bShowoplaydo1 = false;
    SetToolbarItemState( m_leftclick_tool_id, m_bShowoplaydo1 );
    m_pDialog->Hide();
    SaveConfig();

    RequestRefresh(m_parent_window); // refresh main window

}


