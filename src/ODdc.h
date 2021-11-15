/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Layer to use wxDC or opengl
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
 *f
 */


#ifndef __ODDC_H__
#define __ODDC_H__

#include "TexFont.h"
#include <vector>
#include "ocpn_plugin.h"
#include "../src/oplaydo2_pi.h"

#ifdef ocpnUSE_GL
#include <wx/glcanvas.h>
#endif

#ifdef USE_ANDROID_GLES2
    #include "linmath.h"
#endif    

class ViewPort;
class GLUtesselator;



//----------------------------------------------------------------------------
// ODDC
//----------------------------------------------------------------------------

class wxGLCanvas;

class ODDC
{
public:
    ODDC(wxGLCanvas &canvas);
    ODDC(wxDC &pdc);
    ODDC();

    ~ODDC();

    void Init();

    void SetVP(PlugIn_ViewPort *vp);
    void SetBackground( const wxBrush &brush );
    void SetPen( const wxPen &pen);
    void SetBrush( const wxBrush &brush);
    void SetTextForeground(const wxColour &colour);
    void SetFont(const wxFont& font);
    static void SetGLAttrs( bool highQuality );
    void SetGLStipple() const;

    const wxPen& GetPen() const;
    const wxBrush& GetBrush() const;
    const wxFont& GetFont() const;

    void GetSize(wxCoord *width, wxCoord *height) const;

    void DrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, bool b_hiqual = true);
    void DrawLines( int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0, bool b_hiqual = true);

    void StrokeLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);
    void StrokeLine( wxPoint a, wxPoint b) { StrokeLine(a.x, a.y, b.x, b.y); }
    void StrokeLines( int n, wxPoint *points);

    void DrawArc( wxCoord xc, wxCoord yc, wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, bool b_hiqual = true);
    void StrokeArc( wxCoord xc, wxCoord yc, wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);
    void StrokeSector( wxCoord xc, wxCoord yc, wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord x3, wxCoord y3, wxCoord x4, wxCoord y4 );

    void Clear();
    void DrawRectangle( wxCoord x, wxCoord y, wxCoord w, wxCoord h );
    void DrawRoundedRectangle( wxCoord x, wxCoord y, wxCoord w, wxCoord h, wxCoord rr );
    void DrawCircle(wxCoord x, wxCoord y, wxCoord radius);
    void DrawCircle(const wxPoint &pt, wxCoord radius) { DrawCircle(pt.x, pt.y, radius); }
    void StrokeCircle(wxCoord x, wxCoord y, wxCoord radius);

    void DrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    void StrokePolygon(int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0, float scale = 1.0);

    void DrawPolygonPattern( int n, wxPoint points[], int textureID, wxSize textureSize, wxCoord xoffset=0, wxCoord yoffset=0, float scale=1.0, float angle=0.0 );
    void DrawPolygonsPattern( int n, int npoint[], wxPoint points[], int textureID, wxSize textureSize, wxCoord xoffset=0, wxCoord yoffset=0, float scale=1.0, float angle=0.0 );

    // TODO move to Protected
    void DrawPolygonTessellated(int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0);

    void DrawBitmap(const wxBitmap &bitmap, wxCoord x, wxCoord y, bool usemask);

    void DrawTexture( wxRect texRect, int width, int height, float scaleFactor, wxPoint position, float rotation, wxPoint rPivot);
    void DrawTextureAlpha( wxRect texRect, int width, int height, float scaleFactor, wxPoint position, float rotation, wxPoint rPivot);

    void ResetBoundingBox();
    void CalcBoundingBox(wxCoord x, wxCoord y);

    void DestroyClippingRegion() {}

    wxDC *GetDC() const { return dc; }

    void SetTextureParms( GLint textureId, int width, int height );
	// Needed for ocpndc.cpp to compile. Normally would be in glChartCanvas.cpp



#ifdef ocpnUSE_GL     
    GLfloat     *s_odc_tess_work_buf;
    GLfloat     *s_odc_tess_tex_buf;
#endif

#ifdef USE_ANDROID_GLES2
    int          s_odc_tess_vertex_idx;
    int          s_odc_tess_vertex_idx_this;
    int          s_odc_tess_buf_len;
    GLenum       s_odc_tess_mode;
    int          s_odc_nvertex;
    vec4         s_odc_tess_color;
    ViewPort    *s_odc_tessVP;
    GLint        s_odc_activeProgram;

#endif
    GLUtesselator *m_tobj;


protected:
    void DrawPolygonTessellatedPattern( int n, wxPoint points[], int textureID, wxSize textureSize, wxCoord xoffset=0, wxCoord yoffset=0 );

    bool ConfigurePen();
    bool ConfigureBrush();

    void GLDrawBlendData(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                        int format, const unsigned char *data);
    void drawrrhelperGLES2( wxCoord x0, wxCoord y0, wxCoord r, int quadrant, int steps );

    void DrawGLThickLine( float x1, float y1, float x2, float y2, wxPen pen, bool b_hiqual );
    void DrawGLThickLines(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, wxPen pen, bool b_hiqual);
    void DrawEndCap(float x1, float y1, float t1, float angle);

    wxGLCanvas *glcanvas;
    wxDC *dc;
    wxPen m_pen;
    wxBrush m_brush;
    wxColour m_textforegroundcolour;
    wxFont m_font;

#ifdef ocpnUSE_GL     
    TexFont m_texfont;
#endif

    bool m_buseTex;

#if  wxUSE_GRAPHICS_CONTEXT == 1
    wxGraphicsContext *pgc;
#endif

    float *workBuf;
    size_t workBufSize;
    unsigned int workBufIndex;

    wxSize m_vpSize;

};

#endif
