/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  oplaydo2 Object
 * Author:   David Register, Mike Rossiter
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
 *
 */

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/glcanvas.h>
#include <wx/graphics.h>
#include <wx/progdlg.h>

#include "oplaydo2gui_impl.h"
#include "oplaydo2gui.h"
#include "pi_OverlayFactory.h"
#include "pi_ocpndc.h"

#include <vector>
#include "bbox.h"



#ifdef __WXMSW__
#define snprintf _snprintf
#endif // __WXMSW__

using namespace std;

class Dlg;
class pi_ocpnDC;

#define NUM_CURRENT_ARROW_POINTS 9
static wxPoint CurrentArrowArray[NUM_CURRENT_ARROW_POINTS] = { wxPoint( 0, 0 ), wxPoint( 0, -10 ),
        wxPoint( 55, -10 ), wxPoint( 55, -25 ), wxPoint( 100, 0 ), wxPoint( 55, 25 ), wxPoint( 55,
                10 ), wxPoint( 0, 10 ), wxPoint( 0, 0 )
                                                             };

//----------------------------------------------------------------------------------------------------------
//  pin Overlay Factory Implementation
//----------------------------------------------------------------------------------------------------------
pi_OverlayFactory::pi_OverlayFactory( Dlg &dlg )
	: m_dlg(dlg)
{
    m_dFont_map = new wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
    m_dFont_war = new wxFont( 16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL );

	m_last_vp_scale = 0.;
	m_dtUseNew = wxDateTime::UNow();
	
}

pi_OverlayFactory::~pi_OverlayFactory()
{

}

void pi_OverlayFactory::Reset()
{

}

bool pi_OverlayFactory::RenderGLpiOverlay( wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
    m_pdc = NULL;  // inform lower layers that this is OpenGL render

    DrawCurrentIndicators(vp,false,false,false, m_dtUseNew);
    return true;

}

bool pi_OverlayFactory::RenderpiOverlay( wxDC &dc, PlugIn_ViewPort *vp )
{

#if wxUSE_GRAPHICS_CONTEXT
    wxMemoryDC *pmdc;
    pmdc = wxDynamicCast(&dc, wxMemoryDC);
    wxGraphicsContext *pgc = wxGraphicsContext::Create( *pmdc );
    m_gdc = pgc;
    m_pdc = &dc;
#else
    m_pdc = &dc;
#endif
	   
	m_pdc = &dc;

	DrawCurrentIndicators(vp,false,false,false, m_dtUseNew);
    return true;

}



bool pi_OverlayFactory::DoRenderpiOverlay( PlugIn_ViewPort *vp )
{
    
    m_Message_Hiden.Empty();
    m_last_vp_scale = vp->view_scale_ppm;
	
    if( !m_Message_Hiden.IsEmpty() )
        DrawMessageWindow( m_Message_Hiden , vp->pix_width, vp->pix_height, m_dFont_map );
	    DrawMessageWindow( m_Message_Hiden , vp->pix_width, vp->pix_height, m_dFont_map );
    return true;
}


void pi_OverlayFactory::DrawMessageWindow( wxString msg, int x, int y , wxFont *mfont)
{
    if(msg.empty())
        return;

    wxMemoryDC mdc;
    wxBitmap bm( 1000, 1000 );
    mdc.SelectObject( bm );
    mdc.Clear();

    mdc.SetFont( *mfont );
    mdc.SetPen( *wxTRANSPARENT_PEN);
    mdc.SetBrush( wxColour(243, 229, 47 ) );
    int w, h;
    mdc.GetMultiLineTextExtent( msg, &w, &h );
    h += 2;
    int label_offset = 10;
    int wdraw = w + ( label_offset * 2 );
    mdc.DrawRectangle( 0, 0, wdraw, h );

    mdc.DrawLabel( msg, wxRect( label_offset, 0, wdraw, h ), wxALIGN_LEFT| wxALIGN_CENTRE_VERTICAL);
    mdc.SelectObject( wxNullBitmap );

    wxBitmap sbm = bm.GetSubBitmap( wxRect( 0, 0, wdraw, h ) );

   // DrawOLBitmap( sbm, 0, y - ( GetChartbarHeight() + h ), false );
}


