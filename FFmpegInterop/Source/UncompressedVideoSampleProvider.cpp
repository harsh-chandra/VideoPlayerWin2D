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

#include "pch.h"
#include "UncompressedVideoSampleProvider.h"
#include <iostream>

extern "C"
{
#include <libavutil/imgutils.h>
}


using namespace FFmpegInterop;

UncompressedVideoSampleProvider::UncompressedVideoSampleProvider(
	FFmpegReader^ reader,
	AVFormatContext* avFormatCtx,
	AVCodecContext* avCodecCtx)
	: MediaSampleProvider(reader, avFormatCtx, avCodecCtx)
	, m_pAvFrame(nullptr)
	, m_pSwsCtx(nullptr)
{
	for (int i = 0; i < 4; i++)
	{
		m_rgVideoBufferLineSize[i] = 0;
		m_rgVideoBufferData[i] = nullptr;
	}
}

HRESULT UncompressedVideoSampleProvider::AllocateResources()
{
	HRESULT hr = S_OK;
	hr = MediaSampleProvider::AllocateResources();
	if (SUCCEEDED(hr))
	{
		// Setup software scaler to convert any decoder pixel format (e.g. YUV420P) to NV12 that is supported in Windows & Windows Phone MediaElement
		m_pSwsCtx = sws_getContext(
			m_pAvCodecCtx->width,
			m_pAvCodecCtx->height,
			m_pAvCodecCtx->pix_fmt,
			m_pAvCodecCtx->width,
			m_pAvCodecCtx->height, 
			AV_PIX_FMT_RGB24,
			SWS_BICUBIC,
			NULL,
			NULL,
			NULL);

		if (m_pSwsCtx == nullptr)
		{
			hr = E_OUTOFMEMORY;
		}
	}

	if (SUCCEEDED(hr))
	{
		m_pAvFrame = av_frame_alloc();
		if (m_pAvFrame == nullptr)
		{
			hr = E_OUTOFMEMORY;
		}
	}

	if (SUCCEEDED(hr))
	{
		if (av_image_alloc(m_rgVideoBufferData, m_rgVideoBufferLineSize, m_pAvCodecCtx->width, m_pAvCodecCtx->height, AV_PIX_FMT_RGB24, 1) < 0)
		{
			hr = E_FAIL;
		}
	}

	return hr;
}

UncompressedVideoSampleProvider::~UncompressedVideoSampleProvider()
{
	if (m_pAvFrame)
	{
		av_freep(m_pAvFrame);
	}

	if (m_rgVideoBufferData)
	{
		av_freep(m_rgVideoBufferData);
	}
}

HRESULT UncompressedVideoSampleProvider::WriteAVPacketToStream(DataWriter^ dataWriter, AVPacket* avPacket)
{
	// Convert decoded video pixel format to NV12 using FFmpeg software scaler
	if (sws_scale(m_pSwsCtx, (const uint8_t **)(m_pAvFrame->data), m_pAvFrame->linesize, 0, m_pAvCodecCtx->height, m_rgVideoBufferData, m_rgVideoBufferLineSize) < 0)
	{
		return E_FAIL;
	}

	char filename[MAX_PATH];
	sprintf_s(filename, OUTPUT_FILE_PREFIX);
	AVFrame *rgb_frame = UncompressedVideoSampleProvider::GetRGBAFrame(m_pAvFrame);
	UncompressedVideoSampleProvider::BMPSave(filename, rgb_frame, rgb_frame->width, rgb_frame->height);

	auto YBuffer = ref new Platform::Array<uint8_t>(m_rgVideoBufferData[0], m_rgVideoBufferLineSize[0] * m_pAvCodecCtx->height);
	auto UVBuffer = ref new Platform::Array<uint8_t>(m_rgVideoBufferData[1], m_rgVideoBufferLineSize[1] * m_pAvCodecCtx->height / 2);

	dataWriter->WriteBytes(YBuffer);
	dataWriter->WriteBytes(UVBuffer);
	av_frame_unref(m_pAvFrame);

	return S_OK;
}

HRESULT UncompressedVideoSampleProvider::DecodeAVPacket(DataWriter^ dataWriter, AVPacket* avPacket)
{
	if (avcodec_send_packet(m_pAvCodecCtx, avPacket) < 0)
	{
		DebugMessage(L"DecodeAVPacket Failed\n");
		return S_FALSE;
	}
	else
	{
		if (avcodec_receive_frame(m_pAvCodecCtx, m_pAvFrame) >= 0)
		{
			avPacket->pts = av_frame_get_best_effort_timestamp(m_pAvFrame);
			return S_OK;
		}
	}

	return S_FALSE;
}

