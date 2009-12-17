/********************************
 Interface Window for IG Scanner
 Alan Ott
 10-18-2006
********************************/

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