void pi_OverlayFactory::drawCurrentArrow(int x, int y, double rot_angle, double scale, double rate )
{   	
	double m_rate = rate;
	wxPoint p[9];

    wxColour colour;	
		
	c_GLcolour = colour;  // for filling GL arrows

	wxPen pen( colour, 2 );
	wxBrush brush(colour);

    if( m_pdc ) {
        m_pdc->SetPen( pen );
        m_pdc->SetBrush( brush);  
    }

   
	if( scale > 1e-2 ) {

        float sin_rot = sin( rot_angle * PI / 180. );
        float cos_rot = cos( rot_angle * PI / 180. );

        // Move to the first point

        float xt = CurrentArrowArray[0].x;
        float yt = CurrentArrowArray[0].y;

        float xp = ( xt * cos_rot ) - ( yt * sin_rot );
        float yp = ( xt * sin_rot ) + ( yt * cos_rot );
        int x1 = (int) ( xp * scale );
        int y1 = (int) ( yp * scale );

		p[0].x = x;
		p[0].y = y;

		p_basic[0].x = 100;
		p_basic[0].y = 100;

        // Walk thru the point list
        for( int ip = 1; ip < NUM_CURRENT_ARROW_POINTS; ip++ ) {
            xt = CurrentArrowArray[ip].x;
            yt = CurrentArrowArray[ip].y;

            float xp = ( xt * cos_rot ) - ( yt * sin_rot );
            float yp = ( xt * sin_rot ) + ( yt * cos_rot );
            int x2 = (int) ( xp * scale );
            int y2 = (int) ( yp * scale );

			p_basic[ip].x = 100 + x2;
			p_basic[ip].y = 100 + y2;

			if (m_pdc){
				m_pdc->DrawLine( x1 + x, y1 + y, x2 + x, y2 + y );
			}
			else{
				DrawGLLine(x1 + x, y1 + y, x2 + x, y2 + y , 2, colour);
			}
			p[ip].x = x1 + x; 
            p[ip].y = y1 + y;

			x1 = x2;
            y1 = y2;            			
         }

		//p[9].x = x1;
		//p[9].y = y1;

		if( m_bShowFillColour && m_pdc){
			m_pdc->SetBrush(brush);
			m_pdc->DrawPolygon(9,p);
		}

    }
}

wxImage &pi_OverlayFactory::DrawGLText( double value, int precision ){

	wxString labels;

	int p = precision;

	labels.Printf( _T("%.*f"), p, value );
	
	wxMemoryDC mdc(wxNullBitmap);

	wxFont *pTCFont;
					pTCFont = wxTheFontList->FindOrCreateFont( 12, wxDEFAULT, wxNORMAL, wxBOLD, FALSE,
                                                   wxString( _T ( "Eurostile Extended" ) ) );
    mdc.SetFont(*pTCFont);

    int w, h;
    mdc.GetTextExtent(labels, &w, &h);

    int label_offset = 10;   //5

    wxBitmap bm(w +  label_offset*2, h + 1);
    mdc.SelectObject(bm);
    mdc.Clear();

    wxColour text_color;

    GetGlobalColor( _T ("UINFD" ), &text_color );
    wxPen penText(text_color);
	mdc.SetPen(penText);

    mdc.SetBrush(*wxTRANSPARENT_BRUSH);
    mdc.SetTextForeground(text_color);
    mdc.SetTextBackground(wxTRANSPARENT);
          
    int xd = 0;
    int yd = 0;

    mdc.DrawText(labels, label_offset + xd, yd+1);
          
    mdc.SelectObject(wxNullBitmap);

    m_labelCache[value] = bm.ConvertToImage();

    m_labelCache[value].InitAlpha();

    wxImage &image = m_labelCache[value];

    unsigned char *d = image.GetData();
    unsigned char *a = image.GetAlpha();

    w = image.GetWidth(), h = image.GetHeight();
    for( int y = 0; y < h; y++ )
        for( int x = 0; x < w; x++ ) {
            int r, g, b;
            int ioff = (y * w + x);
            r = d[ioff* 3 + 0];
            g = d[ioff* 3 + 1];
            b = d[ioff* 3 + 2];

            a[ioff] = 255-(r+g+b)/3;
        }
		
		return m_labelCache[value];
}

