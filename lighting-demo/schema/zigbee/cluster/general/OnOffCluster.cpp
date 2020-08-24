
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
 *    SOURCE TEMPLATE: trait.cpp
 *    SOURCE PROTO: zigbee/cluster/general/on_off_cluster.proto
 *
 */

#include <zigbee/cluster/general/OnOffCluster.h>

namespace Schema {
namespace Zigbee {
namespace Cluster {
namespace General {
namespace OnOffCluster {

using namespace ::nl::Weave::Profiles::DataManagement;

//
// Property Table
//

const TraitSchemaEngine::PropertyInfo PropertyMap[] = {
    { kPropertyHandle_Root, 1 }, // on_off
    { kPropertyHandle_Root, 2 }, // global_scene_control
    { kPropertyHandle_Root, 3 }, // on_time
    { kPropertyHandle_Root, 4 }, // off_wait_time
    { kPropertyHandle_Root, 5 }, // ramp_info
    { kPropertyHandle_RampInfo, 1 }, // ramp_duration
    { kPropertyHandle_RampInfo, 2 }, // type
};

//
// Schema
//

const TraitSchemaEngine TraitSchema = {
    {
        kWeaveProfileId,
        PropertyMap,
        sizeof(PropertyMap) / sizeof(PropertyMap[0]),
        2,
#if (TDM_EXTENSION_SUPPORT) || (TDM_VERSIONING_SUPPORT)
        2,
#endif
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
#if (TDM_EXTENSION_SUPPORT)
        NULL,
#endif
#if (TDM_VERSIONING_SUPPORT)
        NULL,
#endif
    }
};

//
// Event Structs
//

const nl::FieldDescriptor RampInfoFieldDescriptors[] =
{
    {
        NULL, offsetof(RampInfo, rampDuration), SET_TYPE_AND_FLAGS(nl::SerializedFieldTypeUInt32, 0), 1
    },

    {
        NULL, offsetof(RampInfo, type), SET_TYPE_AND_FLAGS(nl::SerializedFieldTypeInt32, 0), 2
    },

};

const nl::SchemaFieldDescriptor RampInfo::FieldSchema =
{
    .mNumFieldDescriptorElements = sizeof(RampInfoFieldDescriptors)/sizeof(RampInfoFieldDescriptors[0]),
    .mFields = RampInfoFieldDescriptors,
    .mSize = sizeof(RampInfo)
};

} // namespace OnOffCluster
} // namespace General
} // namespace Cluster
} // namespace Zigbee
} // namespace Schema
