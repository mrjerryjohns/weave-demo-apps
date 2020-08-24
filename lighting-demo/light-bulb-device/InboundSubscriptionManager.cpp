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
 *      This file interacts with the SubscriptionHandler class to manage incoming subscriptions.
 */
#include "InboundSubscriptionManager.h"
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

extern SubscriptionEngine gSubscriptionEngine;

InboundSubscriptionManager::InboundSubscriptionManager()
    : mSourceTraitCatalog(ResourceIdentifier(ResourceIdentifier::SELF_NODE_ID), mSourceTraitCatalogStore, sizeof(mSourceTraitCatalogStore) / sizeof(mSourceTraitCatalogStore[0]))
{
}

WEAVE_ERROR
InboundSubscriptionManager::Init()
{
    WEAVE_ERROR err;
    TraitDataHandle handle;

    err = gSubscriptionEngine.Init(&ExchangeMgr, this, EngineEventCallback);
    SuccessOrExit(err);

    // Enable subscribers to subscribe now.
    err = gSubscriptionEngine.EnablePublisher(NULL, &mSourceTraitCatalog);
    SuccessOrExit(err);

exit:
   return err;
}

void
InboundSubscriptionManager::PublisherEventCallback (void * const aAppState,
        SubscriptionHandler::EventID aEvent, const SubscriptionHandler::InEventParam & aInParam, SubscriptionHandler::OutEventParam & aOutParam)
{
    InboundSubscriptionManager *_this = static_cast<InboundSubscriptionManager*>(aAppState);

    switch (aEvent)
    {
        case SubscriptionHandler::kEvent_OnSubscribeRequestParsed:
        {
            uint64_t subscriberNodeId = aInParam.mSubscribeRequestParsed.mHandler->GetPeerNodeId();

            DEBUG_PRINT(kWDM, "Incoming Subscription from %llx", subscriberNodeId);

            // AcceptSubscribeRequest and EndSubscription may be used either sync or async, to move the state machine forward
            aInParam.mSubscribeRequestParsed.mHandler->AcceptSubscribeRequest(aInParam.mSubscribeRequestParsed.mTimeoutSecMin );

            break;
        }

        case SubscriptionHandler::kEvent_OnExchangeStart:
            DEBUG_PRINT(kWDM, "Publisher->kEvent_OnExchangeStart (%llx)", aInParam.mExchangeStart.mHandler->GetPeerNodeId());
            break;

        case SubscriptionHandler::kEvent_OnSubscriptionEstablished:
        {
            DEBUG_PRINT(kWDM, "Publisher->kEvent_OnSubscriptionEstablished (%llx)", aInParam.mSubscriptionEstablished.mHandler->GetPeerNodeId());
            break;
        }

        case SubscriptionHandler::kEvent_OnSubscriptionTerminated:
        {
            WEAVE_ERROR reason = aInParam.mSubscriptionTerminated.mReason;

            if (reason == WEAVE_ERROR_STATUS_REPORT_RECEIVED)
            {
                ERROR_PRINT
                    (
                     "Publisher->kEvent_OnSubscriptionTerminated (%llx) reason=0x%x profile=0x%x status=0x%x",
                     aInParam.mSubscriptionTerminated.mHandler->GetPeerNodeId(),
                     reason,
                     aInParam.mSubscriptionTerminated.mStatusProfileId,
                     aInParam.mSubscriptionTerminated.mStatusCode
                    );
            }
            else
            {
                ERROR_PRINT
                    (
                     "Publisher->kEvent_OnSubscriptionTerminated (%llx) reason=0x%x",
                     aInParam.mSubscriptionTerminated.mHandler->GetPeerNodeId(),
                     reason
                    );
            }

            break;
        }

        default:
            SubscriptionHandler::DefaultEventHandler(aEvent, aInParam, aOutParam);
            break;
    }
}

void
InboundSubscriptionManager::EngineEventCallback (void * const aAppState,
                                            SubscriptionEngine::EventID aEvent,
                                            const SubscriptionEngine::InEventParam & aInParam,
                                            SubscriptionEngine::OutEventParam & aOutParam)
{
    InboundSubscriptionManager *_this = static_cast<InboundSubscriptionManager*>(aAppState);
    switch (aEvent)
    {
    case SubscriptionEngine::kEvent_OnIncomingSubscribeRequest :
        DEBUG_PRINT(kWDM, "Engine->kEvent_OnIncomingSubscribeRequest");

        aOutParam.mIncomingSubscribeRequest.mHandlerAppState = _this;
        aOutParam.mIncomingSubscribeRequest.mHandlerEventCallback = InboundSubscriptionManager::PublisherEventCallback;
        aOutParam.mIncomingSubscribeRequest.mRejectRequest = false;
        break;

    default:
        SubscriptionEngine::DefaultEventHandler(aEvent, aInParam, aOutParam);
        break;
    }
}