wxImage &pi_OverlayFactory::DrawGLTextDir( double value, int precision ){

	wxString labels;

	int p = precision;

	labels.Printf( _T("%03.*f"), p, value );
	
	wxMemoryDC mdc(wxNullBitmap);

	wxFont *pTCFont;
					pTCFont = wxTheFontList->FindOrCreateFont( 12, wxDEFAULT, wxNORMAL, wxBOLD, FALSE,
                                                   wxString( _T ( "Eurostile Extended" ) ) );

    mdc.SetFont(*pTCFont);

    int w, h;
    mdc.GetTextExtent(labels, &w, &h);

    int label_offset = 10;   //5

    wxBitmap bm(w +  label_offset*2, h + 1);
    mdc.SelectObject(bm);
    mdc.Clear();

    wxColour text_color;

    GetGlobalColor( _T ("UINFD" ), &text_color );
    wxPen penText(text_color);
	mdc.SetPen(penText);

    mdc.SetBrush(*wxTRANSPARENT_BRUSH);
    mdc.SetTextForeground(text_color);
    mdc.SetTextBackground(wxTRANSPARENT);
          
    int xd = 0;
    int yd = 0;

    mdc.DrawText(labels, label_offset + xd, yd+1);
          
    mdc.SelectObject(wxNullBitmap);

    m_labelCache[value] = bm.ConvertToImage();

    m_labelCache[value].InitAlpha();

    wxImage &image = m_labelCache[value];

    unsigned char *d = image.GetData();
    unsigned char *a = image.GetAlpha();

    w = image.GetWidth(), h = image.GetHeight();
    for( int y = 0; y < h; y++ )
        for( int x = 0; x < w; x++ ) {
            int r, g, b;
            int ioff = (y * w + x);
            r = d[ioff* 3 + 0];
            g = d[ioff* 3 + 1];
            b = d[ioff* 3 + 2];

            a[ioff] = 255-(r+g+b)/3;
        }
		
		return m_labelCache[value];
}

wxImage &pi_OverlayFactory::DrawGLTextString( wxString myText ){

	wxString labels;
	labels = myText;
		
	wxMemoryDC mdc(wxNullBitmap);

	wxFont *pTCFont;
					pTCFont = wxTheFontList->FindOrCreateFont( 12, wxDEFAULT, wxNORMAL, wxBOLD, FALSE,
                                                   wxString( _T ( "Eurostile Extended" ) ) );	
    mdc.SetFont( *pTCFont);

    int w, h;
    mdc.GetTextExtent(labels, &w, &h);

    int label_offset = 10;   //5

    wxBitmap bm(w +  label_offset*2, h + 1);
    mdc.SelectObject(bm);
    mdc.Clear();

    wxColour text_color;

    GetGlobalColor( _T ("UINFD" ), &text_color );
    wxPen penText(text_color);
	mdc.SetPen(penText);

    mdc.SetBrush(*wxTRANSPARENT_BRUSH);
    mdc.SetTextForeground(text_color);
    mdc.SetTextBackground(wxTRANSPARENT);
          
    int xd = 0;
    int yd = 0;

    mdc.DrawText(labels, label_offset + xd, yd+1);          
    mdc.SelectObject(wxNullBitmap);

    m_labelCacheText[myText] = bm.ConvertToImage();

    m_labelCacheText[myText].InitAlpha();

    wxImage &image = m_labelCacheText[myText];

    unsigned char *d = image.GetData();
    unsigned char *a = image.GetAlpha();

    w = image.GetWidth(), h = image.GetHeight();
    for( int y = 0; y < h; y++ )
        for( int x = 0; x < w; x++ ) {
            int r, g, b;
            int ioff = (y * w + x);
            r = d[ioff* 3 + 0];
            g = d[ioff* 3 + 1];
            b = d[ioff* 3 + 2];

            a[ioff] = 255-(r+g+b)/3;
        }
	
    return m_labelCacheText[myText];
}


void pi_OverlayFactory::DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2,
                         bool b_hiqual) {
  if (m_pdc) m_pdc->DrawLine(x1, y1, x2, y2);
#ifdef ocpnUSE_GL
  else if (ConfigurePen()) {
    bool b_draw_thick = false;

    float pen_width = wxMax(g_piGLMinSymbolLineWidth, m_pen.GetWidth());

    //      Enable anti-aliased lines, at best quality
    if (b_hiqual) {
      SetGLStipple();

#ifndef __WXQT__
      glEnable(GL_BLEND);
      glEnable(GL_LINE_SMOOTH);
#endif

      if (pen_width > 1.0) {
        GLint parms[2];
        glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE, &parms[0]);
        if (glGetError()) glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, &parms[0]);
        if (pen_width > parms[1])
          b_draw_thick = true;
        else
          glLineWidth(pen_width);
      } else
        glLineWidth(pen_width);
    } else {
      if (pen_width > 1) {
        GLint parms[2];
        glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, &parms[0]);
        if (pen_width > parms[1])
          b_draw_thick = true;
        else
          glLineWidth(pen_width);
      } else
        glLineWidth(pen_width);
    }

