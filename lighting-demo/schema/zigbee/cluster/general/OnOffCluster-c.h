
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
 *    SOURCE TEMPLATE: trait.c.h
 *    SOURCE PROTO: zigbee/cluster/general/on_off_cluster.proto
 *
 */
#ifndef _ZIGBEE_CLUSTER_GENERAL__ON_OFF_CLUSTER_C_H_
#define _ZIGBEE_CLUSTER_GENERAL__ON_OFF_CLUSTER_C_H_



    //
    // Commands
    //

    typedef enum
    {
      kToggleRequestId = 0x2,
    } schema_zigbee_cluster_general_on_off_cluster_command_id_t;





    //
    // Enums
    //

    // RampType
    typedef enum
    {
    RAMP_TYPE_SLOW = 1,
    RAMP_TYPE_FAST = 2,
    } schema_zigbee_cluster_general_on_off_cluster_ramp_type_t;




#endif // _ZIGBEE_CLUSTER_GENERAL__ON_OFF_CLUSTER_C_H_
