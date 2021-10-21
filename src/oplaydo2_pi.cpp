/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  oplaydo2 Plugin
 * Author:   Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2013 by Mike Rossiter                                *
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

#include "oplaydo2_pi.h"
#include "oplaydo2gui_impl.h"
#include "oplaydo2gui.h"

float g_piGLMinSymbolLineWidth;

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new oplaydo2_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    oplaydo2 PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

oplaydo2_pi::oplaydo2_pi(void *ppimgr)
      :opencpn_plugin_116 (ppimgr)
{
      // Create the PlugIn icons
      initialize_images();

	  wxFileName fn;
	  wxString tmp_path;

	  tmp_path = GetPluginDataDir("oplaydo2_pi");
	  fn.SetPath(tmp_path);
	  fn.AppendDir(_T("data"));
	  fn.SetFullName("oplaydo2_pi_panel_icon.png");

	  wxString shareLocn = fn.GetFullPath();
	  wxImage panelIcon(shareLocn);
	  
	  if (panelIcon.IsOk())
		  m_panelBitmap = wxBitmap(panelIcon);
	  else
		  wxLogMessage(_T("    oplaydo2_pi panel icon NOT loaded"));
	  m_bShowoplaydo2 = false;
}

oplaydo2_pi::~oplaydo2_pi(void)
{
     delete _img_oplaydo2_pi;
     delete _img_oplaydo2;
     
}

int oplaydo2_pi::Init(void)
{
      AddLocaleCatalog( _T("opencpn-oplaydo2_pi") );

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
	if(m_boplaydo2ShowIcon)

#ifdef oplaydo2_USE_SVG
		m_leftclick_tool_id = InsertPlugInToolSVG(_T("oplaydo2"), _svg_oplaydo2, _svg_oplaydo2, _svg_oplaydo2_toggled,
			wxITEM_CHECK, _("oplaydo2"), _T(""), NULL, CALCULATOR_TOOL_POSITION, 0, this);
#else
		m_leftclick_tool_id = InsertPlugInTool(_T(""), _img_oplaydo2, _img_oplaydo2, wxITEM_CHECK,
			_("oplaydo2"), _T(""), NULL,
			CALCULATOR_TOOL_POSITION, 0, this);
#endif
    

      m_pDialog = NULL;

      return (WANTS_OVERLAY_CALLBACK |
              WANTS_OPENGL_OVERLAY_CALLBACK |
		  
		      WANTS_CURSOR_LATLON      |
              WANTS_TOOLBAR_CALLBACK    |
              INSTALLS_TOOLBAR_TOOL     |
              WANTS_CONFIG             |
			  WANTS_PLUGIN_MESSAGING

           );
}

bool oplaydo2_pi::DeInit(void)
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

			m_bShowoplaydo2 = false;
			SetToolbarItemState( m_leftclick_tool_id, m_bShowoplaydo2 );

      }	
    
    SaveConfig();

    RequestRefresh(m_parent_window); // refresh mainn window

    return true;
}

int oplaydo2_pi::GetAPIVersionMajor()
{
      return atoi(API_VERSION);
}

int oplaydo2_pi::GetAPIVersionMinor()
{
    std::string v(API_VERSION);
    size_t dotpos = v.find('.');
    return atoi(v.substr(dotpos + 1).c_str());
}

int oplaydo2_pi::GetPlugInVersionMajor()
{
      return PLUGIN_VERSION_MAJOR;
}

int oplaydo2_pi::GetPlugInVersionMinor()
{
      return PLUGIN_VERSION_MINOR;
}

wxBitmap *oplaydo2_pi::GetPlugInBitmap()
{
      return &m_panelBitmap;
}

wxString oplaydo2_pi::GetCommonName()
{
      return _("oplaydo2");
}


wxString oplaydo2_pi::GetShortDescription()
{
      return _("oplaydo2 Positions using GPX files");
}

wxString oplaydo2_pi::GetLongDescription()
{
      return _("Creates GPX files with\n\
oplaydo2 Positions");
}

int oplaydo2_pi::GetToolbarToolCount(void)
{
      return 1;
}

void oplaydo2_pi::SetColorScheme(PI_ColorScheme cs)
{
      if (NULL == m_pDialog)
            return;

      DimeWindow(m_pDialog);
}

void oplaydo2_pi::OnToolbarToolCallback(int id)
{
    
	if(NULL == m_pDialog)
      {
            m_pDialog = new Dlg(m_parent_window, this);
            m_pDialog->Move(wxPoint(m_route_dialog_x, m_route_dialog_y));
      }

	  m_pDialog->Fit();
	  //Toggle 
	  m_bShowoplaydo2 = !m_bShowoplaydo2;	  

      //    Toggle dialog? 
      if(m_bShowoplaydo2) {
          m_pDialog->Show();         
      } else
          m_pDialog->Hide();
     
      // Toggle is handled by the toolbar but we must keep plugin manager b_toggle updated
      // to actual status to ensure correct status upon toolbar rebuild
      SetToolbarItemState( m_leftclick_tool_id, m_bShowoplaydo2 );

      RequestRefresh(m_parent_window); // refresh main window
}

bool oplaydo2_pi::LoadConfig(void)
{
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
            pConf->SetPath ( _T( "/Settings/oplaydo2_pi" ) );
			 pConf->Read ( _T( "Showoplaydo2Icon" ), &m_boplaydo2ShowIcon, 1 );
           
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

bool oplaydo2_pi::SaveConfig(void)
{
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
            pConf->SetPath ( _T ( "/Settings/oplaydo2_pi" ) );
			pConf->Write ( _T ( "Showoplaydo2Icon" ), m_boplaydo2ShowIcon );
          
            pConf->Write ( _T ( "DialogPosX" ),   m_route_dialog_x );
            pConf->Write ( _T ( "DialogPosY" ),   m_route_dialog_y );
            
            return true;
      }
      else
            return false;
}

void oplaydo2_pi::Onoplaydo2DialogClose()
{
    m_bShowoplaydo2 = false;
    SetToolbarItemState( m_leftclick_tool_id, m_bShowoplaydo2 );
    m_pDialog->Hide();
    SaveConfig();

    RequestRefresh(m_parent_window); // refresh main window

}


