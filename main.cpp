/**************************************************************************
    Main GUI Entry
    Copyright 2009 Alan Ott, Signal 11 Software
    
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
