

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
