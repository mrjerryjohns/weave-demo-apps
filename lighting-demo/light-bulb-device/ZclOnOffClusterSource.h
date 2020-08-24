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

#ifndef ZCL_ON_OFF_CLUSTER_SOURCE
#define ZCL_ON_OFF_CLUSTER_SOURCE

#include <Weave/Profiles/data-management/DataManagement.h>
#include <zigbee/cluster/general/OnOffCluster.h>

#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

class ViewController;

class ZclOnOffClusterSource : public nl::Weave::Profiles::DataManagement::TraitDataSource
{
public:
    ZclOnOffClusterSource();
    void SetViewController(ViewController *viewController) { _viewController = viewController; }

protected:
    WEAVE_ERROR GetLeafData(nl::Weave::Profiles::DataManagement::PropertyPathHandle aLeafHandle, uint64_t aTagToWrite, nl::Weave::TLV::TLVWriter &aWriter) override;

    void OnCustomCommand(nl::Weave::Profiles::DataManagement::Command *command,
            const nl::Weave::WeaveMessageInfo *msgInfo,
            nl::Weave::PacketBuffer *payload,
            const uint64_t &commandType,
            const bool isExpiryTimeValid,
            const int64_t &expiryTimeMicroSecond,
            const bool isMustBeVersionValid,
            const uint64_t &muustBeVersion,
            nl::Weave::TLV::TLVReader &argumentReader) override;
    
private:
    friend class ViewController;

    bool _onOff;
    bool _globalSceneControl;
    uint16_t _onTime;
    uint16_t _offWaitTime;
    Schema::Zigbee::Cluster::General::OnOffCluster::RampInfo _rampInfo;

    ViewController *_viewController;
};

#endif
