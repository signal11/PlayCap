/********************************
 Interface Window for IG Scanner
 Alan Ott
 10-18-2006
********************************/



#include "InterfaceWindow.h"

#include <FXPNGIcon.h>

#ifdef WIN32
	#include <winsock2.h>
#else
	#include <netinet/in.h>  // sockaddr_in
	#include <arpa/inet.h>   // inet_ntoa()
#endif

#include <iostream>
using std::cout;
using std::endl;

#include "tango/network-transmit-receive.png.h"
#include "tango/network-transmit-receive-48.png.h"


FXDEFMAP(InterfaceWindow) InterfaceWindowMap[] = {
	FXMAPFUNC(SEL_TIMEOUT, InterfaceWindow::ID_TIME, InterfaceWindow::onTimeout),
	FXMAPFUNC(SEL_COMMAND, InterfaceWindow::ID_CAPTURE, InterfaceWindow::onCapture),
};

FXIMPLEMENT(InterfaceWindow, FXVerticalFrame, InterfaceWindowMap, ARRAYNUMBER(InterfaceWindowMap));


InterfaceWindow::InterfaceWindow(FXComposite *parent, pcap_if_t *ifaces, FXString title, FXString buttonCaption)
	: FXVerticalFrame(parent),
	  interfaces(ifaces)
{
	selected_interface = NULL;
	
	FXPNGIcon *capture_icon = new FXPNGIcon(getApp(), networktransmitreceive);
	FXPNGIcon *capture_icon_48 = new FXPNGIcon(getApp(), networktransmitreceive48);

	FXLabel *label = new FXLabel(this, title, capture_icon_48);
	label->setFont(new FXFont(getApp(), "Arial", 16));


	new FXWindow(new FXVerticalFrame(this));
		
	
	// Create the GUI.
#ifndef WIN32
	//FXMatrix *matrix = new FXMatrix(this, 2, MATRIX_BY_COLUMNS);
#else
	FXMatrix *matrix = new FXMatrix(this,2, MATRIX_BY_COLUMNS);
#endif
	
	pcap_if_t *itr = interfaces;
	while (itr) {

		// Find the IP addr of this device (if any).
		FXString ip_addr;
		pcap_addr *addr = itr->addresses;
		while (addr) {
			if (addr->addr->sa_family == AF_INET) {
				sockaddr_in *inaddr = (sockaddr_in*) addr->addr;
				ip_addr = inet_ntoa(inaddr->sin_addr);
				break;
			}
			addr = addr->next;
		}


#ifndef WIN32
		// Unix Platforms
		FXHorizontalFrame *outerHF = new FXHorizontalFrame(this, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 2*DEFAULT_SPACING, 0);
		FXVerticalFrame *vf = new FXVerticalFrame(outerHF, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,DEFAULT_SPACING);
		FXHorizontalFrame *hf = new FXHorizontalFrame(vf, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0);
		FXLabel *l = new FXLabel(hf, itr->name);
		l->setFont(new FXFont(getApp(), "Helvetica", 14, FXFont::Bold));
		new FXLabel(hf, ip_addr, NULL, LABEL_NORMAL|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
		new FXLabel(vf, (itr->description)? itr->description: "");
#else
		// Windows
		FXHorizontalFrame *outerHF = new FXHorizontalFrame(this, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 2*DEFAULT_SPACING, 0);
		FXVerticalFrame *vf = new FXVerticalFrame(outerHF, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,DEFAULT_SPACING);
		//FXHorizontalFrame *hf = new FXHorizontalFrame(vf, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0);
		//FXLabel *l = new FXLabel(hf, itr->name);
		//l->setFont(new FXFont(getApp(), "Helvetica", 14, FXFont::Bold));
		new FXLabel(vf, (itr->description)? itr->description: "");
		new FXLabel(vf, ip_addr, NULL, LABEL_NORMAL|LAYOUT_CENTER_Y);
			
		
		//new FXLabel(matrix, (itr->description)? itr->description: "");
		//new FXLabel(matrix, ip_addr, NULL, LABEL_NORMAL|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
		//FXHorizontalFrame *outerHF = new FXHorizontalFrame(this, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 2*DEFAULT_SPACING, 0);
#endif
		
		FXButton *btn = new FXButton(outerHF, buttonCaption, capture_icon, this, ID_CAPTURE, BUTTON_NORMAL|LAYOUT_CENTER_Y);
		btn->setUserData(itr);

		new FXSeparator(this);
		
		itr = itr->next;
	}	
	
	
	//getApp()->addTimeout(this, ID_TIME, 250);
}

InterfaceWindow::~InterfaceWindow()
{
	cout << "removing InterfaceWindow Timer" << endl;
	//getApp()->removeTimeout(this, ID_TIME);
}

long
InterfaceWindow::onTimeout(FXObject *sender, FXSelector sel, void *ptr)
{

	//getApp()->addTimeout(this, ID_TIME, 250);
	return 1;
}

long
InterfaceWindow::onCapture(FXObject *sender, FXSelector sel, void *ptr)
{
	cout << "onCapture" << endl;
	FXId *sender_id = (FXId*) sender;
	selected_interface = (pcap_if_t *) sender_id->getUserData();
	this->getParent()->handle(this, MKUINT(FXDialogBox::ID_ACCEPT,SEL_COMMAND), NULL);	
	return 1;
}
