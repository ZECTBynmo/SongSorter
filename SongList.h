///////////////////////////////////////////////////////////
//
// SongList.h: A list of the songs/wav files we're currently looking at
// Copyright (c) 2010 - Mike Vegeto  All Rights Reserved
//
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <QListView>
#include <QTableWidget>

//////////////////////////////////////////////////////////////////////////
//! A list of the songs/wav files we're currently looking at
class SongList : public QTableWidget {
	Q_OBJECT
public:
	SongList( QWidget *parent ); //!< Initialize

private:

}; // end class SongList


