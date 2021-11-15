/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Layer to perform wxDC drawing using wxDC or opengl
 * Author:   Jon Gough based on work by Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2011 by Sean D'Epagnier                                 *
 *   sean at depagnier dot com                                             *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 *
 */

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif

#ifdef __MSVC__
#include <windows.h>
#endif

#ifdef __WXMSW__
   // #include "GL/gl.h"            // local copy for Windows
   // #include "GL/glext.h"
   // #include <GL/glu.h>
#else

    #ifndef __OCPN__ANDROID__
        //#include <GL/gl.h>
       // #include <GL/glu.h>
    #else
        #include "qopengl.h"                  // this gives us the qt runtime gles2.h
        #include "gl_private.h"
        #include "pi_shaders.h"
    #endif

#endif

#ifdef ocpnUSE_GL
#include <wx/glcanvas.h>
#endif

#include <wx/graphics.h>
#include <wx/dcclient.h>

#include <vector>


#include "oplaydo2_pi.h"
#include "ODdc.h"
//#include "ODUtils.h"
//#include "wx28compat.h"
// Needed for ocpndc.cpp to compile. Normally would be in glChartCanvas.cpp
float g_GLMinSymbolLineWidth;

#define GL_ALIASED_LINE_WIDTH_RANGE     0x846E
#define GL_SMOOTH_LINE_WIDTH_RANGE      0x0B22

#define __CALL_CONVENTION

wxArrayPtrVoid gTesselatorVertices;

// These are only global to this module to allow tessellation callbacks to access them. Tessellation does not handle classes and methods
bool        g_bTexture2D;
int         g_iTextureHeight;
int         g_iTextureWidth;

//----------------------------------------------------------------------------
/* pass the dc to the constructor, or NULL to use opengl */
ODDC::ODDC( wxGLCanvas &canvas ) :
        glcanvas( &canvas ), dc( NULL ), m_pen( wxNullPen ), m_brush( wxNullBrush )
{
#if wxUSE_GRAPHICS_CONTEXT
    pgc = NULL;
#endif
#ifdef ocpnUSE_GL
    m_textforegroundcolour = wxColour( 0, 0, 0 );
    g_bTexture2D = false;
#endif    
}

ODDC::ODDC( wxDC &pdc ) :
        glcanvas( NULL ), dc( &pdc ), m_pen( wxNullPen ), m_brush( wxNullBrush )
{
#if wxUSE_GRAPHICS_CONTEXT
    pgc = NULL;
    wxMemoryDC *pmdc = wxDynamicCast(dc, wxMemoryDC);
    if( pmdc ) pgc = wxGraphicsContext::Create( *pmdc );
    else {
        wxClientDC *pcdc = wxDynamicCast(dc, wxClientDC);
        if( pcdc ) pgc = wxGraphicsContext::Create( *pcdc );
    }
#endif
    m_textforegroundcolour = wxColour( 0, 0, 0 );
    g_bTexture2D = false;
    
}

ODDC::ODDC() :
        glcanvas( NULL ), dc( NULL ), m_pen( wxNullPen ), m_brush( wxNullBrush )
{
#if wxUSE_GRAPHICS_CONTEXT
    pgc = NULL;
#endif
    g_bTexture2D = false;
}

ODDC::~ODDC()
{
#if wxUSE_GRAPHICS_CONTEXT
    if( pgc ) delete pgc;
#endif
}

void ODDC::Clear()
{
    if( dc ) dc->Clear();
    else {
#ifdef ocpnUSE_GL
        wxBrush tmpBrush = m_brush;
        int w, h;
        SetBrush( wxBrush( glcanvas->GetBackgroundColour() ) );
        glcanvas->GetSize( &w, &h );
        DrawRectangle( 0, 0, w, h );
        SetBrush( tmpBrush );
#endif        
    }
}

void ODDC::SetBackground( const wxBrush &brush )
{
    if( dc )
        dc->SetBackground( brush );
    else {
#ifdef ocpnUSE_GL
        glcanvas->SetBackgroundColour( brush.GetColour() );
#endif
    }
}

