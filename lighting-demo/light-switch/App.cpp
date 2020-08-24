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
 *      Main application.
 */
#include "Debug.h"

#include <SystemLayer/SystemLayer.h>
#include <InetLayer/InetLayer.h>
#include <Weave/Core/WeaveCore.h>
#include <InetLayer/InetLayer.h>
#include <SystemLayer/SystemFaultInjection.h>
#include <Weave/Support/WeaveFaultInjection.h>
#include <InetLayer/InetFaultInjection.h>

#include "App.h"

using namespace nl::Weave;
using namespace nl::Weave::Profiles::DataManagement;

// When initially sending a message to a peer before they've responded,
// how often we should re-transmit a message.
#define DEVICE_WRM_INITIAL_RETRANS_TIMEOUT_MS       500 

// When the peer has responded, how often to re-transmit a message after that.
#define DEVICE_WRM_ACTIVE_RETRANS_TIMEOUT_MS        500 

#define DEVICE_WRM_MAX_RETRANS                      3

// Define the timeout for piggybacking a WRM acknowledgment message
#define DEVICE_WRM_PIGGYBACK_ACK_TIMEOUT_MS         200

const nl::Weave::WRMPConfig gWRMPConfig = { DEVICE_WRM_INITIAL_RETRANS_TIMEOUT_MS,
                                                          DEVICE_WRM_ACTIVE_RETRANS_TIMEOUT_MS,
                                                          DEVICE_WRM_PIGGYBACK_ACK_TIMEOUT_MS,
                                                          DEVICE_WRM_MAX_RETRANS };


void BindingEventCallback (void * const apAppState,
                           const Binding::EventType aEvent,
                           const Binding::InEventParam & aInParam,
                           Binding::OutEventParam & aOutParam)
{
    switch (aEvent) {
        default:
            Binding::DefaultEventHandler(apAppState, aEvent, aInParam, aOutParam);
    }
}

CommandSender *gCommandSender = NULL;

void CommandEventHandler(void * const appState, CommandSender::EventType event, const CommandSender::InEventParam &inParam, CommandSender::OutEventParam &outEventParam)
{
    switch (event) {
        case CommandSender::kEvent_InProgressReceived:
            DEBUG_PRINT(kAPP, "Received In Progress message. Waiting for a response");
            break;

        case CommandSender::kEvent_StatusReportReceived:
             DEBUG_PRINT(kAPP, "Received Status Report 0x%08x : 0x%04x", inParam.StatusReportReceived.statusReport->mProfileId, inParam.StatusReportReceived.statusReport->mStatusCode);
             break;

        case CommandSender::kEvent_CommunicationError:
             DEBUG_PRINT(kAPP, "Communication Error: %d", inParam.CommunicationError.error);
             break;

        case CommandSender::kEvent_ResponseReceived:
             DEBUG_PRINT(kAPP, "Response message, end");
             break;

        case CommandSender::kEvent_DefaultCheck:
             gCommandSender->DefaultEventHandler(appState, event, inParam, outEventParam);
             break;
    }
}

App::App()
{
}

WEAVE_ERROR
App::Start(GtkApplication *application, uint64_t destinationNodeId)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

    // Create a new view controller to manage our UI/screen. 
    _viewController = new ViewController(this, application);

    // Create the data sinks to handle the incoming data, and appropriately
    // modify the view
    _onOffSink = new ZclOnOffClusterSink(_viewController);

    // Allocate a new binding to use to setup a conversation with the bulb
    _binding = ExchangeMgr.NewBinding(BindingEventCallback, NULL);

    // Initialize the binding with the details of the transport we want, which includes
    // UDP + Weave Reliable Messaging.
    _binding->BeginConfiguration()
        .Target_NodeId(destinationNodeId)
        .TargetAddress_WeaveFabric(0x1)
        .Transport_UDP_WRM()
        .Transport_DefaultWRMPConfig(gWRMPConfig)
        .Security_None()
        .Exchange_ResponseTimeoutMsec(2000)
        .PrepareBinding();

    // Start a subscription to the bulb for the on/off cluster
    _subscriptionManager = new OutboundSubscriptionManager(_binding);
    _subscriptionManager->AddSink(_onOffSink);
    _subscriptionManager->Start();

    err = _commandSender.Init(_binding, CommandEventHandler, this);
    SuccessOrExit(err);

exit:
    return err;
}

gboolean App::ToggleBulbState(void *data)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;
    CommandSender::SendParams sendParams;
    App *_this = static_cast<App *>(data);

    DEBUG_PRINT(kAPP, "Toggling Light State...");
    
    memset(&sendParams, 0, sizeof(sendParams));

    // We're targeting the on/off trait instance on the remote device, which is the same that our on/off sink is tracking. There is
    // a convenience method here to auto populate the path based on the sink itself.
    err = sendParams.PopulateTraitPath(&_this->_subscriptionManager->GetCatalog(), _this->_onOffSink, kRootPropertyPathHandle);
    SuccessOrExit(err);

    // The command ID is set to the toggle command.
    sendParams.CommandType = Schema::Zigbee::Cluster::General::OnOffCluster::kToggleRequestId;

    // This command has no payload. Now, send it!
    err = _this->_commandSender.SendCommand(NULL, NULL, sendParams);
    SuccessOrExit(err);

exit:
    WeaveLogFunctError(err);
    return true;
}
