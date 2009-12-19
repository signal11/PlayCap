/**************************************************************************
    Window for Playing Back captures
    Copyright 2009 Alan Ott, Signal 11 Software
    10-1-2009

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

#include <stdio.h>
#include <pcap.h>
#include <pthread.h>

#include "PlaybackWindow.h"
#include "InterfaceWindow.h"
#include "AboutWindow.h"
#include "famfamfam/cross-orig.png.h"
#include "tango/media-playback-start.png.h"
#include "tango/media-playback-stop.png.h"
#include "tango/media-playback-pause.png.h"
#include "tango/media-skip-backward.png.h"
#include "signal11icon.h"
#include "signal11-smallicon.h"

#include "FXPNGIcon.h"
#include "fxver.h"

#ifdef WIN32
	#include <mmsystem.h>
#else
	// Header files for send() on UNIX platforms
	#include <sys/types.h>
	#include <sys/socket.h>
#endif

#include <iostream>
using std::cout;
using std::endl;

FXDEFMAP(PlaybackWindow) PlaybackWindowMap[] = {
	FXMAPFUNC( SEL_COMMAND, PlaybackWindow::ID_OK_BUTTON, PlaybackWindow::onOk ),
	FXMAPFUNC( SEL_COMMAND, PlaybackWindow::ID_START_BUTTON, PlaybackWindow::onStart ),
	FXMAPFUNC( SEL_COMMAND, PlaybackWindow::ID_STOP_BUTTON, PlaybackWindow::onStop ),
	FXMAPFUNC( SEL_COMMAND, PlaybackWindow::ID_PAUSE_BUTTON, PlaybackWindow::onPause ),
	FXMAPFUNC( SEL_COMMAND, PlaybackWindow::ID_REWIND_BUTTON, PlaybackWindow::onRewind ),
	FXMAPFUNC( SEL_TIMEOUT, PlaybackWindow::ID_CAPTURE_TIMEOUT, PlaybackWindow::onTimeout ),
	FXMAPFUNC( SEL_COMMAND, PlaybackWindow::ID_OPEN, PlaybackWindow::onOpen ),
	FXMAPFUNC( SEL_COMMAND, PlaybackWindow::ID_ABOUT, PlaybackWindow::onAbout ),
};

FXIMPLEMENT(PlaybackWindow, FXMainWindow, PlaybackWindowMap, ARRAYNUMBER(PlaybackWindowMap));


FXIcon *PlaybackWindow::closeIcon;
FXIcon *PlaybackWindow::startIcon;
FXIcon *PlaybackWindow::stopIcon;
FXIcon *PlaybackWindow::pauseIcon;
FXIcon *PlaybackWindow::rewindIcon;

static double tv_to_double(const struct timeval *tv)
{
	return (double)tv->tv_sec + (double)tv->tv_usec / 1000000.0;
}

#ifdef WIN32
static double get_time()
{
	LARGE_INTEGER count, freq;
	QueryPerformanceCounter(&count);
	QueryPerformanceFrequency(&freq);
	return (double)count.QuadPart / (double)freq.QuadPart;
}
#else
static double get_time()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv_to_double(&tv);
}
#endif

static void *thread_function(void *ptr)
{
	PlaybackWindow *pw = (PlaybackWindow*) ptr;
	pw->playbackThread();

	return NULL;
}


PlaybackWindow::PlaybackWindow(FXApp *app, FXString filename)
	: FXMainWindow(app, "PlayCap", NULL/*icon*/, NULL/*icon*/, DECOR_CLOSE|DECOR_MINIMIZE|DECOR_MAXIMIZE|DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE, 200,200,250,0 )
{
	/* Initialize class members */
	next_packet_data   = NULL;
	next_packet_header = (struct pcap_pkthdr*) calloc(1,sizeof(struct pcap_pkthdr));
	pcap_file = NULL;
	live_capture = NULL;
	this->filename = filename;
	state = STOPPED;
	send_handle = -1;
	total_packets = 0;
	packets_processed = 0;
	initialized = false;

#ifdef WIN32
	timeBeginPeriod(1);
#endif

	/* Window Icons, large and small */
	mainWindowIcon = new FXICOIcon(getApp(), Signal11, FXRGB(192,192,192),0,16,16);
	setIcon(mainWindowIcon);
	mainWindowMiniIcon = new FXICOIcon(getApp(), Signal11small, FXRGB(192,192,192),0,16,16);
	setMiniIcon(mainWindowMiniIcon);

	/* Create Icons */
	startIcon = new FXPNGIcon(getApp(), mediaplaybackstart);
	startIcon->blend(getBackColor());
	stopIcon = new FXPNGIcon(getApp(), mediaplaybackstop);
	stopIcon->blend(getBackColor());
	pauseIcon= new FXPNGIcon(getApp(), mediaplaybackpause);
	pauseIcon->blend(getBackColor());
	rewindIcon= new FXPNGIcon(getApp(), mediaskipbackward);
	rewindIcon->blend(getBackColor());

	
	/* Create Controls */
	FXVerticalFrame *topVF = new FXVerticalFrame(this, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);

	/* Create Menu */
	menubar = new FXMenuBar(topVF, LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
	filemenu = new FXMenuPane(this);
	new FXMenuTitle(menubar, "&File", NULL, filemenu);
	new FXMenuCommand(filemenu, "&Open...\tCtrl-O\tOpen Existing File", NULL, this, ID_OPEN);
	new FXMenuSeparator(filemenu);
	new FXMenuCommand(filemenu, "E&xit\tCtrl-X", NULL, getApp(), FXApp::ID_QUIT);

	FXMenuPane *helpmenu = new FXMenuPane(this);
	new FXMenuTitle(menubar, "&Help", NULL, helpmenu);
	new FXMenuCommand(helpmenu, "&About...", NULL, this, ID_ABOUT);
	
	new FXSeparator(topVF);

	FXVerticalFrame *vf = new FXVerticalFrame(topVF, LAYOUT_FILL_X);

	/* Buttons */
	FXHorizontalFrame *hf = new FXHorizontalFrame(vf, 0x0, 0,0,0,0, 0,0,0,0);
	startButton = new FXButton(hf, "\tPlay", startIcon, this, ID_START_BUTTON);
	stopButton  = new FXButton(hf, "\tStop", stopIcon, this, ID_STOP_BUTTON);
	pauseButton = new FXButton(hf, "\tPause", pauseIcon, this, ID_PAUSE_BUTTON);
	//rewindButton= new FXButton(hf, "\tRewind", rewindIcon, this, ID_PAUSE_BUTTON);
	startButton->disable();
	stopButton->disable();
	pauseButton->disable();
	
	/* Progress Bar */
	progress = new FXProgressBar(new FXHorizontalFrame(vf, LAYOUT_FILL_X|FRAME_SUNKEN,0,0,0,0,0,0,0,0), this, ID_START_BUTTON, LAYOUT_FILL_X);
	progress->setProgress(0);

	/* Packets processed Label */
	processedLabel = new FXLabel(vf, "");

	/* Elapsed Time */
	hf = new FXHorizontalFrame(vf, 0x0, 0,0,0,0, 0,0,0,0);
	elapsedTimeLabel = new FXLabel(hf,secToString(0.0));
	slashLabel = new FXLabel(hf, "/");
	totalTimeLabel = new FXLabel(hf, secToString(0.0));


	/* Close button with icon */	
	if (!PlaybackWindow::closeIcon) {
		PlaybackWindow::closeIcon = new FXPNGIcon(getApp(), crossorig);
		PlaybackWindow::closeIcon->blend(getBackColor());
	}
	new FXButton(vf, "Close", closeIcon, this, ID_CLOSE, BUTTON_NORMAL|LAYOUT_RIGHT);


	// Set the packets processed readout.
	setPlaybackPacketsProcessed(0);

	pcap_file = NULL;
}

PlaybackWindow::~PlaybackWindow()
{
	if (state == PLAYING || state == PAUSED)
		onStop(NULL,0,NULL);
	free(next_packet_header);
	
	// FOX menus must be deleted explicitly.
	delete filemenu;
}

FXString
PlaybackWindow::secToString(double sec)
{
	char time_string[64];
	
	int    minutes = sec / 60;
	double seconds = fmod(sec,60);

	sprintf(time_string, "%d:%06.3f", minutes, seconds);
	
	return time_string;
}

void
PlaybackWindow::setPlaybackTotalTime(double sec)
{
	totalTimeLabel->setText(secToString(sec));
}

void
PlaybackWindow::setPlaybackElapsedTime(double sec)
{
	elapsedTimeLabel->setText(secToString(sec));
}

void
PlaybackWindow::setPlaybackPacketsProcessed(unsigned int packets)
{
	FXString s;
	s.format("Processed %u of %u", packets_processed, total_packets);
	processedLabel->setText(s);
}

bool
PlaybackWindow::sendPacket(const u_char *data, pcap_pkthdr &hdr)
{
	bool error = false;

	// On Windows, use pcap_send(). On Linux (and others) use
	// the fd handle from libpcap. This is because pcap_send()
	// on UNIX platforms is a newer feature, and I'd like it to
	// be as portable to older systems as possible.
#ifdef WIN32
	int ret;
	ret = pcap_sendpacket(live_capture, data, hdr.caplen);
	if (ret < 0) {
		error = true;
	}
#else
	int ret;
	ret = send(send_handle, data, hdr.caplen, 0);
	if (ret < 0) {
		error = true;
	}
#endif

	if (error) {
		FXMessageBox::information( this, MBOX_OK, "Network Error", "Unable to send data on the network.\n");
		onStop(NULL,0,NULL);
	}

	return true;
}

/* This is the playback function. It gets called every millisecond. When
 * called, it will determine if the next packet in the capture needs to be
 * sent. It will then send all the packets in the capture which need to be
 * sent. Since libpcap does not allow a client program to move to the
 * previous packet, peek into the capture, or put a packet back once it has
 * been removed (using pcap_next()), every packet retrieved is stored in the
 * next_packet_data pointer. This way, if it's not time to send that packet
 * yet, it is stored for the next time pollASyncPlayback() gets called.
 */
bool
PlaybackWindow::pollASyncPlayback()
{
	/* Return immediately if we're paused (or stopped, but that
	   shouldn't happen as this function isn't called from STOP). */
	if (state != PLAYING)
		return true;

	bool ready_to_send = false;

	do {
		ready_to_send = false;
		
		// See if it's ready to send
		if (!next_packet_data) {
			// Get the next packet
			next_packet_data = (const char*)
				    pcap_next(pcap_file, next_packet_header);
		
			// next_packet_data now contains the next packet to
			// send, or it's NULL, meaning end of capture.
		}
		
		if (next_packet_data) {
			// See if it's time to send this packet.
			double pkt_time = 
				tv_to_double(&next_packet_header->ts)
				- capture_start_time;
			double playback_time =
				get_time() - playback_start_time;
			
			if (playback_time > pkt_time)
				ready_to_send = true;
			
			// Update the time readout
			elapsed_time = playback_time;
		}
		else {
			// next_packet_data was NULL, so We're done,
			// don't get called again.
			return false;
		}

		if (ready_to_send) {
			// Update the progress status.
			packets_processed++;
			
			// Send the packet.
			sendPacket((const u_char*)next_packet_data, *next_packet_header);

			// Set the next packet to NULL, so that a new one
			// will be grabbed from the file on the next iteration.
			next_packet_data = NULL;
		}
	} while (ready_to_send);


	/* Call this function again */
	return true;
}

/* This is the playback thread. It calls pollASyncPlayback() every
 * millisecond. When the playback reaches its end, pollASyncPlayback() will
 * return false.
 */
void
PlaybackWindow::playbackThread()
{
	bool keep_going = true;
	while (run_thread && keep_going) {

		keep_going = pollASyncPlayback();

		// Sleep for 1 millisecond.
		#define SLEEP_MILLIS 1
		#ifdef WIN32
			Sleep(SLEEP_MILLIS);
		#else
			usleep(SLEEP_MILLIS*1000);
		#endif
	}
	printf("Exiting playback thread.\n");
	run_thread = false;
}

/* This function gets called periodically (every 5ms or so) from the main
 * GUI thread. Its job is to update the GUI based on the packets_processed
 * and elapsed_time variables. There's no mutex on them, and this seems to
 * work well enough on Linux and Windows platforms. The worst case here (if
 * for some reason it were to not work) is that a garbage value would be
 * printed in the GUI one frame. This has not been observed.
 */
long
PlaybackWindow::onTimeout(FXObject *, FXSelector, void*)
{
	/* Update the progress indicator */
	progress->setProgress(packets_processed);

	/* Update the playback time indicator */
	setPlaybackElapsedTime(elapsed_time);
	
	/* Update the packets processed label */
	setPlaybackPacketsProcessed(packets_processed);

	
	/* Call us again if we're still running, otherwise
	 * get into the STOP state. */
	if (run_thread) {
		// FOX 1.7 changes the timeouts to all be nanoseconds.
		// Fox 1.6 had all timeouts as milliseconds.
		int timeout_scalar = 1;
		#if (FOX_MINOR >= 7)
			timeout_scalar = 1000*1000;
		#endif
		getApp()->addTimeout(this, ID_CAPTURE_TIMEOUT,
			5 * timeout_scalar /*5ms*/);
	}
	else {
		/* Thread is not running because the playback has finised.
		   onStop() will reset the player. */
		onStop(NULL,0,NULL);
	}
	
	return 1;
}

long
PlaybackWindow::onStart(FXObject *, FXSelector, void*)
{
	// If we're PAUSED, then just set the state back to playing.
	// There's no need to re-open the socket or anything.
	if (state == PAUSED) {
		state = PLAYING;
		startButton->disable();
		stopButton->enable();
		pauseButton->enable();
		//rewindButton->disable();

		// Fake the playback_start_time, so that the time-based
		// asynchronous playback will pick up where it left off.
		playback_start_time += get_time() - pause_time;

		return 1;
	}

	/* Starting the capture from the beginning. */

	progress->setProgress(0);

	pcap_if_t *devs = NULL;
	char errbuf[PCAP_ERRBUF_SIZE];
	
	// Open the capture file.
	pcap_file = pcap_open_offline(filename.text(), errbuf);
	if (!pcap_file) {
		FXMessageBox::information( this, MBOX_OK, "File Open Error", "Can't open original file %s for reading.\n", filename.text() );
	}

	// Popup window with list of interfaces in it.
	pcap_findalldevs(&devs, errbuf);
	if (devs) {
		// Show the interface Window.
		FXDialogBox *dlg = new FXDialogBox(this, "Select a device", DECOR_CLOSE|DECOR_TITLE|DECOR_BORDER);
		InterfaceWindow *ifwin = new InterfaceWindow(dlg, devs, "Select Playback Device", "Playback");
		FXuint res = dlg->execute();
		
		if (res) {
			// User selected an interface. Start capturing.
			cout << "User selected " << ifwin->getSelectedInterface()->name << endl;
			
			live_capture = pcap_open_live(ifwin->getSelectedInterface()->name, 9999, 1/*promisc*/, 5/*read_timeout*/, errbuf);
			if (live_capture) {
				res = pcap_setnonblock(live_capture, 1, errbuf);
				if (res >= 0) {
				
				}
				else
					cout << "Error setting nonblock: " << errbuf << endl;

			#ifndef WIN32
				send_handle = pcap_get_selectable_fd(live_capture);
			#else
				send_handle = 0;
			#endif
				if (send_handle >= 0) {
					// Set the GUI and app state to
					// the PLAYING state.
					state = PLAYING;
					startButton->disable();
					stopButton->enable();
					pauseButton->enable();
					//rewindButton->disable();
					playback_start_time = get_time();

					// Start the playback thread.
					run_thread = true;
					packets_processed = 0;
					elapsed_time = 0.0;
					pthread_create(&thread, NULL, &thread_function, this);
					// Start the periodic update timer.
					// FOX 1.7 changes the timeouts to all be nanoseconds.
					// Fox 1.6 had all timeouts as milliseconds.
					int timeout_scalar = 1;
					#if (FOX_MINOR >= 7)
						timeout_scalar = 1000*1000;
					#endif
					getApp()->addTimeout(this, ID_CAPTURE_TIMEOUT, 1 * timeout_scalar  /*1ms*/);
				}
				else {
					FXMessageBox::information(this, MBOX_OK, "Networking Error", "Unable to get a write handle for the playback.");
				}
			}
		}
		else {
			// User pushed cancel from the interface window.
		}

		delete dlg;
	}
	else {
		// report error.
		cout << "Error finding devices " << errbuf << endl;
#ifdef WIN32
		const char *privs = "Administrator";
#else
		const char *privs = "root user";
#endif
		FXString s;
		s.format("There was an error trying to open the network device.\nThis most likely means that you do not have permission to open raw sockets.\nEnsure that you have %s privileges and try again.\n\nError: %s", privs, errbuf);
		FXMessageBox::error(this, MBOX_OK, "Capture Error", "%s", s.text());
	}
	
	pcap_freealldevs(devs);

	return 1;
}

long
PlaybackWindow::onStop(FXObject *, FXSelector, void*)
{
	
	// Reset the state of the GUI
	state = STOPPED;
	progress->setProgress(0);
	startButton->enable();
	stopButton->disable();
	pauseButton->disable();
	//rewindButton->disable();
	
	next_packet_data = NULL;
	
	getApp()->removeTimeout(this, ID_CAPTURE_TIMEOUT);
	
	// Stop the playback thread and wait for it to exit.
	run_thread = false;
	pthread_join(thread, NULL);

	elapsed_time = 0.0;
	packets_processed = 0;
	setPlaybackPacketsProcessed(0);
	setPlaybackElapsedTime(0.0);

	// Clean up the libpcap objects.
	pcap_close(live_capture);
	pcap_close(pcap_file);
	send_handle = -1;

	cout << "Stopped" << endl;

	return 1;
}

long
PlaybackWindow::onPause(FXObject *, FXSelector, void*)
{
	state = PAUSED;
	startButton->enable();
	stopButton->enable();
	pauseButton->disable();
	//rewindButton->enable();

	pause_time = get_time();
	
	return 1;
}

long
PlaybackWindow::onRewind(FXObject *, FXSelector, void*)
{
	/* What to do here ?? */

	return 1;
}

long
PlaybackWindow::onOk(FXObject *, FXSelector, void*)
{
	return 1;
}

long
PlaybackWindow::onOpen(FXObject *, FXSelector, void*)
{
	char filename[512];
	char file_title[512];
	int res;

	filename[0] = '\0';
	file_title[0] = '\0';

#ifdef WIN32

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = (HWND) this->id();
	ofn.hInstance = NULL;//GetModuleHandle(NULL);
	ofn.lpstrFilter = "All Files\0*.*\0\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.lpstrFileTitle = file_title;
	ofn.nMaxFileTitle = sizeof(file_title);
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	ofn.Flags = /*OFN_DONTADDTORECENT |*/ OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = 0;
	ofn.lCustData = 0;
	ofn.lpfnHook = 0;
	ofn.lpTemplateName = 0;
	//ofn.pvReserved = 0;
	//ofn.dwReserved = 0;
	//ofn.FlagsEx = 0;


	res = GetOpenFileName( &ofn );
	unsigned int err = CommDlgExtendedError();

#else
	// Use the Fox File open dialog.
	FXFileDialog *dlg = new FXFileDialog(this, "Open PCAP File");
	dlg->setPatternList("TCPDump Files (*.dump,*.tcpdump,*.pcap)\nAll Files (*)");
	res = dlg->execute();
	strncpy(filename, dlg->getFilename().text(), sizeof(filename));
	filename[sizeof(filename)-1] = '\0';


#endif
	if( res ) {
		/* Open the file */
		FXApp::instance()->beginWaitCursor();
		this->filename = filename;
		openFile();
		FXApp::instance()->endWaitCursor();
	}

	return 1;
}

long
PlaybackWindow::onAbout(FXObject *, FXSelector, void*)
{
	AboutWindow box(this);
	box.execute(PLACEMENT_OWNER);

	return 1;
}


/* This function is called by Fox Toolkit when the window is actually
 * created on the X server */
void
PlaybackWindow::create()
{
	FXMainWindow::create();
	
	/* Show the window */
	show();

	/* Open the file, if one was provided */
	if (filename != "")
		openFile();
}

bool
PlaybackWindow::openFile()
{

	// If we are playing, stop.
	if (state == PLAYING || state == PAUSED)
		onStop(NULL,0,NULL);

	// Count the total number of packets in the file and find out
	// what the total elapsed time of the capture is. The only way to do
	// this with libpcap is to call pcap_next() until you get to the
	// end.
	const u_char *data;
	struct pcap_pkthdr header;
	char errbuf[PCAP_ERRBUF_SIZE];
	
	// Open the file.
	pcap_file = pcap_open_offline(filename.text(), errbuf);
	if (!pcap_file) {
		FXMessageBox::information( getApp(), MBOX_OK, "File Open Error", "Can't open original file %s for reading.\n", filename.text() );
		initialized = false;
		return false;
	}
	else {
		total_packets = 0;
		memset(&header,0,sizeof(header));

		// Look at the first packet and get the start time;
		capture_start_time = 0;
		if (pcap_next(pcap_file, &header)) {
			capture_start_time = tv_to_double(&header.ts);
			total_packets++;
		}

		// Count the packets in the file.
		while ((data = pcap_next(pcap_file, &header))) {
			total_packets++;
		}
		
		// Get the total time based on the _last_
		// packet header in the file.
		total_time = tv_to_double(&header.ts) - capture_start_time;
		setPlaybackTotalTime(total_time);
		

		// Set the total number of packets in the progress bar.
		progress->setTotal(total_packets);

		// Close the open file.
		pcap_close(pcap_file);
		pcap_file = NULL;
		
		// Set the state of the buttons.
		startButton->enable();
		stopButton->disable();
		pauseButton->disable();

		// Reset the counters.
		elapsed_time = 0.0;
		packets_processed = 0;
		setPlaybackPacketsProcessed(0);
		setPlaybackElapsedTime(0.0);
		
		initialized = true;
	}
	
	return true;
}

