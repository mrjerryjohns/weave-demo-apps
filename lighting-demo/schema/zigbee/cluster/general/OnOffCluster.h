
/*
 *    Copyright (c) 2019-2020 Google LLC.
 *    Copyright (c) 2016-2018 Nest Labs, Inc.
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

/*
 *    THIS FILE IS GENERATED. DO NOT MODIFY.
 *
 *    SOURCE TEMPLATE: trait.cpp.h
 *    SOURCE PROTO: zigbee/cluster/general/on_off_cluster.proto
 *
 */
#ifndef _ZIGBEE_CLUSTER_GENERAL__ON_OFF_CLUSTER_H_
#define _ZIGBEE_CLUSTER_GENERAL__ON_OFF_CLUSTER_H_

#include <Weave/Profiles/data-management/DataManagement.h>
#include <Weave/Support/SerializationUtils.h>



namespace Schema {
namespace Zigbee {
namespace Cluster {
namespace General {
namespace OnOffCluster {

extern const nl::Weave::Profiles::DataManagement::TraitSchemaEngine TraitSchema;

enum {
      kWeaveProfileId = (0x1U << 16) | 0x6U
};

//
// Properties
//

enum {
    kPropertyHandle_Root = 1,

    //---------------------------------------------------------------------------------------------------------------------------//
    //  Name                                IDL Type                            TLV Type           Optional?       Nullable?     //
    //---------------------------------------------------------------------------------------------------------------------------//

    //
    //  on_off                              bool                                 bool              NO              NO
    //
    kPropertyHandle_OnOff = 2,

    //
    //  global_scene_control                bool                                 bool              NO              NO
    //
    kPropertyHandle_GlobalSceneControl = 3,

    //
    //  on_time                             uint32                               uint16            NO              NO
    //
    kPropertyHandle_OnTime = 4,

    //
    //  off_wait_time                       uint32                               uint16            NO              NO
    //
    kPropertyHandle_OffWaitTime = 5,

    //
    //  ramp_info                           RampInfo                             structure         NO              NO
    //
    kPropertyHandle_RampInfo = 6,

    //
    //  ramp_duration                       uint32                               uint32            NO              NO
    //
    kPropertyHandle_RampInfo_RampDuration = 7,

    //
    //  type                                RampType                             int               NO              NO
    //
    kPropertyHandle_RampInfo_Type = 8,

    //
    // Enum for last handle
    //
    kLastSchemaHandle = 8,
};

//
// Event Structs
//

struct RampInfo
{
    uint32_t rampDuration;
    int32_t type;

    static const nl::SchemaFieldDescriptor FieldSchema;

};

struct RampInfo_array {
    uint32_t num;
    RampInfo *buf;
};

//
// Commands
//

enum {
    kToggleRequestId = 0x2,
};

//
// Enums
//

enum RampType {
    RAMP_TYPE_SLOW = 1,
    RAMP_TYPE_FAST = 2,
};

} // namespace OnOffCluster
} // namespace General
} // namespace Cluster
} // namespace Zigbee
} // namespace Schema
#endif // _ZIGBEE_CLUSTER_GENERAL__ON_OFF_CLUSTER_H_