void ODDC::SetGLAttrs( bool highQuality )
{
#ifdef ocpnUSE_GL
    
    //      Enable anti-aliased polys, at best quality
    if( highQuality ) {
        glEnable( GL_LINE_SMOOTH );
        glEnable( GL_POLYGON_SMOOTH );
        glEnable( GL_BLEND );
    } else {
        glDisable(GL_LINE_SMOOTH);
        glDisable( GL_POLYGON_SMOOTH );
        glDisable( GL_BLEND );
    }
    
#endif
}

void ODDC::SetPen( const wxPen &pen )
{
    if( dc ) {
        if( pen == wxNullPen ) dc->SetPen( *wxTRANSPARENT_PEN );
        else
            dc->SetPen( pen );
    } else
        m_pen = pen;
}

void ODDC::SetBrush( const wxBrush &brush )
{
    if( dc ) dc->SetBrush( brush );
    else
        m_brush = brush;
}

void ODDC::SetTextForeground( const wxColour &colour )
{
    if( dc ) dc->SetTextForeground( colour );
    else
        m_textforegroundcolour = colour;
}

void ODDC::SetFont( const wxFont& font )
{
    if( dc ) dc->SetFont( font );
    else
        m_font = font;
}

const wxPen& ODDC::GetPen() const
{
    if( dc ) return dc->GetPen();
    return m_pen;
}

const wxBrush& ODDC::GetBrush() const
{
    if( dc ) return dc->GetBrush();
    return m_brush;
}

const wxFont& ODDC::GetFont() const
{
    if( dc ) return dc->GetFont();
    return m_font;
}

void ODDC::GetSize( wxCoord *width, wxCoord *height ) const
{
    if( dc )
        dc->GetSize( width, height );
    else {
#ifdef ocpnUSE_GL
        glcanvas->GetSize( width, height );
#endif
    }
}

void ODDC::SetVP(PlugIn_ViewPort *vp)
{
#ifdef USE_ANDROID_GLES2
    configureShaders(vp->pix_width, vp->pix_height);
#endif
    m_vpSize = wxSize(vp->pix_width, vp->pix_height);
}


#ifdef ocpnUSE_GL
/* draw a half circle using triangles */
void ODDC::DrawEndCap(float x1, float y1, float t1, float angle)
{
    const int steps = 16;
    float xa, ya;
    bool first = true;
    for(int i = 0; i <= steps; i++) {
        float a = angle + M_PI/2 + M_PI/steps*i;

        float xb = x1 + t1 / 2 * cos( a );
        float yb = y1 + t1 / 2 * sin( a );
        if(first)
            first = false;
        else {
            glVertex2f( x1, y1 );
            glVertex2f( xa, ya );
            glVertex2f( xb, yb );
        }
        xa = xb, ya = yb;
    }
}
#endif