AVFrame *UncompressedVideoSampleProvider::GetRGBAFrame(AVFrame *pFrameYuv)
{

	AVFrame *avFrameRGB = av_frame_alloc();

	struct SwsContext *img_convert_ctx = sws_getContext(
		m_pAvCodecCtx->width,
		m_pAvCodecCtx->height,
		m_pAvCodecCtx->pix_fmt,
		m_pAvCodecCtx->width,
		m_pAvCodecCtx->height,
		AV_PIX_FMT_RGBA,
		SWS_BICUBIC,
		NULL,
		NULL,
		NULL);

	int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_pAvCodecCtx->width, m_pAvCodecCtx->height, 1);
	//int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, m_pAvCodecCtx->width, m_pAvCodecCtx->height);
	uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

	av_image_copy_to_buffer(buffer, numBytes, pFrameYuv->data, pFrameYuv->linesize, AV_PIX_FMT_RGB24, pFrameYuv->width, pFrameYuv->height, NULL);
	//avpicture_fill((AVPicture *)avFrameRGB, buffer, AV_PIX_FMT_RGB24, m_pAvCodecCtx->width, m_pAvCodecCtx->height);
	std::cout << buffer << std::endl;
	sws_scale(img_convert_ctx, pFrameYuv->data, pFrameYuv->linesize, 0, pFrameYuv->height, avFrameRGB->data, avFrameRGB->linesize);

	sws_freeContext(img_convert_ctx);


	//uint8_t * pointers;
	//int * lineSizes = pFrameYuv->linesize;
	//int width = m_pAvCodecCtx->width;
	//int height = m_pAvCodecCtx->height;
	//int bufferImgSize = av_image_alloc(&pointers, lineSizes, width, height, AV_PIX_FMT_RGBA, 0);
	////AVFrame *frame = avcodec_alloc_frame();
	//uint8_t * buffer = (uint8_t*)av_mallocz(bufferImgSize);
	//if (pointers)
	//{
	//	av_image_fill_arrays(&pointers, lineSizes, buffer, AV_PIX_FMT_RGBA, width, height, 0);
	//	/*frame->width = width;
	//	frame->height = height;*/
	//	//frame->data[0] = buffer;

	//	sws_scale(sws_getContext(m_pAvCodecCtx->width, m_pAvCodecCtx->height, m_pAvCodecCtx->pix_fmt, m_pAvCodecCtx->width,
	//		m_pAvCodecCtx->height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL), pFrameYuv->data, pFrameYuv->linesize,
	//		0, height, &pointers, lineSizes);
	//}

	//std::cout << "ptrs: " << pointers[0] << std::endl;

	//return pointers;

	return avFrameRGB;
}

bool UncompressedVideoSampleProvider::BMPSave(const char *pFileName, AVFrame * frame, int w, int h)
{
	bool bResult = false;

	FILE *stream;

	if (frame)
	{
		FILE* file = (FILE *)fopen_s(&stream, pFileName, "wb");
		std::cout << "Opened file" << std::endl;
		if (file)
		{
			// RGB image
			int imageSizeInBytes = 3 * w * h;
			int headersSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			int fileSize = headersSize + imageSizeInBytes;

			uint8_t * pData = new uint8_t[headersSize];

			if (pData != NULL)
			{
				BITMAPFILEHEADER& bfHeader = *((BITMAPFILEHEADER *)(pData));

				bfHeader.bfType = 'MB';
				bfHeader.bfSize = fileSize;
				bfHeader.bfOffBits = headersSize;
				bfHeader.bfReserved1 = bfHeader.bfReserved2 = 0;

				BITMAPINFOHEADER& bmiHeader = *((BITMAPINFOHEADER *)(pData + headersSize - sizeof(BITMAPINFOHEADER)));

				bmiHeader.biBitCount = 3 * 8;
				bmiHeader.biWidth = w;
				bmiHeader.biHeight = h;
				bmiHeader.biPlanes = 1;
				bmiHeader.biSize = sizeof(bmiHeader);
				bmiHeader.biCompression = BI_RGB;
				bmiHeader.biClrImportant = bmiHeader.biClrUsed =
					bmiHeader.biSizeImage = bmiHeader.biXPelsPerMeter =
					bmiHeader.biYPelsPerMeter = 0;

				fwrite(pData, headersSize, 1, file);

				uint8_t *pBits = frame->data[0] + frame->linesize[0] * h - frame->linesize[0];
				int nSpan = -frame->linesize[0];

				int numberOfBytesToWrite = 3 * w;

				for (size_t i = 0; i < h; ++i, pBits += nSpan)
				{
					fwrite(pBits, numberOfBytesToWrite, 1, file);
				}

				bResult = true;
				delete[] pData;
			}

			fclose(file);
		}
	}

	return bResult;
}
