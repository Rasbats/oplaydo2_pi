/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Climatology Plugin
 * Author:   Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2018 by Sean D'Epagnier                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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
 *
 */

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/vector.h>

#ifdef __WXOSX__
# include <OpenGL/OpenGL.h>
# include <OpenGL/gl3.h>
#endif

#ifdef __OCPN__ANDROID__
#include <qopengl.h>
#include "gl_private.h"
#endif

#ifdef USE_ANDROID_GLES2
#include "GLES2/gl2.h"
#endif

#include "oplaydo2_pi.h"
#include "icons.h"


#define FAILED_FILELIST_MSG_LEN 150

class piDC;

//static int s_multitexturing = 0;
//static PFNGLACTIVETEXTUREARBPROC s_glActiveTextureARB = 0;
//static PFNGLMULTITEXCOORD2DARBPROC s_glMultiTexCoord2dARB = 0;

static int texture_format;
static bool glQueried = false;

static GLboolean QueryExtension( const char *extName )
{
    /*
     ** Search for extName in the extensions string. Use of strstr()
     ** is not sufficient because extension names can be prefixes of
     ** other extension names. Could use strtok() but the constant
     ** string returned by glGetString might be in read-only memory.
     */
    char *p;
    char *end;
    int extNameLen;

    extNameLen = strlen( extName );

    p = (char *) glGetString( GL_EXTENSIONS );
    if( NULL == p ) {
        return GL_FALSE;
    }

    end = p + strlen( p );

    while( p < end ) {
        int n = strcspn( p, " " );
        if( ( extNameLen == n ) && ( strncmp( extName, p, n ) == 0 ) ) {
            return GL_TRUE;
        }
        p += ( n + 1 );
    }
    return GL_FALSE;
}

#if defined(__WXMSW__)
#define systemGetProcAddress(ADDR) wglGetProcAddress(ADDR)
#elif defined(__WXOSX__)
#include <dlfcn.h>
#define systemGetProcAddress(ADDR) dlsym( RTLD_DEFAULT, ADDR)
#elif defined(__OCPN__ANDROID__)
#define systemGetProcAddress(ADDR) eglGetProcAddress(ADDR)
#else
#define systemGetProcAddress(ADDR) glXGetProcAddress((const GLubyte*)ADDR)
#endif

double deg2rad(double degrees)
{
  return M_PI * degrees / 180.0;
}

piOverlay::~piOverlay()
{
   // if(m_iTexture)
      //  glDeleteTextures( 1, &m_iTexture );
    //delete m_pDCBitmap, delete[] m_pRGBA;
}




piOverlayFactory::piOverlayFactory( Dlg &dlg )
    : m_dlg(dlg)
{
    // make sure the user data directory exists
   

}

piOverlayFactory::~piOverlayFactory()
{
   
}


bool piOverlayFactory::RenderOverlay(piDC &dc, PlugIn_ViewPort &vp)
{
	m_dc = &dc;	

	if (!dc.GetDC()) {
		if (!glQueried) {
			
			glQueried = true;
		}
#ifndef USE_GLSL
		glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT); //Save state

		//      Enable anti-aliased lines, at best quality
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
		glEnable(GL_BLEND);
	}

	wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	m_dc->SetFont(font);

	wxColour myColour = wxColour("RED");
	std::vector<Position> mypoints =  m_dlg.my_points;
	
	for (std::vector<Position>::iterator it = mypoints.begin(); it != mypoints.end(); it++) {


		wxPoint p, pn;
        GetCanvasPixLL( &vp, &p, (it)->myLat, (it)->myLon);
		GetCanvasPixLL( &vp, &pn, (it)->myNextLat, (it)->myNextLon);

        DrawLine(p.x, p.y, pn.x, pn.y, myColour, 4);	

		

		


	}
	
	
	
	DrawLine(40, 40, 120, 120, myColour, 4);

	polyPoints[0] = wxPoint(48, 160);
	polyPoints[1] = wxPoint(108, 160);
	polyPoints[2] = wxPoint(108, 220);
	polyPoints[3] = wxPoint(48, 160);

	m_dc->ConfigurePen();
	m_dc->SetPen( wxPen(myColour, 1 ));
	m_dc->ConfigureBrush();
    m_dc->SetBrush(myColour);
	m_dc->DrawPolygon(4, polyPoints, 0, 0, 1.0, 0);
	
	
    return true;
}


