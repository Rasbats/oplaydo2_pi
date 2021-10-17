/* Autogenerated by png2wx.pl on Tue Feb 21 15:00:34 2017 */
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#	include <wx/wx.h>
#endif
#include <wx/filename.h>
#include <wx/mstream.h>
#include "icons.h"

wxBitmap *_img_oplaydo1;
wxBitmap *_img_oplaydo1_pi;

#ifdef PLUGIN_USE_SVG
#include "ocpn_plugin.h"
wxString _svg_oplaydo1;
wxString _svg_oplaydo1_toggled;
#endif

void initialize_images(void)
{
	{
		wxMemoryInputStream sm("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000 \000\000\000 \004\003\000\000\000\201Tg\307\000\000\000'PLTE\000\001\000\000\034\000\004=\000\000r\000\000\215\000\000\330\000\233\253\276\242\262\305\252\261\307\014\377\000\265\277\324\270\307\333\327\337\3657\020\324\260\000\000\000\011pHYs\000\000\013\023\000\000\013\023\001\000\232\234\030\000\000\000\atIME\a\341\002\025\0169\003\221\330g@\000\000\000_IDAT(\317c8\263\243\003\011t\237a8\276r&\022\230U\303\260k&\nX\315\320\211*0\203\241\022U`:\021\002\231\020\306\024\006N0=\r&`\200&\020\304\200&\300\200! @H\305\314\011\303Y\000\006\260\b`\204:FT\236@\215\354\036\2063\333!\011\241\002LV\237\001\000r\177\021\344t,\207\332\000\000\000\000IEND\256B`\202", 243);
		_img_oplaydo1 = new wxBitmap(wxImage(sm));
	}
	{
		wxMemoryInputStream sm("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000 \000\000\000 \004\003\000\000\000\201Tg\307\000\000\000*PLTE\001\004\000\002\005\001\000\011\002\003\013\000\000\017\000\001\022\003\000H\000\000|\000\000\216\000\000\347\000\000\373\004\000\375\000\014\377\0000\373=\212\212\376\270\000\000\000\011pHYs\000\000\013\023\000\000\013\023\001\000\232\234\030\000\000\000\atIME\a\341\002\025\0169)Jc\256\226\000\000\000JIDAT(\317c\270\213\n\3560\234A\0034\025(`D\025\230\306\300\200\"p\"\000M\340\000\0033\272\000\013\272\300\021\0064[0\004\016\343\021X\203\020\330\203\244b\023\272\226C\350\002\a\321\005\016PA\200>\321\200[\000\000\222\016\204\027\3205\031\316\000\000\000\000IEND\256B`\202", 225);
		_img_oplaydo1_pi = new wxBitmap(wxImage(sm));
	}
	
#ifdef PLUGIN_USE_SVG
    wxFileName fn;
    wxString tmp_path;

    tmp_path = GetPluginDataDir("oplaydo1_pi");
    fn.SetPath(tmp_path);
    fn.AppendDir(_T("data"));

    fn.SetFullName(_T("oplaydo1_pi.svg"));
    _svg_oplaydo1 = fn.GetFullPath();
    fn.SetFullName(_T("oplaydo1_pi_toggled.svg"));
    _svg_oplaydo1_toggled = fn.GetFullPath();

#endif // PLUGIN_USE_SVG

	
	
	return;
}