#ifdef USE_ANDROID_GLES2
    if (b_draw_thick)
      piDrawGLThickLine(x1, y1, x2, y2, m_pen, b_hiqual);
    else {
      glUseProgram(pi_color_tri_shader_program);

      float fBuf[4];
      GLint pos = glGetAttribLocation(pi_color_tri_shader_program, "position");
      glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                            fBuf);
      glEnableVertexAttribArray(pos);

      //             GLint matloc =
      //             glGetUniformLocation(pi_color_tri_shader_program,"MVMatrix");
      //             glUniformMatrix4fv( matloc, 1, GL_FALSE, (const
      //             GLfloat*)cc1->GetpVP()->vp_transform);

      float colorv[4];
      colorv[0] = m_pen.GetColour().Red() / float(256);
      colorv[1] = m_pen.GetColour().Green() / float(256);
      colorv[2] = m_pen.GetColour().Blue() / float(256);
      colorv[3] = 1.0;

      GLint colloc = glGetUniformLocation(pi_color_tri_shader_program, "color");
      glUniform4fv(colloc, 1, colorv);

      wxDash *dashes;
      int n_dashes = m_pen.GetDashes(&dashes);
      if (n_dashes) {
        float angle = atan2f((float)(y2 - y1), (float)(x2 - x1));
        float cosa = cosf(angle);
        float sina = sinf(angle);
        float t1 = m_pen.GetWidth();

        float lpix = sqrtf(powf(x1 - x2, 2) + powf(y1 - y2, 2));
        float lrun = 0.;
        float xa = x1;
        float ya = y1;
        float ldraw = t1 * dashes[0];
        float lspace = t1 * dashes[1];

        ldraw = wxMax(ldraw, 4.0);
        lspace = wxMax(lspace, 4.0);
        lpix = wxMin(lpix, 2000.0);

        while (lrun < lpix) {
          //    Dash
          float xb = xa + ldraw * cosa;
          float yb = ya + ldraw * sina;

          if ((lrun + ldraw) >= lpix)  // last segment is partial draw
          {
            xb = x2;
            yb = y2;
          }

          fBuf[0] = xa;
          fBuf[1] = ya;
          fBuf[2] = xb;
          fBuf[3] = yb;

          glDrawArrays(GL_LINES, 0, 2);

          xa = xa + (lspace + ldraw) * cosa;
          ya = ya + (lspace + ldraw) * sina;
          lrun += lspace + ldraw;
        }
      } else  // not dashed
      {
        fBuf[0] = x1;
        fBuf[1] = y1;
        fBuf[2] = x2;
        fBuf[3] = y2;

        glDrawArrays(GL_LINES, 0, 2);
      }
    }

#else
    if (b_draw_thick)
     // piDrawGLThickLine(x1, y1, x2, y2, m_pen, b_hiqual);
    else {
      wxDash *dashes;
      int n_dashes = m_pen.GetDashes(&dashes);
      if (n_dashes) {
        float angle = atan2f((float)(y2 - y1), (float)(x2 - x1));
        float cosa = cosf(angle);
        float sina = sinf(angle);
        float t1 = m_pen.GetWidth();

        float lpix = sqrtf(powf(x1 - x2, 2) + powf(y1 - y2, 2));
        float lrun = 0.;
        float xa = x1;
        float ya = y1;
        float ldraw = t1 * dashes[0];
        float lspace = t1 * dashes[1];

        ldraw = wxMax(ldraw, 4.0);
        lspace = wxMax(lspace, 4.0);
        lpix = wxMin(lpix, 2000.0);

        glBegin(GL_LINES);
        while (lrun < lpix) {
          //    Dash
          float xb = xa + ldraw * cosa;
          float yb = ya + ldraw * sina;

          if ((lrun + ldraw) >= lpix)  // last segment is partial draw
          {
            xb = x2;
            yb = y2;
          }

          glVertex2f(xa, ya);
          glVertex2f(xb, yb);

          xa = xa + (lspace + ldraw) * cosa;
          ya = ya + (lspace + ldraw) * sina;
          lrun += lspace + ldraw;
        }
        glEnd();
      } else  // not dashed
      {
        glBegin(GL_LINES);
        glVertex2i(x1, y1);
        glVertex2i(x2, y2);
        glEnd();
      }
    }