void piOverlayFactory::DrawLine( double x1, double y1, double x2, double y2,
                                          const wxColour &color, double width )
{
	m_dc->ConfigurePen();
	m_dc->SetPen( wxPen(color, width ) );
	m_dc->ConfigureBrush();
    m_dc->SetBrush( *wxTRANSPARENT_BRUSH);
    m_dc->DrawLine(x1, y1, x2, y2, false);

    m_dc->ConfigurePen();
	m_dc->SetPen( wxPen(color, width ) );

	wxFont font( 16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
    m_dc->SetFont( font );

    m_dc->DrawText("testing", 150, 150);

	}


void piOverlayFactory::DrawNumbers(wxPoint p, double value, int settings,
                                     wxColour back_color) {

#ifdef ocpnUSE_GL
#ifndef USE_ANDROID_GLES2

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(back_color.Red(), back_color.Green(), back_color.Blue(),
              0);

    glLineWidth(1);

    wxString label = "testing";
    int w, h;
	TexFont m_TexFontNumbers;
	m_TexFontNumbers.GetTextExtent(label, &w, &h);

    int label_offsetx = 5, label_offsety = 1;
    int x = p.x - label_offsetx, y = p.y - label_offsety;
    w += 2 * label_offsetx, h += 2 * label_offsety;

    /* draw bounding rectangle */
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);
    glEnd();

    glColor4ub(0, 0, 0, 0);

    glBegin(GL_LINE_LOOP);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    m_TexFontNumbers.RenderString(label, p.x, p.y);
    glDisable(GL_TEXTURE_2D);
#else

#ifdef __WXQT__
    wxFont font = GetOCPNGUIScaledFont_PlugIn(_T("Dialog"));
#else
    wxFont font(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                wxFONTWEIGHT_NORMAL);
#endif

    wxString label = "testing";

    m_dc->SetFont(font);
    int w, h;
    m_dc->GetTextExtent(label, &w, &h);

    int label_offsetx = 5, label_offsety = 1;
    int x = p.x - label_offsetx, y = p.y - label_offsety;
    w += 2 * label_offsetx, h += 2 * label_offsety;

    m_dc->SetBrush(wxBrush(back_color));
    m_dc->DrawRoundedRectangle(x, y, w, h, 0);

    /* draw bounding rectangle */
    m_dc->SetPen(wxPen(wxColour(0, 0, 0), 1));
    m_dc->DrawLine(x, y, x + w, y);
    m_dc->DrawLine(x + w, y, x + w, y + h);
    m_dc->DrawLine(x + w, y + h, x, y + h);
    m_dc->DrawLine(x, y + h, x, y);

    m_dc->DrawText(label, p.x, p.y);

#endif
#endif
  
}



void piOverlayFactory::Plot(piDC *dc, PlugIn_ViewPort *vp, wxColour color) {	

	wxFont font(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC,
		wxFONTWEIGHT_NORMAL);

	dc->SetPen(wxPen(color, 3));
	dc->SetTextForeground(color);
	dc->SetFont(font);
#if 0
	if (dc) {
		dc->SetPen(wxPen(color, 3));
		dc->SetTextForeground(color);
		dc->SetFont(font);
	}
	else {
		glLineWidth(3.0);
		glColor4ub(color.Red(), color.Green(), color.Blue(), color.Alpha());
		m_TexFont.Build(font);
	}
#endif

	wxColour myColour = wxColour("RED");
	std::vector<Position> mypoints =  m_dlg.my_points;

	for (std::vector<Position>::iterator it = mypoints.begin(); it != mypoints.end(); it++) {


	wxPoint p, pn;
        GetCanvasPixLL( vp, &p, (it)->myLat, (it)->myLon);
		GetCanvasPixLL( vp, &pn, (it)->myNextLat, (it)->myNextLon);

        DrawLine(p.x, p.y, pn.x, pn.y, myColour, 4);	



	}
	
	
	
	DrawLine(40, 40, 120, 120, myColour, 4);

}