// Draws a line between (x1,y1) - (x2,y2) with a start thickness of t1
void ODDC::DrawGLThickLine( float x1, float y1, float x2, float y2, wxPen pen, bool b_hiqual )
{
#ifdef ocpnUSE_GL
    
    float angle = atan2f( y2 - y1, x2 - x1 );
    float t1 = pen.GetWidth();
    float t2sina1 = t1 / 2 * sinf( angle );
    float t2cosa1 = t1 / 2 * cosf( angle );

    glBegin( GL_TRIANGLES );

    //    n.b.  The dwxDash interpretation for GL only allows for 2 elements in the dash table.
    //    The first is assumed drawn, second is assumed space
    wxDash *dashes;
    int n_dashes = pen.GetDashes( &dashes );
    if( n_dashes ) {
        float lpix = sqrtf( powf( (float) (x1 - x2), 2) + powf( (float) (y1 - y2), 2) );
        float lrun = 0.;
        float xa = x1;
        float ya = y1;
        float ldraw = t1 * dashes[0];
        float lspace = t1 * dashes[1];

        while( lrun < lpix ) {
            //    Dash
            float xb = xa + ldraw * cosf( angle );
            float yb = ya + ldraw * sinf( angle );

            if( ( lrun + ldraw ) >= lpix )         // last segment is partial draw
            {
                xb = x2;
                yb = y2;
            }

            glVertex2f( xa + t2sina1, ya - t2cosa1 );
            glVertex2f( xb + t2sina1, yb - t2cosa1 );
            glVertex2f( xb - t2sina1, yb + t2cosa1 );

            glVertex2f( xb - t2sina1, yb + t2cosa1 );
            glVertex2f( xa - t2sina1, ya + t2cosa1 );
            glVertex2f( xa + t2sina1, ya - t2cosa1 );

            xa = xb;
            ya = yb;
            lrun += ldraw;

            //    Space
            xb = xa + lspace * cos( angle );
            yb = ya + lspace * sin( angle );

            xa = xb;
            ya = yb;
            lrun += lspace;
        }
    } else {
        glVertex2f( x1 + t2sina1, y1 - t2cosa1 );
        glVertex2f( x2 + t2sina1, y2 - t2cosa1 );
        glVertex2f( x2 - t2sina1, y2 + t2cosa1 );

        glVertex2f( x2 - t2sina1, y2 + t2cosa1 );
        glVertex2f( x1 - t2sina1, y1 + t2cosa1 );
        glVertex2f( x1 + t2sina1, y1 - t2cosa1 );

        /* wx draws a nice rounded end in dc mode, so replicate
           this for opengl mode, should this be done for the dashed mode case? */
        if(pen.GetCap() == wxCAP_ROUND) {
            DrawEndCap( x1, y1, t1, angle);
            DrawEndCap( x2, y2, t1, angle + M_PI);
        }

    }

    glEnd();
#endif    
}

void ODDC::DrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, bool b_hiqual )
{
    if( dc )
        dc->DrawLine( x1, y1, x2, y2 );
#ifdef ocpnUSE_GL
    else if( ConfigurePen() ) {
        bool b_draw_thick = false;

        float pen_width = wxMax(g_GLMinSymbolLineWidth, m_pen.GetWidth());

        //      Enable anti-aliased lines, at best quality
        if( b_hiqual ) {

#ifndef __WXQT__
            glEnable( GL_BLEND );
            glEnable( GL_LINE_SMOOTH );
#endif            
            
            if( pen_width > 1.0 ) {
                GLint parms[2];
                glGetIntegerv( GL_SMOOTH_LINE_WIDTH_RANGE, &parms[0] );
                if( pen_width > parms[1] ) b_draw_thick = true;
                else
                    glLineWidth( pen_width );
            } else
                glLineWidth( pen_width );
        } else {            
            if( pen_width > 1 ) {
                GLint parms[2];
                glGetIntegerv( GL_ALIASED_LINE_WIDTH_RANGE, &parms[0] );
                if( pen_width > parms[1] ) b_draw_thick = true;
                    else
                        glLineWidth( pen_width );
            } else
                glLineWidth( pen_width );
        }
        
        if( b_draw_thick ) DrawGLThickLine( x1, y1, x2, y2, m_pen, b_hiqual );
        else {
            wxDash *dashes;
            int n_dashes = m_pen.GetDashes( &dashes );
            if( n_dashes ) {
                float angle = atan2f( (float) ( y2 - y1 ), (float) ( x2 - x1 ) );
                float cosa = cosf( angle );
                float sina = sinf( angle );
                float t1 = m_pen.GetWidth();
                    
                float lpix = sqrtf( powf(x1 - x2, 2) + powf(y1 - y2, 2) );
                float lrun = 0.;
                float xa = x1;
                float ya = y1;
                float ldraw = t1 * dashes[0];
                float lspace = t1 * dashes[1];
                    
                glBegin( GL_LINES );
                while( lrun < lpix ) {
                    //    Dash
                    float xb = xa + ldraw * cosa;
                    float yb = ya + ldraw * sina;

                    if( ( lrun + ldraw ) >= lpix )         // last segment is partial draw
                    {
                        xb = x2;
                        yb = y2;
                        }

                        glVertex2f( xa, ya );
                        glVertex2f( xb, yb );

                        xa = xa + ( lspace + ldraw ) * cosa;
                        ya = ya + ( lspace + ldraw ) * sina;
                        lrun += lspace + ldraw;

                    }
                glEnd();
            } else                    // not dashed
            {
                glBegin( GL_LINES );
                glVertex2i( x1, y1 );
                glVertex2i( x2, y2 );
                glEnd();
            }
        }

        glDisable( GL_LINE_STIPPLE );

        if( b_hiqual ) {
            glDisable( GL_LINE_SMOOTH );
            glDisable( GL_BLEND );
        }
    }
#endif    
}

