/*******************************
 FOX GUI for AcuScene Scanner
 Alan Ott. AcuSoft, Inc.
 5-6-2003
*******************************/


#include <fx.h>
#include "PlaybackWindow.h"

int main( int argc, char **argv )
{

	FXApp app("Playback", "Signal 11 Software");
	app.init(argc,argv);
	
	FXMainWindow *mw;
	if( argc < 2 )
		mw = new PlaybackWindow(&app, (const char*)NULL);
	else
		mw = new PlaybackWindow(&app, argv[1]);

	app.create();
	app.run( );
	
	return 0;
}
