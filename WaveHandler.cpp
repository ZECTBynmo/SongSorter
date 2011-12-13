///////////////////////////////////////////////////////////
//
// WaveHandler.cpp: Performs various wave file operations
// Copyright (c) 2010 - iZotope, Inc.  All Rights Reserved
// 
//////////////////////////////////////////////////////////////////////////////
// DevStudio!namespace UI DevStudio!class WaveHandler
#include "WaveHandler.h"
#include "QFile.h"

//////////////////////////////////////////////////////////////////////////////
/*! Initialize */
UI::WaveHandler::WaveHandler( ) {
	
	
} // end WaveHandler::WaveHandler()


//////////////////////////////////////////////////////////////////////////////
/*! Reads the wave file located at \a strFilePath */
bool UI::WaveHandler::LoadWaveFile( QString strFilePath ) {

	bool bResult= false;
	
	// Close();
	
	// Open the file
	QFile theWavFile( strFilePath );

// 	BOOL bResult = FALSE;
// 
// 	// Close output device
// 	Close();
// 
// 	// Load .WAV file
// 	FILE* file = _tfopen(lpszFilePath, _T("rb"));
// 	if (file != NULL)
// 	{
// 		// Read .WAV descriptor
// 		fread(&m_Descriptor, sizeof(_WAVEDESCR), 1, file);
// 
// 		// Check for valid .WAV file
// 		if (strncmp((LPCSTR)m_Descriptor.wave, "WAVE", 4) == 0)
// 		{
// 			// Read .WAV format
// 			fread(&m_Format, sizeof(_WAVEFORMAT), 1, file);
// 
// 			// Check for valid .WAV file
// 			if ((strncmp((LPCSTR)m_Format.id, "fmt", 3) == 0) && (m_Format.format == 1))
// 			{
// 				// Read next chunk
// 				BYTE id[4];
// 				DWORD size;
// 				fread(id, sizeof(BYTE), 4, file);
// 				fread(&size, sizeof(DWORD), 1, file);
// 				DWORD offset = ftell(file);
// 
// 				// Read .WAV data
// 				LPBYTE lpTemp = (LPBYTE)malloc(m_Descriptor.size*sizeof(BYTE));
// 				while (offset < m_Descriptor.size)
// 				{
// 					// Check for .WAV data chunk
// 					if (strncmp((LPCSTR)id, "data", 4) == 0)
// 					{
// 						if (m_lpData == NULL)
// 							m_lpData = (LPBYTE)malloc(size*sizeof(BYTE));
// 						else
// 							m_lpData = (LPBYTE)realloc(m_lpData, (m_dwSize+size)*sizeof(BYTE));
// 						fread(m_lpData+m_dwSize, sizeof(BYTE), size, file);
// 						m_dwSize += size;
// 					}
// 					else
// 						fread(lpTemp, sizeof(BYTE), size, file);
// 
// 					// Read next chunk
// 					fread(id, sizeof(BYTE), 4, file);
// 					fread(&size, sizeof(DWORD), 1, file);
// 					offset = ftell(file);
// 				}
// 				free(lpTemp);
// 
// 				// Open output device
// 				if (!Open(m_Format.channels, m_Format.sampleRate, m_Format.bitsPerSample))
// 				{
// 					m_hWaveout = NULL;
// 					bResult = FALSE;
// 				}
// 				else
// 					bResult = TRUE;
// 			}
// 		}
// 
// 		// Close .WAV file
// 		fclose(file);
// 	}
// 
// 	return bResult;
	
	return true;
} // end WaveHandler::LoadWaveFile()