void ODDC::DrawLines( int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, bool b_hiqual )
{
    if( dc )
        dc->DrawLines( n, points, xoffset, yoffset );
#ifdef ocpnUSE_GL
    else if( ConfigurePen() ) {

        SetGLAttrs( b_hiqual );
        bool b_draw_thick = false;

        glDisable( GL_LINE_STIPPLE );
        
        //      Enable anti-aliased lines, at best quality
        if( b_hiqual ) {
            glEnable( GL_BLEND );
            if( m_pen.GetWidth() > 1 ) {
                GLint parms[2];
                glGetIntegerv( GL_SMOOTH_LINE_WIDTH_RANGE, &parms[0] );
                if( m_pen.GetWidth() > parms[1] ) b_draw_thick = true;
                else
                    glLineWidth( wxMax(g_GLMinSymbolLineWidth, m_pen.GetWidth()) );
            } else
                glLineWidth( wxMax(g_GLMinSymbolLineWidth, 1) );
        } else {
            if( m_pen.GetWidth() > 1 ) {
                GLint parms[2];
                glGetIntegerv( GL_ALIASED_LINE_WIDTH_RANGE, &parms[0] );
                if( m_pen.GetWidth() > parms[1] ) b_draw_thick = true;
                else
                    glLineWidth( wxMax(g_GLMinSymbolLineWidth, m_pen.GetWidth()) );
            } else
                glLineWidth( wxMax(g_GLMinSymbolLineWidth, 1) );
        }

        if( b_draw_thick) {
            //DrawGLThickLines( n, points, xoffset, yoffset, m_pen, b_hiqual );
        } else {
            if( b_hiqual ) {
                glEnable( GL_LINE_SMOOTH );
            }
            glBegin( GL_LINE_STRIP );
            for( int i = 0; i < n; i++ ) {
                glVertex2i( points[i].x + xoffset, points[i].y + yoffset );
            }
            glEnd();
        }

        glDisable( GL_LINE_STIPPLE );
        SetGLAttrs( false );
    }
#endif    
}

