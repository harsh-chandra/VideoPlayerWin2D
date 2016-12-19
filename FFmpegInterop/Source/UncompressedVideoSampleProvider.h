//*****************************************************************************
//
//	Copyright 2015 Microsoft Corporation
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//	http ://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.
//
//*****************************************************************************

#pragma once
#include "MediaSampleProvider.h"
#define OUTPUT_FILE_PREFIX "c:\\temp\\image%d.bmp"

extern "C"
{
#include <libswscale/swscale.h>
}


namespace FFmpegInterop
{
	ref class UncompressedVideoSampleProvider: MediaSampleProvider
	{
	public:
		virtual ~UncompressedVideoSampleProvider();


	internal:
		UncompressedVideoSampleProvider(
			FFmpegReader^ reader,
			AVFormatContext* avFormatCtx,
			AVCodecContext* avCodecCtx);
		virtual HRESULT WriteAVPacketToStream(DataWriter^ writer, AVPacket* avPacket) override;
		virtual HRESULT DecodeAVPacket(DataWriter^ dataWriter, AVPacket* avPacket) override;
		virtual HRESULT AllocateResources() override;

	private:
		AVFrame* m_pAvFrame;
		SwsContext* m_pSwsCtx;
		int m_rgVideoBufferLineSize[4];
		uint8_t* m_rgVideoBufferData[4];
		AVFrame* GetRGBAFrame(AVFrame *pFrameYuv);
		bool BMPSave(const char * pFileName, AVFrame * frame, int w, int h);
		AVFrame * GetNextFrame(AVFrame *frameYUV);
	};

	typedef struct tagBITMAPFILEHEADER {
		WORD    bfType;
		DWORD   bfSize;
		WORD    bfReserved1;
		WORD    bfReserved2;
		DWORD   bfOffBits;
	} BITMAPFILEHEADER, FAR *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
}

