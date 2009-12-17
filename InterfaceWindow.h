/**************************************************************************
    Window for Selecting a Network Interface
    Copyright 2009 Alan Ott, Signal 11 Software
    10-18-2006
    
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

#ifndef INTERFACE_WINDOW_H__
#define INTERFACE_WINDOW_H__

#include <fx.h>

#include <pcap.h>


class InterfaceWindow : public FXVerticalFrame
{
	FXDECLARE(InterfaceWindow)

public:
	enum {
		ID_FIRST = FXVerticalFrame::ID_LAST,
		ID_TIME,
		ID_CAPTURE,
		ID_LAST	
	};
	
private:
	pcap_if_t *interfaces;
	pcap_if_t *selected_interface;

	FXTimer *timer;

protected:
	InterfaceWindow() { };


public:

	InterfaceWindow(FXComposite *parent, pcap_if_t *interfaces, FXString title, FXString buttonCaption);
	~InterfaceWindow();
	pcap_if_t *getSelectedInterface() { return selected_interface; };
	
	
	
	long onTimeout(FXObject *, FXSelector, void *);
	long onCapture(FXObject *, FXSelector, void *);

};


#endif // INTERFACE_WINDOW_H__
