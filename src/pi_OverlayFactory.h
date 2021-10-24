/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  otidalplan Plugin Freinds
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 */

#include <map>
#include <wx/string.h>
#include "bbox.h"
#include "ocpn_plugin.h"


using namespace std;


//----------------------------------------------------------------------------------------------------------
//    otidalplan Overlay Specification
//----------------------------------------------------------------------------------------------------------

class piOverlay {
public:
    piOverlay( void )
    {
        m_iTexture = 0;
        m_pDCBitmap = NULL, m_pRGBA = NULL;
    }

    ~piOverlay( void )
    {
        if(m_iTexture)
         // glDeleteTextures( 1, &m_iTexture );
        delete m_pDCBitmap, delete[] m_pRGBA;
    }

    unsigned int m_iTexture; /* opengl mode */

    wxBitmap *m_pDCBitmap; /* dc mode */
    unsigned char *m_pRGBA;

    int m_width;
    int m_height;
};

//----------------------------------------------------------------------------------------------------------
//    otidalplan Overlay Factory Specification
//----------------------------------------------------------------------------------------------------------

class Dlg;

class pi_OverlayFactory {
public:
   pi_OverlayFactory( Dlg &dlg );
    ~pi_OverlayFactory();

    void SetSettings( bool hiDefGraphics, bool GradualColors )
    {
      m_hiDefGraphics = hiDefGraphics;
      m_bGradualColors = GradualColors;
    }

    void SetMessage( wxString message ) { m_Message = message; }
    void SetTimeZone( int TimeZone ) { m_TimeZone = TimeZone; }
    void SetParentSize( int w, int h ) { m_ParentSize.SetWidth(w) ; m_ParentSize.SetHeight(h) ;}
	bool RenderGLpiOverlay( wxGLContext *pcontext, PlugIn_ViewPort *vp );
    bool RenderpiOverlay( wxDC &dc, PlugIn_ViewPort *vp );

	void DrawCurrentIndicators(PlugIn_ViewPort *BBox, bool bRebuildSelList,
		bool bforce_redraw_currents, bool bdraw_mono_for_mask, wxDateTime myTime);
	void DrawAllCurrentsInViewPort(PlugIn_ViewPort *BBox, bool bRebuildSelList,
        bool bforce_redraw_currents, bool bdraw_mono_for_mask, wxDateTime myTime);

    void Reset();
	wxImage &DrawGLText( double value, int precision);
	wxImage &DrawGLTextDir( double value, int precision);
	wxImage &DrawGLTextString( wxString myText);
	wxImage &DrawGLPolygon();


	void drawGLPolygons(pi_OverlayFactory *pof, wxDC *dc,
                                PlugIn_ViewPort *vp, 
                                wxImage &imageLabel, double myLat, double myLon, int offset );

	void DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2,
		bool b_hiqual);

	void DrawGLLine( double x1, double y1, double x2, double y2, double width, wxColour myColour );
	PlugIn_ViewPort *vp;
	bool              m_bShowRate;
    bool              m_bShowDirection;
	bool              m_bShowFillColour;
	wxDateTime        m_dtUseNew;

	void drawCurrentArrow(int x, int y, double rot_angle, double scale, double rate);
private:

	bool inGL;
	
    bool DoRenderpiOverlay( PlugIn_ViewPort *vp );

    void DrawMessageWindow( wxString msg, int x, int y , wxFont *mfont);

    double m_last_vp_scale;

	//  for GL
	wxColour c_GLcolour;
	wxPoint p_basic[9];
	//
    wxString m_Message;
    wxString m_Message_Hiden;
    int  m_TimeZone;
    wxSize  m_ParentSize;

    wxDC *m_pdc;
    wxGraphicsContext *m_gdc;

    wxFont *m_dFont_map;
    wxFont *m_dFont_war;

    bool m_hiDefGraphics;
    bool m_bGradualColors;

    std::map < double , wxImage > m_labelCache;
	std::map < wxString , wxImage > m_labelCacheText;

    Dlg &m_dlg;

    wxBoundingBox *myBox;   
    LLBBox * myLLBox;

	wxString        *pTC_Dir;

};