void ODDC::DrawArc( wxCoord xc, wxCoord yc, wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, bool b_hiqual )
{
    if( dc )
        dc->DrawArc( x1, y1, x2, y2, xc, yc );
    #ifdef ocpnUSE_GL
    else if( ConfigurePen() ) {
        bool b_draw_thick = false;
        
        float pen_width = wxMax(g_GLMinSymbolLineWidth, m_pen.GetWidth());
        
        //      Enable anti-aliased lines, at best quality
        if( b_hiqual ) {
            
            #ifndef __WXQT__
            glEnable( GL_BLEND );
            glEnable( GL_LINE_SMOOTH );
            #endif            
            
            if( pen_width > 1.0 ) {
                GLint parms[2];
                glGetIntegerv( GL_SMOOTH_LINE_WIDTH_RANGE, &parms[0] );
                if( pen_width > parms[1] ) b_draw_thick = true;
                else
                    glLineWidth( pen_width );
            } else
                glLineWidth( pen_width );
        } else {            
            if( pen_width > 1 ) {
                GLint parms[2];
                glGetIntegerv( GL_ALIASED_LINE_WIDTH_RANGE, &parms[0] );
                if( pen_width > parms[1] ) b_draw_thick = true;
                else
                    glLineWidth( pen_width );
            } else
                glLineWidth( pen_width );
        }
        
        if( b_draw_thick ) DrawGLThickLine( x1, y1, x2, y2, m_pen, b_hiqual );
        else {
            wxDash *dashes;
            int n_dashes = m_pen.GetDashes( &dashes );
            if( n_dashes ) {
                float angle = atan2f( (float) ( y2 - y1 ), (float) ( x2 - x1 ) );
                float cosa = cosf( angle );
                float sina = sinf( angle );
                float t1 = m_pen.GetWidth();
                
                float lpix = sqrtf( powf(x1 - x2, 2) + powf(y1 - y2, 2) );
                float lrun = 0.;
                float xa = x1;
                float ya = y1;
                float ldraw = t1 * dashes[0];
                float lspace = t1 * dashes[1];
                
                glBegin( GL_LINES );
                while( lrun < lpix ) {
                    //    Dash
                    float xb = xa + ldraw * cosa;
                    float yb = ya + ldraw * sina;
                    
                    if( ( lrun + ldraw ) >= lpix )         // last segment is partial draw
                    {
                        xb = x2;
                        yb = y2;
                    }
                    
                    glVertex2f( xa, ya );
                    glVertex2f( xb, yb );
                    
                    xa = xa + ( lspace + ldraw ) * cosa;
                    ya = ya + ( lspace + ldraw ) * sina;
                    lrun += lspace + ldraw;
                    
                }
                glEnd();
            } else                    // not dashed
            {
                glBegin( GL_LINES );
                glVertex2i( x1, y1 );
                glVertex2i( x2, y2 );
                glEnd();
            }
        }
        
        glDisable( GL_LINE_STIPPLE );
        
        if( b_hiqual ) {
            glDisable( GL_LINE_SMOOTH );
            glDisable( GL_BLEND );
        }
    }
    #endif    
}

void ODDC::StrokeLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
#if wxUSE_GRAPHICS_CONTEXT
    if( pgc ) {
        pgc->SetPen( dc->GetPen() );
        pgc->StrokeLine( x1, y1, x2, y2 );

        dc->CalcBoundingBox( x1, y1 );
        dc->CalcBoundingBox( x2, y2 );
    } else
#endif
        DrawLine( x1, y1, x2, y2, true );
}

void ODDC::StrokeLines( int n, wxPoint *points) {
    if(n < 2) /* optimization and also to avoid assertion in pgc->StrokeLines */
        return;

#if wxUSE_GRAPHICS_CONTEXT
    if( pgc ) {
        wxPoint2DDouble* dPoints = (wxPoint2DDouble*) malloc( n * sizeof( wxPoint2DDouble ) );
        for( int i=0; i<n; i++ ) {
            dPoints[i].m_x = points[i].x;
            dPoints[i].m_y = points[i].y;
        }
        pgc->SetPen( dc->GetPen() );
        pgc->StrokeLines( n, dPoints );
        free( dPoints );
    } else
#endif
        DrawLines( n, points, 0, 0, true );
}

void ODDC::StrokeArc( wxCoord xc, wxCoord yc, wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
    #if wxUSE_GRAPHICS_CONTEXT
    if( pgc ) {
        pgc->SetPen( dc->GetPen() );
        pgc->SetBrush(dc->GetBrush());
        
        wxGraphicsPath gpath = pgc->CreatePath();
        wxDouble  l_dFirstAngle = atan2((y1 - yc), (x1 -xc));
        wxDouble  l_dSecondAngle = atan2((y2-yc), (x2-xc));
        wxDouble  l_radius = sqrt(pow((y2-yc), 2.0) + pow((x2-xc), 2.0));
        gpath.MoveToPoint( x1, y1 );
        gpath.AddArc( xc, yc, l_radius, l_dFirstAngle, l_dSecondAngle, true);
        pgc->DrawPath( gpath );

        dc->CalcBoundingBox( x1, y1 );
        dc->CalcBoundingBox( x2, y2 );
    } else
        #endif
        DrawArc( xc, yc, x1, y1, x2, y2, true );
}

