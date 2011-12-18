///////////////////////////////////////////////////////////
//
// SongSorter.cpp: MainWindow class
// Copyright (c) 2011 - Mike Vegeto.  All Rights Reserved
// 
//////////////////////////////////////////////////////////////////////////////
// DevStudio!class SongSorter

#include "songsorter.h"
#include <C:\FFTW\fftw3.h>
#include <iostream>
#include <sndfile.hh>
#include <stdio.h>
#include <QLibrary>
#include <vector>
#include <math.h>
#include <QListView>
#include "SongList.h"
#include "DropArea.h"
#include "qstring.h"
#include <algorithm>
#include <math.h>
#include "qmenubar.h"
#include "qlineedit.h"
#include "qresource.h"
#include "qcombobox.h"
#include "qbrush.h"
#include "qpainter.h"
#include "qgraphicseffect.h"
using namespace std;

#ifdef _UNICODE
#define _T(x)      L ## x
#else  _UNICODE */
#define _T(x)      L ## x
#endif /* _UNICODE */

namespace {
	const uint NUM_SAMPLES_READ= 100000,
			   NUM_DATA_COLUMNS= 4,
			   BUFFER_SIZE= 8192;
			   
	// Our sorting methods
	enum {
		SORT_METHOD_SINGLE_PEAK,
		SORT_METHOD_PEAK_RANGE,
		SORT_METHOD_AVE_RANGE,
		
		NUM_SORT_METHODS
	};
	
	const char* const SORT_METHOD_LABELS[NUM_SORT_METHODS]= { "Peak Frequency", "Peak Frequency in Range", "Peak Frequency in Range Average" };
}

SongSorter::SongSorter(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags),
	m_uSortMethod(SORT_METHOD_SINGLE_PEAK),
	m_uTargetFreq(0), m_uFreqRange(1) {

	// Register our resources
	QResource::registerResource( "songsorter.rcc" );
	
	//////////////////////////////////////////////////////////////////////////
	// Create our Objects
	//////////////////////////////////////////////////////////////////////////
	// Create our Song list
	m_pSongList= new SongList( this );
	m_pSongList->setRowCount( 0 );
	m_pSongList->setColumnCount( NUM_DATA_COLUMNS );
	
// 	QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
// 	effect->setBlurRadius(0);
// 	effect->setColor(QColor("#EEEEEE"));
// 	effect->setOffset(1,1);
// 	setGraphicsEffect(effect);

	// Set the headings of the song list
	QStringList strHeaderList;
	strHeaderList.append( QString("File Path") );
	strHeaderList.append( QString("Peak Frequency") );
	strHeaderList.append( QString("Max Amplitude") );
	strHeaderList.append( QString("Sample Rate") );	
	m_pSongList->setHorizontalHeaderLabels( strHeaderList );
	
	// Labels
	m_pLabelFreqChooser= new QLabel( "<font color='white'>Choose target frequency", this );
	m_pLabelFreqRange= new QLabel( "<font color='white'>Choose frequency span", this );
	m_pLabelSortMethod= new QLabel( "<font color='white'>Choose Sort Method", this );
	
	// Frequency Chooser
	m_pFreqChooser= new QSpinBox( this );
	connect( m_pFreqChooser, SIGNAL(valueChanged(int)), this, SLOT(slotTargetFreqChanged(int)) );
	m_pFreqChooser->setMaximum( 20000 );
	m_pFreqChooser->setValue( 500 );
	m_pFreqChooser->setSingleStep( 50 );
	m_pFreqChooser->setSuffix( " Hz" );
	
	// Frequency range chooser
	m_pFreqRangeChooser= new QSpinBox( this );
	connect( m_pFreqRangeChooser, SIGNAL(valueChanged(int)), this, SLOT(slotFreqSpanChanged(int)) );
	m_pFreqRangeChooser->setMinimum( 1 );
	m_pFreqRangeChooser->setMaximum( 5000 );
	m_pFreqRangeChooser->setValue( 40 );
	m_pFreqRangeChooser->setSingleStep( 20 );
	m_pFreqRangeChooser->setSuffix( " Hz" );
	
	// Sort method chooser
	m_pSortMethodChooser= new QComboBox( this );
	connect( m_pSortMethodChooser, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSortMethodChanged(int)) );
	for( uint iMethod=0; iMethod<NUM_SORT_METHODS; ++iMethod )
		m_pSortMethodChooser->addItem( SORT_METHOD_LABELS[iMethod] );
	m_pSortMethodChooser->setCurrentIndex( SORT_METHOD_SINGLE_PEAK );
	
	// Create our Drop Area
	m_pDropArea= new DropArea( this );
	connect(m_pDropArea, SIGNAL(sigSongDropped(QString)), this, SLOT(slotSongAdded(QString)));
	
	// Create our menu bar
	m_pMenuBar = new QMenuBar( this );
	
	// Create our menu headings
	m_pFileMenu = new QMenu(tr("&File"), this);
	
	// Setup our menu
	setupMenu();
	
	// Setup our layout
	setupLayout();
} // end SongSorter::SongSorter()


