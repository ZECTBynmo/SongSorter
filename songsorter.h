#ifndef SONGSORTER_H
#define SONGSORTER_H

#include <QtGui/QMainWindow>
#include <QListView>
#include "SongList.h"
#include "DropArea.h"
#include <vector>
#include <math.h>
#include "qlineedit.h"
#include "qspinbox.h"
#include "qcombobox.h"
using namespace std;

class SongSorter : public QMainWindow
{
	Q_OBJECT

public:
	SongSorter(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SongSorter();
	
public slots:
	void slotSongAdded( QString strFilePath );		//!< Handle the song that was just added
	void slotTargetFreqChanged( int iTargetFreq );	//!< Handle changes to our target frequency
	void slotFreqSpanChanged( int iFreqSpan );		//!< Handle changes to our frequency span
	void slotSortMethodChanged( int iSortMethod );	//!< Handle changes to our sort method

private:

	struct SongInfo {
		SongInfo();		//!< Our SongInfo constructor
		
		bool operator() (int i,int j) { return (i<j); }
		
		QString strFilePath;	//!< Our file path
		
		float fMaxFreq,			//!< The frequency with the highest energy in the file
			  fMaxAmp;			//!< The highest amplitude found the file
			  
		vector<float> fMaxFFTAmps,		//!< A collection of the file's largest FFT amplitudes
					  fAverageFFTAmps;	//!< A collection of the file's average FFT amplitudes
					  
		uint uSampleRate;		//!< The file's sample rate
	}; // End struct SongInfo
	vector<SongInfo> m_songInfos;
	
	// Custom comparison struct
	struct PeakFreqComparator {
		PeakFreqComparator( uint uUserSortMethod, uint uUserTargetFreq, uint uUserFreqSpan ) { 
			uSortMethod= uUserSortMethod; 
			uTargetFreq= uUserTargetFreq;
			uFreqSpan= uUserFreqSpan;
		} // end PeakFreqComparator()
		uint uSortMethod,		//!< Our current comparison mode
			 uTargetFreq,	//!< The freq we're targeting	
			 uFreqSpan;		//!< The span of our analysis
			 
		inline bool operator() (const SongInfo& struct1, const SongInfo& struct2);		//!< Custom comparison operator
		float findAveragePeak( const SongInfo& song );	//!< Find the average of the peak amplitudes
		float findAverageAve( const SongInfo& song );	//!< Find the average of the average amplitudes
		uint hzToBin( float fHz, uint uSampleRate );	//!< returns the frequency bin closes to the desired frequency

	};

	vector<float> m_fSongAudioSamples,	//!< Our audio samples buffer
				  m_FFTMagnitudes;		//!< Our FFT output buffer
				  
	QLabel* m_pLabelFreqChooser,		//!< Our frequency target chooser label
		  * m_pLabelFreqRange,			//!< Our frequency range chooser
		  * m_pLabelSortMethod;			//!< Our sort method chooser label
		  
	QSpinBox* m_pFreqChooser,			//!< Target frequency entry
			* m_pFreqRangeChooser;		//!< Frequency range text edit
			
	QComboBox* m_pSortMethodChooser;	//!< Sort method drop down
	
	SongList* m_pSongList;
	DropArea* m_pDropArea;
	
	QMenuBar *m_pMenuBar;		//!< Our menu bar
	QMenu *m_pFileMenu;
	
	uint m_uSortMethod,		//!< Our current comparison mode
		 m_uTargetFreq,		//!< Our current target frequency
		 m_uFreqRange;		//!< Our current range of frequencies to analyze
	
	//! Overridden paint event
	virtual void paintEvent(QPaintEvent *);
	
	//! Populate and position our widgets
	void setupLayout();
	//! Setup our menu
	void setupMenu();
	
	//! Populate our song list
	void populateSongList();
	
	void ProcessSong( QString strFilePath ); //!< Take the FFT
	void sortSongs();		//!< Sort the songs we've analyzed
		
	float windowHann( unsigned int uSample, unsigned int uWindowSize ); //!< Returns the appropriate coefficient in a hann window
	bool compareMaxFreqs(int i,int j);		//!< Compares the max frequency of songInfos
	float samplesTodB( float fSampleAmp );	//!< Converts an amplitude from samples to decibels
		
	// Calculates log2 of number.  
	float log2( float n ) {	return log( n ) / log( 2.0f ); }

};

#endif // SONGSORTER_H