void ODDC::StrokeSector( wxCoord xc, wxCoord yc, wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord x3, wxCoord y3, wxCoord x4, wxCoord y4  )
{
#if wxUSE_GRAPHICS_CONTEXT
    if( pgc ) {
        pgc->SetPen( dc->GetPen() );
        pgc->SetBrush(dc->GetBrush());
        
        double y1yc, x1xc, y3yc, x3xc;
        y1yc = y1-yc;
        x1xc = x1-xc;
        y3yc = y3-yc;
        x3xc = x3-xc;
        wxGraphicsPath gpath = pgc->CreatePath();
        wxDouble  l_dFirstAngle;
        l_dFirstAngle = atan2(y1yc, x1xc);
        
        wxDouble  l_dSecondAngle;
        l_dSecondAngle = atan2(y3yc, x3xc);
        
        wxDouble  l_radius = sqrt(pow((y2-yc), 2.0) + pow((x2-xc), 2.0));
        gpath.MoveToPoint( x1, y1 );
        gpath.AddLineToPoint( x2, y2 );
        gpath.AddArc( xc, yc, l_radius, l_dFirstAngle, l_dSecondAngle, true );
        gpath.MoveToPoint( x3, y3 );
        gpath.AddLineToPoint( x4, y4 );
        l_radius = sqrt(pow((y1-yc), 2.0) + pow((x1-xc), 2.0));
        gpath.AddArc( xc, yc, l_radius, l_dSecondAngle, l_dFirstAngle, false);
        pgc->StrokePath( gpath );
        pgc->FillPath( gpath );
        
        dc->CalcBoundingBox( x1, y1 );
        dc->CalcBoundingBox( x3, y3 );
    } 
#endif
        
}

void ODDC::DrawRectangle( wxCoord x, wxCoord y, wxCoord w, wxCoord h )
{
    if( dc )
        dc->DrawRectangle( x, y, w, h );
#ifdef ocpnUSE_GL
    else {
        if( ConfigureBrush() ) {
            glBegin( GL_QUADS );
            glVertex2i( x, y );
            glVertex2i( x + w, y );
            glVertex2i( x + w, y + h );
            glVertex2i( x, y + h );
            glEnd();
        }

        if( ConfigurePen() ) {
            glBegin( GL_LINE_LOOP );
            glVertex2i( x, y );
            glVertex2i( x + w, y );
            glVertex2i( x + w, y + h );
            glVertex2i( x, y + h );
            glEnd();
        }
    }
#endif    
}

/* draw the arc along corners */
static void drawrrhelper( wxCoord x0, wxCoord y0, wxCoord r, int quadrant, int steps )
{
#ifdef ocpnUSE_GL
    float step = 1.0/steps, rs = 2.0*r*step, rss = rs*step, x, y, dx, dy, ddx, ddy;
    switch(quadrant) {
    case 0: x =  r, y =  0, dx =   0, dy = -rs, ddx = -rss, ddy =  rss; break;
    case 1: x =  0, y = -r, dx = -rs, dy =   0, ddx =  rss, ddy =  rss; break;
    case 2: x = -r, y =  0, dx =   0, dy =  rs, ddx =  rss, ddy = -rss; break;
    case 3: x =  0, y =  r, dx =  rs, dy =   0, ddx = -rss, ddy = -rss; break;
    default: return; // avoid unitialized compiler warnings
    }

    for(int i=0; i<steps; i++) {
        glVertex2i( x0 + floor(x), y0 + floor(y) );
         x += dx+ddx/2,  y += dy+ddy/2;
        dx += ddx,      dy += ddy;
    }
    glVertex2i( x0 + floor(x), y0 + floor(y) );
#endif
}

