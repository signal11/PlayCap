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


#ifndef ABOUTWINDOW_H__
#define ABOUTWINDOW_H__

#include <fx.h>


class AboutWindow : public FXDialogBox
{
	FXDECLARE(AboutWindow)
	
public:
	enum {
		ID_FIRST = FXDialogBox::ID_LAST,
		ID_OK_BUTTON,
		ID_LAST,
	};

private:
	FXIcon *ico;
protected:
	AboutWindow() {};
public:
	AboutWindow(FXWindow *parent);
	~AboutWindow();
	
	long onOk(FXObject *sender, FXSelector sel, void *ptr);
};

#endif // ABOUTWINDOW_H__
