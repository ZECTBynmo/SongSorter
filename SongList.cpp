///////////////////////////////////////////////////////////
//
// SongList.cpp: A list of the songs/wav files we're currently looking at
// Copyright (c) 2011 - Mike Vegeto  All Rights Reserved
// 
//////////////////////////////////////////////////////////////////////////////
// DevStudio!class SongList
#include "SongList.h"
#include <QTableWidget>

//////////////////////////////////////////////////////////////////////////////
/*! Initialize */
SongList::SongList( QWidget *parent ) :
QTableWidget( parent ) {

	setShowGrid( true );
	
} // end SongList::SongList()
