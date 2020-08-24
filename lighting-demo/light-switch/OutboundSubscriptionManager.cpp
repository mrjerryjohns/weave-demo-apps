/*
 *
 *    Copyright (c) 2020 Google.
 *    All rights reserved.
 *
 *    This document is the property of Google. It is considered
 *    confidential and proprietary information.
 *
 *    This document may not be reproduced or transmitted in any form,
 *    in whole or in part, without the express written permission of
 *    Google.
 *
 *
 */

/**
 *    @file
 *      Handles outbound subscriptions through the SubscriptionClient object.
 */

#include "OutboundSubscriptionManager.h"
#include "Debug.h"

#include <SystemLayer/SystemLayer.h>
#include <InetLayer/InetLayer.h>
#include <Weave/Core/WeaveCore.h>
#include <InetLayer/InetLayer.h>
#include <SystemLayer/SystemFaultInjection.h>
#include <Weave/Support/WeaveFaultInjection.h>
#include <InetLayer/InetFaultInjection.h>

using namespace nl::Weave;
using namespace nl::Weave::Profiles::DataManagement;

OutboundSubscriptionManager::OutboundSubscriptionManager(Binding *binding)
    : mSinkTraitCatalog(ResourceIdentifier(ResourceIdentifier::SELF_NODE_ID), mSinkTraitCatalogStore, sizeof(mSinkTraitCatalogStore) / sizeof(mSinkTraitCatalogStore[0]))
{
    mSubscriptionClient = NULL;
    mBinding = binding;

    mNumTraits = 0;
    mFirstTime = true;
}

WEAVE_ERROR
OutboundSubscriptionManager::AddSink(TraitDataSink *dataSink)
{
    WEAVE_ERROR err = mSinkTraitCatalog.Add(mNumTraits, dataSink, mTraitDataHandleList[mNumTraits]);
    mNumTraits++;
    return err;
}

void
OutboundSubscriptionManager::TearDownSubscription()
{
    if (mSubscriptionClient) {
        mSubscriptionClient->Free();
        mSubscriptionClient = NULL;
    }
}

WEAVE_ERROR
OutboundSubscriptionManager::Start()
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

    // Clean up old subscriptions before initiating a new one.
    TearDownSubscription();

    err = SubscriptionEngine::GetInstance()->NewClient(&mSubscriptionClient,
        mBinding,
        this,
        SubscriptionEventCallback,
        &mSinkTraitCatalog,
        4000);
    SuccessOrExit(err);

    mSubscriptionClient->EnableResubscribe(ResubscribePolicyCallback);
    mSubscriptionClient->InitiateSubscription();

exit:
    return err;
}

void
OutboundSubscriptionManager::ResubscribePolicyCallback(void * const aAppState, SubscriptionClient::ResubscribeParam & aInParam, uint32_t & aOutIntervalMsec)
{
    OutboundSubscriptionManager *_this = static_cast<OutboundSubscriptionManager *>(aAppState);

    if (_this->mFirstTime) {
        aOutIntervalMsec = 0;
        _this->mFirstTime = false;
    }
    else {
        aOutIntervalMsec = 3000;
    }

    DEBUG_PRINT(kWDM, "Resubscribing in %d seconds", aOutIntervalMsec / 1000);
}

void
OutboundSubscriptionManager::SubscriptionEventCallback (void * const aAppState,
                                        SubscriptionClient::EventID aEvent,
                                        const SubscriptionClient::InEventParam & aInParam,
                                        SubscriptionClient::OutEventParam & aOutParam)
{
    OutboundSubscriptionManager *_this = static_cast<OutboundSubscriptionManager*>(aAppState);

    switch (aEvent) {
        case SubscriptionClient::kEvent_OnExchangeStart:
            DEBUG_PRINT(kWDM, "Client->kEvent_OnExchangeStart");
            break;

        case SubscriptionClient::kEvent_OnSubscribeRequestPrepareNeeded:
        {
            SubscriptionClient* client = aInParam.mSubscribeRequestPrepareNeeded.mClient;
            TraitPath *traitPathSet = new TraitPath[_this->mNumTraits];

            DEBUG_PRINT(kWDM, "Client->kEvent_OnSubscribeRequestPrepareNeeded");

            for (int i = 0; i < _this->mNumTraits; i++) {
                traitPathSet[i].mTraitDataHandle = _this->mTraitDataHandleList[i];
                traitPathSet[i].mPropertyPathHandle = kRootPropertyPathHandle;
            }

            aOutParam.mSubscribeRequestPrepareNeeded.mPathList = traitPathSet;
            aOutParam.mSubscribeRequestPrepareNeeded.mPathListSize = _this->mNumTraits;

            aOutParam.mSubscribeRequestPrepareNeeded.mTimeoutSecMin = 3;
            aOutParam.mSubscribeRequestPrepareNeeded.mTimeoutSecMax = 3;

            aOutParam.mSubscribeRequestPrepareNeeded.mNeedAllEvents = false;
            aOutParam.mSubscribeRequestPrepareNeeded.mLastObservedEventList = NULL;
            aOutParam.mSubscribeRequestPrepareNeeded.mLastObservedEventListSize = 0;
            break;
        }

        case SubscriptionClient::kEvent_OnSubscriptionEstablished:
        {
            SubscriptionClient* client = aInParam.mSubscriptionEstablished.mClient;
            DEBUG_PRINT(kWDM, "Client->kEvent_OnSubscriptionEstablished");
            break;
        }

        case SubscriptionClient::kEvent_OnNotificationRequest:
            DEBUG_PRINT(kWDM, "Client->kEvent_OnNotificationRequest");
            break;

        case SubscriptionClient::kEvent_OnNotificationProcessed:
            DEBUG_PRINT(kWDM, "Client->kEvent_OnNotificationProcessed");
            break;

        case SubscriptionClient::kEvent_OnSubscriptionTerminated:
        {
            WEAVE_ERROR reason = aInParam.mSubscriptionTerminated.mReason;
            SubscriptionClient *client = aInParam.mSubscriptionTerminated.mClient;

            if (reason == WEAVE_ERROR_STATUS_REPORT_RECEIVED)
            {
                ERROR_PRINT( 
                    "Client->kEvent_OnSubscriptionTerminated reason=0x%x profile=0x%x status=0x%x",
                    reason,
                    aInParam.mSubscriptionTerminated.mStatusProfileId,
                    aInParam.mSubscriptionTerminated.mStatusCode
                );
            }
            else {
                ERROR_PRINT( 
                    "Client->kEvent_OnSubscriptionTerminated reason=0x%x",
                    reason
                    );
            }

            if (!aInParam.mSubscriptionTerminated.mWillRetry)
            {
                _this->Start();
            }
            
            break;
        }

        case SubscriptionClient::kEvent_OnSubscriptionActivity:
        {
            DEBUG_PRINT(kWDM, "Client->kEvent_OnSubscriptionActivity");
            break;
        }

        default:
            DEBUG_PRINT(kWDM, "Unknown SubscriptionClient event: %d", aEvent);
            SubscriptionClient::DefaultEventHandler(aEvent, aInParam, aOutParam);
            break;
    }
}
