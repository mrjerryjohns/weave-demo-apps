/*
 *
 *    Copyright (c) 2020 Google.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *    @file
 *      TraitDataSink that receives data for the ZCL on/off cluster and appropriately
 *      modifies the UI
 */

#include "ZclOnOffClusterSink.h"
#include "ViewController.h"

#include <Debug.h>
#include <sys/time.h>

using namespace nl::Weave::TLV;
using namespace nl::Weave::Profiles;
using namespace nl::Weave::Profiles::DataManagement;

using namespace Schema::Zigbee::Cluster::General::OnOffCluster;

ZclOnOffClusterSink::ZclOnOffClusterSink(ViewController *viewController)
    : TraitDataSink(&TraitSchema),
    _viewController(viewController)
{
    _onOff = false;
    _globalSceneControl = false;
    _onTime = 0;
    _offWaitTime = 0;
}

WEAVE_ERROR ZclOnOffClusterSink::OnEvent(uint16_t aType, void * aInEventParam)
{
    //
    // Wait for the change end event to process all the items that were modified.
    //
    if (aType == kEventChangeEnd) {
        _viewController->ProcessChanges(_changeList);
        _changeList.clear();
    }

    return WEAVE_NO_ERROR;
}

WEAVE_ERROR ZclOnOffClusterSink::SetLeafData(PropertyPathHandle aLeafHandle, TLVReader &reader)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

    switch (aLeafHandle)
    {
        case kPropertyHandle_OnOff:
            err = reader.Get(_onOff);
            nlREQUIRE_SUCCESS(err, exit);

            _changeList["on_off"] = _onOff ? "on" : "off";
            DEBUG_PRINT(kAPP, "<< OnOff = %s", _changeList["on_off"].c_str());
            break;

        case kPropertyHandle_GlobalSceneControl:
            err = reader.Get(_globalSceneControl);
            nlREQUIRE_SUCCESS(err, exit);

            _changeList["global_scene_control"] = _globalSceneControl ? "on" : "off";
            DEBUG_PRINT(kAPP, "<< GlobalSceneControl = %s", _changeList["global_scene_control"].c_str());
            break;

        case kPropertyHandle_OnTime:
            err = reader.Get(_onTime);
            nlREQUIRE_SUCCESS(err, exit);

            _changeList["on_time"] = std::to_string(_onTime);
            DEBUG_PRINT(kAPP, "<< OnTime = %s", _changeList["on_time"].c_str());
            break;
            
        case kPropertyHandle_OffWaitTime:
            err = reader.Get(_offWaitTime);
            nlREQUIRE_SUCCESS(err, exit);

            _changeList["off_wait_time"] = std::to_string(_offWaitTime);
            DEBUG_PRINT(kAPP, "<< OffWaitTime = %s", _changeList["off_wait_time"].c_str());
            break;

        case kPropertyHandle_RampInfo_Type:
            err = reader.Get(_rampInfo.type);
            nlREQUIRE_SUCCESS(err, exit);

            _changeList["ramp_info.type"] = std::to_string(_rampInfo.type);
            DEBUG_PRINT(kAPP, "<< RampInfo.Type = %s", _changeList["ramp_info.type"].c_str());
            break;

        case kPropertyHandle_RampInfo_RampDuration:
            err = reader.Get(_rampInfo.rampDuration);
            nlREQUIRE_SUCCESS(err, exit);

            _changeList["ramp_info.duration"] = std::to_string(_rampInfo.rampDuration);
            DEBUG_PRINT(kAPP, "<< RampInfo.Duration = %s", _changeList["ramp_info.duration"].c_str());
            break;

        default:
            break;
    }

exit:
    return err;
}