//////////////////////////////////////////////////////////////////////////////
/*! Overridden paint event */
void SongSorter::paintEvent(QPaintEvent *pe) {
	
	// Setup our background image
	QPainter* pPainter = new QPainter(this);
	pPainter->drawPixmap( m_pFileMenu->x()-2, m_pFileMenu->height()-2, width() + 3, height()-m_pFileMenu->height()+2, 
						  QPixmap("./Images/Background3.png"));
						  
	delete pPainter;
	QWidget::paintEvent(pe);
	
} // end SongSorter::paintEvent()


SongSorter::~SongSorter()
{

}


//////////////////////////////////////////////////////////////////////////////
/*! Take the FFT */
void SongSorter::ProcessSong( QString strFilePath ) {

/*	const char* strFilePath= strFilePath.toLocal8Bit().data();*/
	SF_INFO* audioInfo;
	//sf_open( strFilePath, SFM_READ, audioInfo );
	
	fftw_complex *in, *out;
	fftw_plan p;

	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * BUFFER_SIZE);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * BUFFER_SIZE);
	p = fftw_plan_dft_1d(BUFFER_SIZE, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	
	m_fSongAudioSamples.resize( NUM_SAMPLES_READ );
	m_FFTMagnitudes.resize( BUFFER_SIZE );
	
	SndfileHandle theWavFile( strFilePath.toLatin1(), SFM_READ );
	
	uint uTotalSamples= theWavFile.read(&m_fSongAudioSamples[0], m_fSongAudioSamples.size() );
	
	// Create a SongInfo to represent the file we're processing
	SongInfo songInfo;
	
	// Zero the song's FFT amps
	for( uint iBin=0; iBin<songInfo.fMaxFFTAmps.size(); ++iBin ) {
		songInfo.fMaxFFTAmps[iBin]= 0.0f;
		songInfo.fAverageFFTAmps[iBin]= 0.0f;
	}
	
	uint uReadCount= 0;
	uint uMaxIndex= 0;
	float fMax= 0.0f;
 	while( uReadCount*BUFFER_SIZE + BUFFER_SIZE < uTotalSamples ) {
 	
		// Window our data and copy it into our input
		for( uint iIndex=0; iIndex<BUFFER_SIZE; ++iIndex ) {
			uint uSampleIndex= uReadCount*BUFFER_SIZE + iIndex;
			m_fSongAudioSamples[uSampleIndex]*= windowHann( iIndex, BUFFER_SIZE );
			in[iIndex][0]= m_fSongAudioSamples[uSampleIndex];
			in[iIndex][1]= 0.0f;
		}
		
		// Transform our buffer
		fftw_execute(p);
		
		for( uint iIndex=0; iIndex<BUFFER_SIZE; ++iIndex ) {
			float fCurrentBin= out[iIndex][0];
			
			// Determine whether each bin is a new maximum
			if( abs(fCurrentBin) > songInfo.fMaxFFTAmps[iIndex] )
				songInfo.fMaxFFTAmps[iIndex]= abs(fCurrentBin);
				
			// Sum all frequency bins for averaging
			songInfo.fAverageFFTAmps[iIndex]+= fCurrentBin;
			
			// Record the absolute peak amplitude and it's frequency
			if( fCurrentBin > fMax ) {
				fMax= abs(fCurrentBin);
				uMaxIndex= iIndex;
			}
		}

 		uReadCount++;
 	} // While we still have samples to be read
	for( uint iBin=0; iBin<songInfo.fMaxFFTAmps.size(); ++iBin ) {
		songInfo.fAverageFFTAmps[iBin]= songInfo.fAverageFFTAmps[iBin] / uReadCount;
	}
 	
 	// Divide all our average bins by the number of read cycles to find the average
 	
	
	float fHz= uMaxIndex *( (theWavFile.samplerate()/2.0f)/(BUFFER_SIZE/2.0f) );
	
	songInfo.fMaxAmp= abs( samplesTodB(fMax) );
	songInfo.fMaxFreq= fHz;
	songInfo.strFilePath= strFilePath;
	songInfo.uSampleRate= theWavFile.samplerate();
	
	m_songInfos.push_back( songInfo );
	

} // end SongSorter::ProcessSong()


//////////////////////////////////////////////////////////////////////////////
/*! Use a hanning window on the data */
float SongSorter::windowHann( unsigned int n, unsigned int N ) {
	return 0.5 * (1.0 - cos(2.0 * 3.14159 * n / double(N - 1)));
} // end SongSorter::windowHann()


