/*  Copyright (c) MIPoPS. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-3-Clause license that can
 *  be found in the LICENSE.txt file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#pragma once
#include "Common/Config.h"
#ifdef MEDIAINFO_DLL
    #include "MediaInfoDLL/MediaInfoDLL.h"
    #define MediaInfoNameSpace MediaInfoDLL
#elif defined MEDIAINFO_STATIC
    #include "MediaInfoDLL/MediaInfoDLL_Static.h"
    #define MediaInfoNameSpace MediaInfoDLL
#else
    #include "MediaInfo/MediaInfoList.h"
    #define MediaInfoNameSpace MediaInfoLib
#endif
#ifndef DISABLE_SIMULATOR
    #define ENABLE_SIMULATOR
#endif
#include "Common/Merge.h"
#include "TimeCode.h"
#include "MediaInfo/MediaInfo_Events.h"
#include <chrono>
#include <vector>
using namespace MediaInfoNameSpace;
using namespace std;

#if defined(ENABLE_AVFCTL) || defined(ENABLE_SIMULATOR)
#include "Common/ProcessFileWrapper.h"
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void UpdateDynamicDisplay(const string& Content = {});
string MediaInfo_Version();
//---------------------------------------------------------------------------

//***************************************************************************
// Enums
//***************************************************************************

#if defined(ENABLE_AVFCTL) || defined(ENABLE_SIMULATOR)
enum rewind_mode {
    Rewind_Mode_None,
    Rewind_Mode_TimeCode,
    Rewind_Mode_TimeCode2,
    Rewind_Mode_Abst,
};
#endif

//***************************************************************************
// Class file
//***************************************************************************

class file
{
public:
    MediaInfo MI;
    std::vector<MediaInfo_Event_DvDif_Change_0*> PerChange;
    std::vector<MediaInfo_Event_DvDif_Analysis_Frame_1*> PerFrame;
    struct caption_fielddata
    {
        caption_fielddata(const MediaInfo_int8u* NewData)
        {
            Data[0] = NewData[0];
            Data[1] = NewData[1];
        }

        int compare(const MediaInfo_int8u* NewData)
        {
            if (Data[0] < NewData[0])
                return -1;
            if (Data[0] > NewData[0])
                return 1;
            if (Data[1] < NewData[1])
                return -1;
            if (Data[1] > NewData[1])
                return 1;
            return 0;
        }

        uint8_t Data[2];
    };
    struct captions_fielddata
    {
        captions_fielddata(MediaInfo_int64u NewStartFrameNumber) :
            StartFrameNumber(NewStartFrameNumber)
        {}
        MediaInfo_int64u StartFrameNumber;
        vector<caption_fielddata> Captions;
    };
    struct captions_data
    {
        double PTS;
        double DUR;
        vector<captions_fielddata> FieldData[2];
    };
    vector<captions_data> PerFrame_Captions_PerSeq_PerField;
    size_t FrameNumber = 0;
    double FrameRate = 0;

    file();
    ~file();

    void Parse(const String& FileName);
    void Parse_Buffer(const uint8_t* Buffer, size_t Buffer_Size);

    bool TransportControlsSupported();
    #if defined(ENABLE_AVFCTL) || defined(ENABLE_SIMULATOR)
    void RewindToTimeCode(TimeCode TC);
    void RewindToAbst(int Abst);
    #endif
    void AddChange(const MediaInfo_Event_DvDif_Change_0* FrameData);
    void AddFrameAnalysis(const MediaInfo_Event_DvDif_Analysis_Frame_1* FrameData);
    void AddFrameData(const MediaInfo_Event_Global_Demux_4* FrameData);

    // Merge
    void Merge_Finish() { Merge.Finish(); }

private:
    size_t Merge_FilePos;
    dv_merge Merge;
    bool no_sourceorcontrol_aud_set_in_first_frame = false;

    #if defined(ENABLE_AVFCTL) || defined(ENABLE_SIMULATOR)
public:
    BaseWrapper* Controller = nullptr;
    float Speed_Before = 0;
    float Speed_After = 0;
    bool PauseRequested = false;
    rewind_mode RewindMode = Rewind_Mode_None;
    TimeCode RewindTo_TC;
    int RewindTo_Abst = 0;
    int DelayedPlay = 0;
    #endif
};