void ODDC::DrawRoundedRectangle( wxCoord x, wxCoord y, wxCoord w, wxCoord h, wxCoord r )
{
    if( dc )
        dc->DrawRoundedRectangle( x, y, w, h, r );
#ifdef ocpnUSE_GL
    else {
        r++;
        int steps = ceil(sqrt((float)r));

        wxCoord x1 = x + r, x2 = x + w - r;
        wxCoord y1 = y + r, y2 = y + h - r;
        if( ConfigureBrush() ) {
            glBegin( GL_TRIANGLE_FAN );
            drawrrhelper( x2, y1, r, 0, steps );
            drawrrhelper( x1, y1, r, 1, steps );
            drawrrhelper( x1, y2, r, 2, steps );
            drawrrhelper( x2, y2, r, 3, steps );
            glEnd();
        }

        if( ConfigurePen() ) {
            glBegin( GL_LINE_LOOP );
            drawrrhelper( x2, y1, r, 0, steps );
            drawrrhelper( x1, y1, r, 1, steps );
            drawrrhelper( x1, y2, r, 2, steps );
            drawrrhelper( x2, y2, r, 3, steps );
            glEnd();
        }
    }
#endif    
}

void ODDC::DrawCircle( wxCoord x, wxCoord y, wxCoord radius )
{
    DrawEllipse( x - radius, y - radius, 2 * radius, 2 * radius );
}


void ODDC::StrokeCircle( wxCoord x, wxCoord y, wxCoord radius )
{
#if wxUSE_GRAPHICS_CONTEXT
    if( pgc ) {
        wxGraphicsPath gpath = pgc->CreatePath();
        gpath.AddCircle( x, y, radius );

        pgc->SetPen( GetPen() );
        pgc->SetBrush( GetBrush() );
        pgc->DrawPath( gpath );

        // keep dc dirty box up-to-date
        dc->CalcBoundingBox( x + radius + 2, y + radius + 2 );
        dc->CalcBoundingBox( x - radius - 2, y - radius - 2 );
    } else
#endif
        DrawCircle( x, y, radius );
}

void ODDC::DrawEllipse( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    if( dc )
        dc->DrawEllipse( x, y, width, height );
#ifdef ocpnUSE_GL
    else {
        float r1 = width / 2, r2 = height / 2;
        float cx = x + r1, cy = y + r2;

        //      Enable anti-aliased lines, at best quality
        glEnable( GL_BLEND );

        /* formula for variable step count to produce smooth ellipse */
        float steps = floorf(wxMax(sqrtf(sqrtf((float)(width*width + height*height))), 1) * M_PI);

        if( ConfigureBrush() ) {
            glBegin( GL_TRIANGLE_FAN );
            glVertex2f( cx, cy );
            for( float a = 0; a <= 2 * M_PI + M_PI/steps; a += 2 * M_PI / steps )
                glVertex2f( cx + r1 * sinf( a ), cy + r2 * cosf( a ) );
            glEnd();
        }

        if( ConfigurePen() ) {
            glBegin( GL_LINE_STRIP );
            float a;
            for( a = 0; a < 2 * M_PI - M_PI/steps; a += 2 * M_PI / steps )
                glVertex2f( cx + r1 * sinf( a ), cy + r2 * cosf( a ) );
            glVertex2f( cx + r1 * sinf( a ), cy + r2 * cosf( a ) ); // added to ensure line is closed and no holes are shown when not using solid line
            glEnd();
        }

        glDisable( GL_BLEND );
    }
#endif    
}