//////////////////////////////////////////////////////////////////////////////
/*! Populate and position our widgets */
void SongSorter::setupLayout() {
	const uint OBJECTS_WIDTH= 500,
			   OBJECTS_TOTAL_HEIGHT= 600,
			   MENUBAR_Y_OFFSET= m_pMenuBar->height();
			   
	// Set our main window's size
	setFixedSize( OBJECTS_WIDTH+40, OBJECTS_TOTAL_HEIGHT+60+MENUBAR_Y_OFFSET );

	// Set our objects' layouts
	m_pLabelSortMethod->setGeometry( OBJECTS_WIDTH/2 + 40, MENUBAR_Y_OFFSET + 20, 230, m_pLabelFreqChooser->height() );
	m_pSortMethodChooser->setGeometry( OBJECTS_WIDTH/2 + 40, MENUBAR_Y_OFFSET + 50, 230, m_pLabelFreqChooser->height() );
	
	m_pLabelFreqChooser->setGeometry( OBJECTS_WIDTH/2 + 40, MENUBAR_Y_OFFSET + 90, 230, m_pLabelFreqChooser->height() );
	m_pFreqChooser->setGeometry( OBJECTS_WIDTH/2 + 40, MENUBAR_Y_OFFSET + 120, 230, m_pFreqChooser->height() );
	
	m_pLabelFreqRange->setGeometry( OBJECTS_WIDTH/2 + 40, MENUBAR_Y_OFFSET + 160, 230, m_pLabelFreqRange->height() );
	m_pFreqRangeChooser->setGeometry( OBJECTS_WIDTH/2 + 40, MENUBAR_Y_OFFSET + 190, 230, m_pFreqRangeChooser->height() );
	
	m_pMenuBar->setFixedSize( width(), m_pMenuBar->height() );
	m_pSongList->setGeometry( 20, 240+MENUBAR_Y_OFFSET, OBJECTS_WIDTH, 400 );
	m_pDropArea->setGeometry( 20, 20+MENUBAR_Y_OFFSET, OBJECTS_WIDTH/2, 200 );
} // end SongSorter::setupLayout()


//////////////////////////////////////////////////////////////////////////////
/*! Setup our menu */
void SongSorter::setupMenu() {
	m_pMenuBar->addMenu( m_pFileMenu );
} // end SongSorter::setupMenu()


//////////////////////////////////////////////////////////////////////////////
/*! Handle the song that was just added */
void SongSorter::slotSongAdded( QString strFilePath ) {
	// Process the song that was dropped
	ProcessSong( strFilePath );
	
	// Sort by our current criterion
	sortSongs();
} // end SongSorter::slotSongAdded()


//////////////////////////////////////////////////////////////////////////////
/*! Populate our song list */
void SongSorter::populateSongList() {
	m_pSongList->setRowCount( 0 );
	m_pSongList->setColumnCount( NUM_DATA_COLUMNS );

	for( uint iSong=0; iSong<m_songInfos.size(); ++iSong ) {
		int row = m_pSongList->rowCount();
		m_pSongList->insertRow(row);
		m_pSongList->setItem( row, 0, new QTableWidgetItem(QString(m_songInfos[iSong].strFilePath)) );
		m_pSongList->setItem( row, 1, new QTableWidgetItem(QString().setNum(m_songInfos[iSong].fMaxFreq,103,5) + " Hz") );
		m_pSongList->setItem( row, 2, new QTableWidgetItem(QString().setNum(m_songInfos[iSong].fMaxAmp) + " dB") );
		m_pSongList->setItem( row, 3, new QTableWidgetItem(QString().setNum(m_songInfos[iSong].uSampleRate) + " s/s") );
	} // end for each row

	m_pSongList->resizeColumnToContents(0);
} // end SongSorter::populateSongList()


//////////////////////////////////////////////////////////////////////////////
/*! Sort the songs we've analyzed */
void SongSorter::sortSongs() {	
	// Just return if we don't have enough item's to sort
	if( m_songInfos.size() < 2 ) return;
	
	// Use a custom sort struct to sort our song infos
	sort( m_songInfos.begin(), m_songInfos.end(), PeakFreqComparator( m_uSortMethod, m_uTargetFreq, m_uFreqRange ) );	
	
	// Populate our song list with the newly sorted songs
	populateSongList();
} // end SongSorter::sortSongs()


//////////////////////////////////////////////////////////////////////////////
/*! Compares the max frequency of songInfos */
bool SongSorter::compareMaxFreqs(int i,int j) {
	bool bGreaterThan= false;
	
	return false;
} // end (int i,int j()


//////////////////////////////////////////////////////////////////////////////
/*! Converts an amplitude from samples to decibels */
float SongSorter::samplesTodB( float fSampleAmp ) {
	return 6.02 * log2( fSampleAmp );
} // end SongSorter::samplesTodB()