#endif
    glDisable(GL_LINE_STIPPLE);

    if (b_hiqual) {
      glDisable(GL_LINE_SMOOTH);
      glDisable(GL_BLEND);
    }
  }
#endif
}



void pi_OverlayFactory::DrawGLLine( double x1, double y1, double x2, double y2, double width, wxColour myColour )
{
    {
        wxColour isoLineColor = myColour;
		glColor4ub(isoLineColor.Red(), isoLineColor.Green(), isoLineColor.Blue(),
                     255/*isoLineColor.Alpha()*/);

		glPushAttrib(GL_COLOR_BUFFER_BIT | GL_LINE_BIT | GL_ENABLE_BIT |
                     GL_POLYGON_BIT | GL_HINT_BIT ); //Save state
        {

            //      Enable anti-aliased lines, at best quality
            glEnable( GL_LINE_SMOOTH );
            glEnable( GL_BLEND );
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
            glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

			#ifndef __OCPN__ANDROID__
    //      Enable anti-aliased lines, at best quality
				glEnable(GL_LINE_SMOOTH);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
				glLineWidth(2);
			#else
				glLineWidth(5);  // 5 pixels for dense displays
			#endif

            glBegin( GL_LINES );
            glVertex2d( x1, y1 );
            glVertex2d( x2, y2 );
            glEnd();
        }

        glPopAttrib();
    }
}

