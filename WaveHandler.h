///////////////////////////////////////////////////////////
//
// WaveHandler.h: Performs various wave file operations
// Copyright (c) 2010 - NONYA.  All Rights Reserved
//
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "qstring.h"

namespace UI {

	//////////////////////////////////////////////////////////////////////////
	//! Performs various wave file operations
	class WaveHandler {
	public:
		WaveHandler( );	//!< Initialize
		
		//! Reads the wave file located at \a strFilePath
		bool LoadWaveFile( QString strFilePath );
		
		
	}; // end class WaveHandler
	
} // end namespace UI