//////////////////////////////////////////////////////////////////////////////
/*! Handle changes to our target frequency */
void SongSorter::slotTargetFreqChanged( int iTargetFreq ) {
	if( m_uTargetFreq == iTargetFreq ) return;
	
	m_uTargetFreq= uint( iTargetFreq );
	
	// Sort by our current criterion
	sortSongs();
} // end SongSorter::slotTargetFreqChanged()


//////////////////////////////////////////////////////////////////////////////
/*! Handle changes to our frequency span */
void SongSorter::slotFreqSpanChanged( int iFreqSpan ) {
	if( m_uFreqRange == iFreqSpan ) return;
	
	m_uFreqRange= uint( iFreqSpan );
	
	// Sort by our current criterion
	sortSongs();
} // end SongSorter::slotFreqSpanChanged()


//////////////////////////////////////////////////////////////////////////////
/*! Handle changes to our sort method */
void SongSorter::slotSortMethodChanged( int iSortMethod ) {

	m_uSortMethod= uint( iSortMethod );
	
	if( m_uSortMethod == SORT_METHOD_SINGLE_PEAK )
		m_pFreqRangeChooser->setEnabled( false );
	else
		m_pFreqRangeChooser->setEnabled( true );
	
	// Sort by our current criterion
	sortSongs();
} // end SongSorter::slotSortMethodChanged()


//////////////////////////////////////////////////////////////////////////////
/*! Our SongInfo constructor */
SongSorter::SongInfo::SongInfo() {
	// Resize our buffers
	fMaxFFTAmps.resize( BUFFER_SIZE );
	fAverageFFTAmps.resize( BUFFER_SIZE );
} // end SongSorter::SongInfo::SongInfo()


//////////////////////////////////////////////////////////////////////////////
/*! Compare our structs */
inline bool SongSorter::PeakFreqComparator::operator() (const SongInfo& song1, const SongInfo& song2) {
	if( uSortMethod == SORT_METHOD_SINGLE_PEAK ) {
		return( abs(uTargetFreq - song1.fMaxFreq) < abs(uTargetFreq - song2.fMaxFreq) );
	} else if( uSortMethod == SORT_METHOD_PEAK_RANGE ) {
		return findAveragePeak( song1 ) > findAveragePeak( song2 );
	} else if( uSortMethod == SORT_METHOD_AVE_RANGE ) {
		return findAverageAve( song1 ) > findAverageAve( song2 );
	}
} // end PeakFreqComparator::operator()


//////////////////////////////////////////////////////////////////////////////
/*! Find the average of the peak amplitudes */
float SongSorter::PeakFreqComparator::findAveragePeak( const SongInfo& song ) {
	
	// Figure out which frequency bins we're looking at
	uint uStartBin= hzToBin( uTargetFreq-uFreqSpan/2, song.uSampleRate ),
		 uFinishBin= hzToBin( uTargetFreq+uFreqSpan/2, song.uSampleRate ) + 1;
	double dSum= 0;
	
	// Sum the peak amplitudes in the selected frequencies
	for( uint iFreqBin=uStartBin; iFreqBin< uFinishBin; ++iFreqBin ) {
		dSum+= song.fMaxFFTAmps[iFreqBin];
	}
	float fAverage= dSum/(uFinishBin-uStartBin);

	// Calculate and return the average
	return fAverage;
} // end SongSorter::PeakFreqComparator::findAveragePeak()


//////////////////////////////////////////////////////////////////////////////
/*! Find the average of the average amplitudes */
float SongSorter::PeakFreqComparator::findAverageAve( const SongInfo& song ) {
	// Sum the peak amplitudes in the selected frequencies
	uint uStartBin= hzToBin( uTargetFreq-uFreqSpan/2, song.uSampleRate ),
		uFinishBin= hzToBin( uTargetFreq+uFreqSpan/2, song.uSampleRate );

	double dSum= 0;

	for( uint iFreqBin=uStartBin; iFreqBin<=uFinishBin; ++iFreqBin ) {
		dSum+= song.fMaxFFTAmps[iFreqBin];
	}
	float fAverage= dSum/(uFinishBin-uStartBin);

	// Calculate and return the average
	return fAverage;
} // end SongSorter::PeakFreqComparator::findAverageAve()


//////////////////////////////////////////////////////////////////////////////
/*! returns the frequency bin closes to the desired frequency */
uint SongSorter::PeakFreqComparator::hzToBin( float fHz, uint uSampleRate ) {
	uint uBin= fHz / ( (uSampleRate/2.0f)/(BUFFER_SIZE/2.0f) );
	
	return uBin;
} // end SongSorter::PeakFreqComparator::hzToBin()