wxImage &pi_OverlayFactory::DrawGLPolygon(){

	wxString labels;
	labels = _T("");  // dummy label for drawing with
	
	wxColour c_orange = c_GLcolour;

    wxPen penText(c_orange);
    wxBrush backBrush(c_orange);

    wxMemoryDC mdc(wxNullBitmap);

    wxFont mfont( 9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
    mdc.SetFont( mfont );

    int w, h;
    mdc.GetTextExtent(labels, &w, &h);
	
	w = 200;
	h = 200;

    wxBitmap bm(w, h);
    mdc.SelectObject(bm);
    mdc.Clear();

    mdc.SetPen(penText);
    mdc.SetBrush(backBrush);
    mdc.SetTextForeground(c_orange);
    mdc.SetTextBackground(c_orange);
          
    int xd = 0;
    int yd = 0;
   // mdc.DrawRoundedRectangle(xd, yd, w+(label_offset * 2), h+2, -.25);
/*
		
	                wxPoint p2;  
					p2.x = 100;
					p2.y = 100 ;

	                wxPoint z[9];
					z[0].x = p2.x;
                    z[0].y = p2.y;
                    z[1].x = p2.x;
                    z[1].y = p2.y -10 ;
                    z[2].x = p2.x + 55;
                    z[2].y = p2.y - 10;
                    z[3].x = p2.x + 55;
                    z[3].y = p2.y -25;
					z[4].x = p2.x + 100;
                    z[4].y = p2.y;
                    z[5].x = p2.x +55;
                    z[5].y = p2.y + 25 ;
                    z[6].x = p2.x + 55;
                    z[6].y = p2.y + 10;
                    z[7].x = p2.x;
                    z[7].y = p2.y +10;
					z[8].x = p2.x;
                    z[8].y = p2.y;

	*/
    
	mdc.DrawPolygon(9,p_basic,0);
          
    mdc.SelectObject(wxNullBitmap);

    m_labelCacheText[labels] = bm.ConvertToImage();

    m_labelCacheText[labels].InitAlpha();

    wxImage &image = m_labelCacheText[labels];

    unsigned char *d = image.GetData();
    unsigned char *a = image.GetAlpha();

    w = image.GetWidth(), h = image.GetHeight();
    for( int y = 0; y < h; y++ )
        for( int x = 0; x < w; x++ ) {
            int r, g, b;
            int ioff = (y * w + x);
            r = d[ioff* 3 + 0];
            g = d[ioff* 3 + 1];
            b = d[ioff* 3 + 2];

            a[ioff] = 255-(r+g+b)/3;
        }

    return m_labelCacheText[labels];
	/*
	                wxPoint p;  
					p.x = 200;
					p.y = 200;

	                wxPoint z[9];
					z[0].x = p.x;
                    z[0].y = p.y;
                    z[1].x = p.x;
                    z[1].y = p.y -10 ;
                    z[2].x = p.x + 55;
                    z[2].y = p.y - 10;
                    z[3].x = p.x + 55;
                    z[3].y = p.y -25;
					z[4].x = p.x + 100;
                    z[4].y = p.y;
                    z[5].x = p.x +55;
                    z[5].y = p.y + 25 ;
                    z[6].x = p.x + 55;
                    z[6].y = p.y + 10;
                    z[7].x = p.x;
                    z[7].y = p.y +10;
					z[8].x = p.x;
                    z[8].y = p.y;

	
    
	*/

}

void pi_OverlayFactory::DrawCurrentIndicators
(PlugIn_ViewPort *BBox, bool bRebuildSelList,
	bool bforce_redraw_currents, bool bdraw_mono_for_mask, wxDateTime myTime)
{
	for (std::vector<Position>::iterator it = m_dlg.my_points.begin(); it != m_dlg.my_points.end(); it++) {

		double myLat;
		double myLon;
		double tcLat;
		double tcLon;

		wxColour colour("RED");

		myLat = (*it).myLat;
		myLon = (*it).myLon;

		tcLat = (*it).myNextLat;
		tcLon = (*it).myNextLon;
		

		myLLBox = new LLBBox;
		wxBoundingBox LLBBox(BBox->lon_min, BBox->lat_min, BBox->lon_max, BBox->lat_max);

		int pixxc, pixyc, pixxl, pixyl;
		wxPoint cpoint, lpoint;
		GetCanvasPixLL(BBox, &cpoint, tcLat, tcLon);
		pixxc = cpoint.x;
		pixyc = cpoint.y;

		GetCanvasPixLL(BBox, &lpoint, myLat, myLon);
		pixxl = lpoint.x;
		pixyl = lpoint.y;

		if (m_pdc) {
			m_pdc->DrawLine(pixxc, pixyc, pixxl, pixyl);
		}
		else {
			DrawGLLine(pixxc, pixyc, pixxl, pixyl, 3, colour);
		}
	}
	return;
/*
	//if (BBox->chart_scale > 1000000){
		//return;
	//}

	double rot_vp = BBox->rotation * 180 / M_PI;

	// Set up the scaler
	int mmx, mmy;
	wxDisplaySizeMM(&mmx, &mmy);

	int sx, sy;
	wxDisplaySize(&sx, &sy);

	double m_pix_per_mm = ((double)sx) / ((double)mmx);

	int mm_per_knot = 10;
	float current_draw_scaler = mm_per_knot * m_pix_per_mm * 100 / 100.0;

	// End setting up scaler

	//double tcvalue, dir;
	bool bnew_val = true;
	double lon_last = 0.;

	wxDateTime yn = Now();

	double myLat;
	double myLon;
	double tcLat;
	double tcLon;

	wxColour colour("RED");

	wxPen pen(colour, 3);
	wxBrush brush(colour);
	pen.SetStyle(wxLONG_DASH);
	

	if (m_pdc) {
		m_pdc->SetPen(pen);
		m_pdc->SetBrush(brush);
	}

	for (std::vector<tc>::iterator it = m_dlg.dummyTC.begin(); it != m_dlg.dummyTC.end(); it++) {

		myLat = (*it).lat;
		myLon = (*it).lon;

		tcLat = (*it).tcLat;
		tcLon = (*it).tcLon;

	
		myLLBox = new LLBBox;
		wxBoundingBox LLBBox(BBox->lon_min, BBox->lat_min, BBox->lon_max, BBox->lat_max);

		//if( LLBBox.PointInBox( myLon, myLat, 0 )   )  {


			   // wxMessageBox(wxString::Format(_T("%5.2f"), tcvalue));

		int pixxc, pixyc, pixxl, pixyl;
		wxPoint cpoint, lpoint;
		GetCanvasPixLL(BBox, &cpoint, tcLat, tcLon);
		pixxc = cpoint.x;
		pixyc = cpoint.y;

		GetCanvasPixLL(BBox, &lpoint, myLat, myLon);
		pixxl = lpoint.x;
		pixyl = lpoint.y;

		if (m_pdc) {
			m_pdc->DrawLine(pixxc, pixyc,pixxl, pixyl);
		}
		else {
			DrawGLLine(pixxc, pixyc, pixxl, pixyl, 3, colour);
		}

		

		// }

	}
*/
}


void pi_OverlayFactory::DrawAllCurrentsInViewPort(PlugIn_ViewPort *BBox, bool bRebuildSelList,
        bool bforce_redraw_currents, bool bdraw_mono_for_mask, wxDateTime myTime)
{
	return;
	/*
	if (!m_dlg.b_showTidalArrow){
		return;
	}

	//if (BBox->chart_scale > 1000000){
		//return;
	//}

    double rot_vp = BBox->rotation*180/M_PI;

	// Set up the scaler
	int mmx, mmy;
	wxDisplaySizeMM( &mmx, &mmy );

	int sx, sy;
	wxDisplaySize( &sx, &sy );

	double m_pix_per_mm = ( (double) sx ) / ( (double) mmx );

	int mm_per_knot = 10;
	float current_draw_scaler = mm_per_knot * m_pix_per_mm * 100 / 100.0;

	// End setting up scaler

    double tcvalue, dir;
    bool bnew_val = true;
    double lon_last = 0.;

	wxDateTime yn = m_dlg.m_dtNow; 

	double myLat;
	double myLon;

	for (std::list<Arrow>::iterator it = m_dlg.m_arrowList.begin();
		it != m_dlg.m_arrowList.end(); it++){

		myLat = (*it).m_lat;
		myLon = (*it).m_lon;
		dir = (*it).m_dir;
		tcvalue = (*it).m_force;

		//wxMessageBox(_T("here"));
		
		

				
				myLLBox = new LLBBox;
				wxBoundingBox LLBBox( BBox->lon_min, BBox->lat_min , BBox->lon_max, BBox->lat_max );
							
				//if( LLBBox.PointInBox( myLon, myLat, 0 )   )  {

					
					   // wxMessageBox(wxString::Format(_T("%5.2f"), tcvalue));
                        									
						int pixxc, pixyc;
						wxPoint cpoint;
                        GetCanvasPixLL(BBox, &cpoint, myLat, myLon);
                        pixxc = cpoint.x;
                        pixyc = cpoint.y;  
                          
						//wxMessageBox(wxString::Format(_T("%5.2f"), myLat));
						//    Adjust drawing size using logarithmic scale
						double a1 = tcvalue*5;
                       // a1 = wxMax(5.0, a1);      // Current values less than 0.1 knot
												// will be displayed as 0
                        double a2 = log10( a1 );
                        double scale = current_draw_scaler * a2;

						drawCurrentArrow( pixxc, pixyc,	dir - 90 + rot_vp , scale/30, tcvalue );
                               
						int shift = 0;

						if(!m_pdc && m_bShowFillColour){
							drawGLPolygons(this, m_pdc, BBox, DrawGLPolygon(), myLat, myLon, shift);
						}


					   if (!m_pdc){
						   if( m_bShowRate){
                          
							  DrawGLLabels( this, m_pdc, BBox, 
									  DrawGLText( fabs(tcvalue), 1), myLat, myLon, 0 ) ;
							  shift = 13;
						   }
						   if( m_bShowDirection){
						  
								DrawGLLabels( this, m_pdc, BBox, 
											DrawGLTextDir(dir, 0), myLat, myLon, shift) ;
						   }
					   }			

						wxFont *pTCFont;
						pTCFont = wxTheFontList->FindOrCreateFont( 12, wxDEFAULT, wxNORMAL, wxBOLD, FALSE,
													   wxString( _T ( "Eurostile Extended" ) ) );
						char sbuf[20];					 
					
						if( m_bShowRate && m_pdc ) 
						{
							m_pdc->SetFont( *pTCFont );
							snprintf( sbuf, 19, "%3.1f", fabs(tcvalue) );
							m_pdc->DrawText( wxString( sbuf, wxConvUTF8 ), pixxc, pixyc );
							shift = 13;
						}					 
					
						if ( m_bShowDirection && m_pdc)	
						{	
							snprintf( sbuf, 19, "%03.0f", dir );
							m_pdc->DrawText( wxString( sbuf, wxConvUTF8 ), pixxc, pixyc + shift );
						}


                   // }
                    
         }              
      */      
}        
