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


#ifndef PLAYBACKWINDOW_H__
#define PLAYBACKWINDOW_H__

#include <fx.h>
#include <pthread.h>
#include <pcap.h>

typedef struct pcap pcap_t;
struct pcap_pkthdr;


class PlaybackWindow : public FXMainWindow  {
	FXDECLARE(PlaybackWindow)
	
public:
	enum {
		ID_FIRST = FXMainWindow::ID_LAST,
		ID_START_BUTTON,
		ID_STOP_BUTTON,
		ID_PAUSE_BUTTON,
		ID_REWIND_BUTTON,
		ID_OK_BUTTON,
		ID_CAPTURE_TIMEOUT,

		ID_OPEN,
		ID_ABOUT,

		ID_LAST,
	};
	
private:
	enum State {
		PLAYING,
		STOPPED,
		PAUSED,
	};
	
	FXIcon *mainWindowIcon;
	FXIcon *mainWindowMiniIcon;

	static FXIcon *closeIcon;
	static FXIcon *startIcon;
	static FXIcon *stopIcon;
	static FXIcon *pauseIcon;
	static FXIcon *rewindIcon;

	FXMenuBar *menubar;
	FXMenuPane *filemenu;

	FXButton       *startButton;
	FXButton       *stopButton;
	FXButton       *pauseButton;
	FXButton       *rewindButton;
	
	FXLabel        *elapsedTimeLabel;
	FXLabel        *slashLabel;
	FXLabel        *totalTimeLabel;
	
	FXLabel        *processedLabel;
	
	FXCheckButton  *syncButton;
	FXProgressBar  *progress;
	
	FXDataTarget   syncTarget;
	
	FXString       filename;
	State          state;  // State is read from the playback thread
	int            send_handle; // UNIX only.

	pcap_t         *pcap_file;
	pcap_t         *live_capture;
	unsigned int   total_packets;
	double         total_time;
	double         capture_start_time;
	double         playback_start_time;
	double         pause_time;
	bool           initialized;
	
	pcap_pkthdr    *next_packet_header;
	const char     *next_packet_data;
	
	// These variables are passed between the playback thread and the
	// main (GUI) thread without locking. It seems to work fine. 
	// elapsed_time and packets_processed are updated by the playback
	// thread and read by the GUI thread. run_thread and sync_playback
	// are updated by the GUI thread and read by the playback thread.
	pthread_t      thread;
	bool           run_thread;
	bool           sync_playback;
	double         elapsed_time;
	unsigned int   packets_processed;
	
	bool isSync(const char *data, size_t length);
	FXString secToString(double sec);
	void setPlaybackTotalTime(double sec);
	void setPlaybackElapsedTime(double sec);
	void setPlaybackPacketsProcessed(unsigned int packets);
	bool sendPacket(const u_char *data, pcap_pkthdr &hdr);
	bool pollSyncPlayback(bool sync_received);
	bool pollASyncPlayback();
	bool openFile();

protected:
	PlaybackWindow() {};
public:
	PlaybackWindow(FXApp *app, FXString filename);
	~PlaybackWindow();
	
	bool isInitialized() { return initialized; }

	void playbackThread();

	
	long onStart(FXObject *sender, FXSelector sel, void *ptr);
	long onStop(FXObject *sender, FXSelector sel, void *ptr);
	long onPause(FXObject *sender, FXSelector sel, void *ptr);
	long onRewind(FXObject *sender, FXSelector sel, void *ptr);
	long onTimer(FXObject *sender, FXSelector sel, void *ptr);
	long onOk(FXObject *sender, FXSelector sel, void *ptr);
	long onTimeout(FXObject *sender, FXSelector sel, void *ptr);
	long onOpen(FXObject *sender, FXSelector sel, void *ptr);
	long onAbout(FXObject *sender, FXSelector sel, void *ptr);
	
	void create();
};

#endif // DATAWINDOW_H__
