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

#ifndef ZCL_ON_OFF_CLUSTER_SINK
#define ZCL_ON_OFF_CLUSTER_SINK

#include <Weave/Profiles/data-management/DataManagement.h>
#include <zigbee/cluster/general/OnOffCluster.h>

#include <string>
#include <map>

#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

class ViewController;

class ZclOnOffClusterSink : public nl::Weave::Profiles::DataManagement::TraitDataSink
{
public:
    ZclOnOffClusterSink(ViewController *controller);

protected:
    WEAVE_ERROR SetLeafData(nl::Weave::Profiles::DataManagement::PropertyPathHandle aLeafHandle, nl::Weave::TLV::TLVReader & aReader) override;
    WEAVE_ERROR OnEvent(uint16_t event, void * aInEventParam) override;

private:
    ViewController *_viewController;
    bool _onOff;
    bool _globalSceneControl;
    uint16_t _onTime;
    uint16_t _offWaitTime;
    Schema::Zigbee::Cluster::General::OnOffCluster::RampInfo _rampInfo;

    std::map<std::string, std::string> _changeList;
};

#endif
