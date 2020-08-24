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

#ifndef _OUTBOUND_SUBSCRIPTION_MANAGER
#define _OUTBOUND_SUBSCRIPTION_MANAGER

#include <Weave/Core/WeaveError.h>
#include <Weave/Core/WeaveSecurityMgr.h>
#include <Weave/Support/CodeUtils.h>
#include <Weave/Profiles/WeaveProfiles.h>
#include <Weave/Profiles/data-management/DataManagement.h>

class OutboundSubscriptionManager {
public:
    OutboundSubscriptionManager(nl::Weave::Binding *binding);

    WEAVE_ERROR Start();
    void TearDownSubscription();
    WEAVE_ERROR AddSink(nl::Weave::Profiles::DataManagement::TraitDataSink *dataSink);

    nl::Weave::Profiles::DataManagement::SingleResourceSinkTraitCatalog &GetCatalog() { return mSinkTraitCatalog; }
    
private:
    static void SubscriptionEventCallback(void * const aAppState,
                                          nl::Weave::Profiles::DataManagement::SubscriptionClient::EventID aEvent, const nl::Weave::Profiles::DataManagement::SubscriptionClient::InEventParam & aInParam,
                                          nl::Weave::Profiles::DataManagement::SubscriptionClient::OutEventParam & aOutParam);

    static void BindingEventCallback(void * const apAppState, const nl::Weave::Binding::EventType aEventType,
                                     const nl::Weave::Binding::InEventParam & aInParam,
                                     nl::Weave::Binding::OutEventParam & aOutParam);

    // Client
    nl::Weave::Profiles::DataManagement::SubscriptionClient *mSubscriptionClient;

private:
    static void ResubscribePolicyCallback(void * const aAppState,
                                                nl::Weave::Profiles::DataManagement::SubscriptionClient::ResubscribeParam & aInParam,
                                                uint32_t & aOutIntervalMsec);
    
    // Sink Catalog
    nl::Weave::Profiles::DataManagement::SingleResourceSinkTraitCatalog mSinkTraitCatalog;
    nl::Weave::Profiles::DataManagement::SingleResourceSinkTraitCatalog::CatalogItem mSinkTraitCatalogStore[10];
    nl::Weave::Profiles::DataManagement::TraitDataHandle mTraitDataHandleList[10];
    uint32_t mNumTraits;
    bool mFirstTime;

    // Binding
    nl::Weave::Binding* mBinding;
};

#endif // _OUTBOUND_SUBSCRIPTION_MANAGER
