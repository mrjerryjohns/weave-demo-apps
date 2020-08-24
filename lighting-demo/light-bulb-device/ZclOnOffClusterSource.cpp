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
 *      The main application implementation of the ZCL On/Off cluster as a data source.
 */

#include "ZclOnOffClusterSource.h"
#include "ViewController.h"

#include <Debug.h>
#include <sys/time.h>

using namespace nl::Weave::TLV;
using namespace nl::Weave::Profiles;
using namespace nl::Weave::Profiles::DataManagement;

using namespace Schema::Zigbee::Cluster::General::OnOffCluster;

ZclOnOffClusterSource::ZclOnOffClusterSource()
    : TraitDataSource(&TraitSchema)
{
    _onOff = false;
    _globalSceneControl = true;
    _onTime = 30;
    _offWaitTime = 100;

    _rampInfo.type = RAMP_TYPE_FAST;
    _rampInfo.rampDuration = 60;
}

WEAVE_ERROR ZclOnOffClusterSource::GetLeafData(PropertyPathHandle aLeafHandle, uint64_t aTagToWrite, TLVWriter & aWriter)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

    switch (aLeafHandle)
    {
        case kPropertyHandle_OnOff:
            err = aWriter.PutBoolean(aTagToWrite, _onOff);
            nlREQUIRE_SUCCESS(err, exit);

            DEBUG_PRINT(kAPP, ">> OnOff = %s", _onOff ? "on" : "off");
            break;

        case kPropertyHandle_GlobalSceneControl:
            err = aWriter.PutBoolean(aTagToWrite, _globalSceneControl);
            nlREQUIRE_SUCCESS(err, exit);

            DEBUG_PRINT(kAPP, ">> GlobalSceneControl = %s", _globalSceneControl ? "on" : "off");
            break;

        case kPropertyHandle_OnTime:
            err = aWriter.Put(aTagToWrite, _onTime);
            nlREQUIRE_SUCCESS(err, exit);

            DEBUG_PRINT(kAPP, ">> OnTime = %d", _onTime);
            break;
            
        case kPropertyHandle_OffWaitTime:
            err = aWriter.Put(aTagToWrite, _offWaitTime);
            nlREQUIRE_SUCCESS(err, exit);

            DEBUG_PRINT(kAPP, ">> OffWaitTime = %d", _offWaitTime);
            break;

        case kPropertyHandle_RampInfo_Type:
            err = aWriter.Put(aTagToWrite, _rampInfo.type);
            nlREQUIRE_SUCCESS(err, exit);

            DEBUG_PRINT(kAPP, ">> RampInfo.Type = %d", _rampInfo.type);
            break;

        case kPropertyHandle_RampInfo_RampDuration:
            err = aWriter.Put(aTagToWrite, _rampInfo.rampDuration);
            nlREQUIRE_SUCCESS(err, exit);

            DEBUG_PRINT(kAPP, ">> RampInfo.Duration = %u", _rampInfo.rampDuration);
            break;

        default:
            break;
    }

exit:
    return err;
}

void
ZclOnOffClusterSource::OnCustomCommand(Command *command,
            const nl::Weave::WeaveMessageInfo *msgInfo,
            nl::Weave::PacketBuffer *payload,
            const uint64_t &commandType,
            const bool isExpiryTimeValid,
            const int64_t &expiryTimeMicroSecond,
            const bool isMustBeVersionValid,
            const uint64_t &muustBeVersion,
            TLVReader &argumentReader)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

    switch (commandType) {
        case kToggleRequestId:
        {
            // Invoke the same method on the view controller that would have been invoked had the user
            // directly pressed on the bulb (i.e toggle it).
            if (_viewController) {
                _viewController->BulbPressed(NULL, NULL, _viewController);
            }

            err = command->SendError(nl::Weave::Profiles::kWeaveProfile_Common, nl::Weave::Profiles::Common::kStatus_Success, err);
            SuccessOrExit(err);

            command = NULL;
        }

        break;
    }

exit:
    WeaveLogFunctError(err);

    if (err != WEAVE_NO_ERROR && command) {
        command->SendError(nl::Weave::Profiles::kWeaveProfile_Common, nl::Weave::Profiles::Common::kStatus_InternalError, err);
    }
}