void ODDC::DrawBitmap( const wxBitmap &bitmap, wxCoord x, wxCoord y, bool usemask )
{
#ifdef ocpnUSE_GLES  // Do not attempt to do anything with glDrawPixels if using opengles
        return;
#endif

    wxBitmap bmp;
    if( x < 0 || y < 0 ) {
        int dx = ( x < 0 ? -x : 0 );
        int dy = ( y < 0 ? -y : 0 );
        int w = bitmap.GetWidth() - dx;
        int h = bitmap.GetHeight() - dy;
        /* picture is out of viewport */
        if( w <= 0 || h <= 0 ) return;
        wxBitmap newBitmap = bitmap.GetSubBitmap( wxRect( dx, dy, w, h ) );
        x += dx;
        y += dy;
        bmp = newBitmap;
    } else {
        bmp = bitmap;
    }
    if( dc )
        dc->DrawBitmap( bmp, x, y, usemask );
#ifdef ocpnUSE_GL
    else {
        wxImage image = bmp.ConvertToImage();
        int w = image.GetWidth(), h = image.GetHeight();

        if( usemask ) {
            unsigned char *d = image.GetData();
            unsigned char *a = image.GetAlpha();

            unsigned char mr, mg, mb;
            if( !a && !image.GetOrFindMaskColour( &mr, &mg, &mb ) ) printf(
                    "trying to use mask to draw a bitmap without alpha or mask\n" );

            unsigned char *e = new unsigned char[4 * w * h];
            if(e && d){
                for( int y = 0; y < h; y++ )
                    for( int x = 0; x < w; x++ ) {
                        unsigned char r, g, b;
                        int off = ( y * w + x );
                        r = d[off * 3 + 0];
                        g = d[off * 3 + 1];
                        b = d[off * 3 + 2];

                        e[off * 4 + 0] = r;
                        e[off * 4 + 1] = g;
                        e[off * 4 + 2] = b;

                        e[off * 4 + 3] =
                                a ? a[off] : ( ( r == mr ) && ( g == mg ) && ( b == mb ) ? 0 : 255 );
                    }
            }

            glColor4f( 1, 1, 1, 1 );
            GLDrawBlendData( x, y, w, h, GL_RGBA, e );
            delete[] ( e );
        } else {
            glRasterPos2i( x, y );
            glPixelZoom( 1, -1 ); /* draw data from top to bottom */
            if(image.GetData())
                glDrawPixels( w, h, GL_RGB, GL_UNSIGNED_BYTE, image.GetData() );
            glPixelZoom( 1, 1 );
        }
    }
#endif    
}



void ODDC::ResetBoundingBox()
{
    if( dc ) dc->ResetBoundingBox();
}

void ODDC::CalcBoundingBox( wxCoord x, wxCoord y )
{
    if( dc ) dc->CalcBoundingBox( x, y );
}

bool ODDC::ConfigurePen()
{
    wxColour c = wxNullColour;
    int width = 0;

    if( !m_pen.IsOk() ) return false;
    if( m_pen == *wxTRANSPARENT_PEN ) 
        width = 0;
    else {
        c = m_pen.GetColour();
        width = m_pen.GetWidth();
    }
#ifdef ocpnUSE_GL
    if(c != wxNullColour)
        glColor4ub( c.Red(), c.Green(), c.Blue(), c.Alpha() );
    glLineWidth(wxMax(g_GLMinSymbolLineWidth, width) );
#endif    
    return true;
}

bool ODDC::ConfigureBrush()
{
    if( m_brush == wxNullBrush || m_brush.GetStyle() == wxBRUSHSTYLE_TRANSPARENT )
        return false;
#ifdef ocpnUSE_GL
    wxColour c = m_brush.GetColour();
    glColor4ub( c.Red(), c.Green(), c.Blue(), c.Alpha() );
#endif    
    return true;
}

void ODDC::GLDrawBlendData( wxCoord x, wxCoord y, wxCoord w, wxCoord h, int format,
        const unsigned char *data )
{
#ifdef ocpnUSE_GL
    glEnable( GL_BLEND );
    glRasterPos2i( x, y );
    glPixelZoom( 1, -1 );
    glDrawPixels( w, h, format, GL_UNSIGNED_BYTE, data );
    glPixelZoom( 1, 1 );
    glDisable( GL_BLEND );
#endif
}

