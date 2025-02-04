/*  Copyright (c) MIPoPS. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-3-Clause license that can
 *  be found in the LICENSE.txt file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "Common/Core.h"
#include "Common/ProcessFile.h"
#include "Common/Output_Captions_Decode.h"
#include "Common/Output_Captions_Scc.h"
#include "Common/Output_Xml.h"
#include "Common/Output_Webvtt.h"
#include "ZenLib/Ztring.h"
#include <future>
#include <mutex>
#include <thread>
using namespace ZenLib;
using namespace std;
//---------------------------------------------------------------------------

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Core::Core()
{
    MediaInfo::Option_Static(__T("ParseSpeed"), __T("1.000"));
    MediaInfo::Option_Static(__T("Demux"), __T("container"));
}

Core::~Core()
{
    PerFile_Clear();
}

//***************************************************************************
// Process
//***************************************************************************

//---------------------------------------------------------------------------
vector<file*>   PerFile;
return_value Core::Process()
{
    // Analyze files (asynchronous)
    PerFile_Clear();
    PerFile.reserve(Inputs.size());
    std::vector<std::future<size_t>> futures;
    for (const auto& Input : Inputs)
    {
        PerFile.push_back(new file());
        futures.emplace_back(std::async(std::launch::async, [](size_t param, const String& Input) {
            PerFile[param]->Parse(Input);
            return param;
            }, PerFile.size()-1, Input));
    }
    for (auto &future : futures) {
       future.get();
    }
    if (Device_Command)
        return ReturnValue_OK;
    if (Merge_Out)
    {
        PerFile[0]->Merge_Finish();
        if (!XmlFile)
            return ReturnValue_OK;
    }

    // Set output defaults
    return_value ToReturn = ReturnValue_OK;
    if (!XmlFile)
        XmlFile = Out;

    // XML
    if (XmlFile)
    {
        if (auto ToReturn2 = Output_Xml(*XmlFile, PerFile, Options, Err))
            ToReturn = ToReturn2;
    }

    // WebVTT
    if (WebvttFile)
    {
        if (auto ToReturn2 = Output_Webvtt(*WebvttFile, PerFile, Err))
            ToReturn = ToReturn2;
    }

    // Closed Captions
    if (!CaptionsFileNames.empty())
    {
        // SCC
        auto SccFileName = CaptionsFileNames.find(Caption_Scc);
        if (SccFileName != CaptionsFileNames.end())
        {
            if (auto ToReturn2 = Output_Captions_Scc(SccFileName->second, OffsetTimeCode, PerFile, Err))
                ToReturn = ToReturn2;
        }

        // Decode (Screen or SRT)
        auto ScreenFileName = CaptionsFileNames.find(Caption_Screen);
        auto SrtFileName = CaptionsFileNames.find(Caption_Srt);
        if (ScreenFileName != CaptionsFileNames.end() || SrtFileName != CaptionsFileNames.end())
        {
            if (auto ToReturn2 = Output_Captions_Decode(ScreenFileName != CaptionsFileNames.end() ? ScreenFileName->second : string(), SrtFileName != CaptionsFileNames.end() ? SrtFileName->second : string(), PerFile, Err))
                ToReturn = ToReturn2;
        }
    }

    return ToReturn;
}

//---------------------------------------------------------------------------
float Core::State ()
{
    size_t Total = 0;
    for (const auto& File : PerFile)
        Total += File->MI.State_Get();
    return (((float)Total)/PerFile.size()/10000);
}

//***************************************************************************
// PerFile
//***************************************************************************

//---------------------------------------------------------------------------
void Core::PerFile_Clear()
{
    for (const auto& File : PerFile)
        delete File;
    PerFile.clear();
}
