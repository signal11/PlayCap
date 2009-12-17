/**************************************************************************
    About Window
    Copyright 2009 Alan Ott, Signal 11 Software
    2007
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

***************************************************************************/


#include "AboutWindow.h"

#include <iostream>
using std::cout;
using std::endl;

#include "FXPNGIcon.h"

#include "signal11-logo.h"

FXDEFMAP(AboutWindow) AboutWindowMap[] = {
	FXMAPFUNC( SEL_COMMAND, AboutWindow::ID_OK_BUTTON, AboutWindow::onOk)
};

FXIMPLEMENT(AboutWindow, FXDialogBox, AboutWindowMap, ARRAYNUMBER(AboutWindowMap))



AboutWindow::AboutWindow(FXWindow *parent)
	: FXDialogBox(parent, "About", DECOR_CLOSE|DECOR_TITLE|DECOR_BORDER)
{

	FXVerticalFrame *vf = new FXVerticalFrame(this);
	
	ico = new FXPNGIcon(getApp(), signal11_logo);
	ico->blend(vf->getBackColor());
	
	new FXLabel(vf, "", ico);
	
	new FXSeparator(vf);
	new FXWindow(vf);

	const char *text = 
		"This is the libpcap playback tool, version 0.9\n"
		"Copyright 2006-2009 Alan Ott, Signal 11 Software, LLC\n"
		"Licensed under the GNU General Public License, version 3\n"
		"www.signal11.us\n"
		"407-222-6975"
		"\n";
	const char *text3 = 
		"Uses Fox-Toolkit (www.fox-toolkit.org)\n"
		"Uses WinPCAP (www.winpcap.org)\n"
		"Uses PThreads-Win32 (sourceware.org/pthreads-win32/)\n"
		"Uses libpng (www.libpng.org)\n"
		"Uses zlib (www.zlib.net)\n"
		"Uses Tango Icons (tango.freedesktop.org)\n"
		"Uses FamFamFam Icons (www.famfamfam.com)";


	new FXLabel(vf, text, NULL, JUSTIFY_CENTER_X|LABEL_NORMAL|LAYOUT_CENTER_X);
	new FXLabel(vf, text3, NULL, JUSTIFY_LEFT|LABEL_NORMAL|LAYOUT_CENTER_X);
	
	new FXHorizontalFrame(vf,0, 0,0,0,20);
	new FXButton(vf, "Close", NULL, this, ID_ACCEPT, BUTTON_NORMAL|FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|LAYOUT_CENTER_Y, 0,0,200,200, 20,20,5,5);
}

long
AboutWindow::onOk(FXObject*, FXSelector, void*)
{
	return 1;
}

AboutWindow::~AboutWindow()
{
	delete ico;
}


