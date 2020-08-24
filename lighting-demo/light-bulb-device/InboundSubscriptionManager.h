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

#ifndef _INBOUND_SUBSCRIPTION_MANAGER_H
#define _INBOUND_SUBSCRIPTION_MANAGER_H

#include <Weave/Core/WeaveError.h>
#include <Weave/Core/WeaveSecurityMgr.h>
#include <Weave/Support/CodeUtils.h>
#include <Weave/Profiles/WeaveProfiles.h>
#include <Weave/Profiles/data-management/DataManagement.h>

class InboundSubscriptionManager
{
public:
    InboundSubscriptionManager();

    WEAVE_ERROR Init();
    nl::Weave::Profiles::DataManagement::SingleResourceSourceTraitCatalog& GetSourceCatalog() {return mSourceTraitCatalog;}

private:
    static void PublisherEventCallback (void * const aAppState,
                                        nl::Weave::Profiles::DataManagement::SubscriptionHandler::EventID aEvent,
                                        const nl::Weave::Profiles::DataManagement::SubscriptionHandler::InEventParam & aInParam,
                                        nl::Weave::Profiles::DataManagement::SubscriptionHandler::OutEventParam & aOutParam);
    
    static void SubscriptionEventCallback(void * const aAppState,
                                          nl::Weave::Profiles::DataManagement::SubscriptionClient::EventID aEvent, const nl::Weave::Profiles::DataManagement::SubscriptionClient::InEventParam & aInParam,
                                          nl::Weave::Profiles::DataManagement::SubscriptionClient::OutEventParam & aOutParam);

    static void BindingEventCallback(void * const apAppState, const nl::Weave::Binding::EventType aEventType,
                                     const nl::Weave::Binding::InEventParam & aInParam,
                                     nl::Weave::Binding::OutEventParam & aOutParam);

    static void EngineEventCallback(void * const aAppState,
                                    nl::Weave::Profiles::DataManagement::SubscriptionEngine::EventID aEvent,
                                    const nl::Weave::Profiles::DataManagement::SubscriptionEngine::InEventParam & aInParam,
                                    nl::Weave::Profiles::DataManagement::SubscriptionEngine::OutEventParam & aOutParam);

private:
    // Source catalog that actually houses sources. 
    nl::Weave::Profiles::DataManagement::SingleResourceSourceTraitCatalog mSourceTraitCatalog;

    // The backing store for the catalog. We need this since the catalog itself doesn't have any dynamic memory allocation constructs.
    nl::Weave::Profiles::DataManagement::SingleResourceSourceTraitCatalog::CatalogItem mSourceTraitCatalogStore[10];
    
};

#endif // _INBOUND_SUBSCRIPTION_MANAGER_H
