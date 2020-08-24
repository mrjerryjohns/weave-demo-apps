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
 *      This file contains code to setup and initialize the Weave stack.
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "Debug.h"

#include <SystemLayer/SystemTimer.h>
#include <SystemLayer/SystemFaultInjection.h>
#include <Weave/Support/WeaveFaultInjection.h>
#include <InetLayer/InetFaultInjection.h>
#include <InetLayer/InetLayer.h>
#include <SystemLayer/SystemFaultInjection.h>
#include <Weave/Support/WeaveFaultInjection.h>
#include <InetLayer/InetFaultInjection.h>
#include <nlfaultinjection.hpp>

#include <Weave/WeaveVersion.h>
#include <SystemLayer/SystemLayer.h>
#include <InetLayer/InetLayer.h>
#include <Weave/Core/WeaveCore.h>
#include <Weave/Support/CodeUtils.h>
#include <Weave/Support/ErrorStr.h>
#include <Weave/Core/WeaveStats.h>

#include <Weave/Support/nlargparser.hpp>
#include <Weave/Profiles/data-management/DataManagement.h>
#include <Weave/Profiles/device-description/DeviceDescription.h>
#include <Weave/Profiles/vendor/nestlabs/device-description/NestProductIdentifiers.hpp>

#include <Weave/Common/ResourceTypeEnum.h>

#include <gio/gio.h>

// using namespace nl::ArgParser;
using namespace nl::Inet;
using namespace nl::Weave;
using namespace nl::Weave::Profiles;
using namespace nl;
using nl::Weave::Profiles::DeviceDescription::WeaveDeviceDescriptor;

// Subscription Engine
nl::Weave::Profiles::DataManagement_Current::SubscriptionEngine gSubscriptionEngine;

nl::Weave::Profiles::DataManagement::SubscriptionEngine * nl::Weave::Profiles::DataManagement::SubscriptionEngine::GetInstance()
{
    return &gSubscriptionEngine;
}

System::Layer SystemLayer;
InetLayer InetLayerObj;

void SetupGioChannels();

void InitNetwork()
{
    void *lContext = NULL;
    InetLayerObj.Init(SystemLayer, lContext);
}

void InitSystemLayer()
{
    SystemLayer.Init(NULL);
}

void HandleConnectionClosed(WeaveConnection *con, WEAVE_ERROR conErr)
{
    char ipAddrStr[64];
    con->PeerAddr.ToString(ipAddrStr, sizeof(ipAddrStr));

    if (conErr == WEAVE_NO_ERROR)
        printf("Connection closed to node %" PRIX64 " (%s)\n", con->PeerNodeId, ipAddrStr);
    else
        printf("Connection ABORTED to node %" PRIX64 " (%s): %s\n", con->PeerNodeId, ipAddrStr, ErrorStr(conErr));

    con->Close();
    //SetupGioChannels();
}

void HandleConnectionReceived(WeaveMessageLayer *msgLayer, WeaveConnection *con)
{
    char ipAddrStr[64];
    con->PeerAddr.ToString(ipAddrStr, sizeof(ipAddrStr));

    printf("Connection received from node %" PRIX64 " (%s)\n", con->PeerNodeId, ipAddrStr);

    con->OnConnectionClosed = HandleConnectionClosed;
    //SetupGioChannels();
}

void HandleSecureSessionEstablished(WeaveSecurityManager *sm, WeaveConnection *con, void *reqState, uint16_t sessionKeyId, uint64_t peerNodeId, uint8_t encType)
{
    char ipAddrStr[64] = "";

    if (con)
        con->PeerAddr.ToString(ipAddrStr, sizeof(ipAddrStr));

    printf("Secure session established with node %" PRIX64 " (%s)\n", peerNodeId, ipAddrStr);
}

void HandleSecureSessionError(WeaveSecurityManager *sm, WeaveConnection *con, void *reqState, WEAVE_ERROR localErr, uint64_t peerNodeId, StatusReport *statusReport)
{
    char ipAddrStr[64] = "";

    if (con)
    {
        con->PeerAddr.ToString(ipAddrStr, sizeof(ipAddrStr));
        con->Close();
    }

    if (localErr == WEAVE_ERROR_STATUS_REPORT_RECEIVED && statusReport != NULL)
        printf("FAILED to establish secure session with node %" PRIX64 " (%s): %s\n", peerNodeId, ipAddrStr, nl::StatusReportStr(statusReport->mProfileId, statusReport->mStatusCode));
    else
        printf("FAILED to establish secure session with node %" PRIX64 " (%s): %s\n", peerNodeId, ipAddrStr, ErrorStr(localErr));
}

void PrintNodeConfig()
{

    printf("Weave Node Configuration:\n");
    printf("  Fabric Id: %" PRIX64 "\n", FabricState.FabricId);
    printf("  Subnet Number: %X\n", FabricState.DefaultSubnet);
    printf("  Node Id: %" PRIX64 "\n", FabricState.LocalNodeId);

    if (MessageLayer.IsListening)
    {
        printf("  Listening Addresses:");
#if WEAVE_CONFIG_ENABLE_TARGETED_LISTEN
        char nodeAddrStr[64];

        if (FabricState.ListenIPv6Addr == IPAddress::Any
#if INET_CONFIG_ENABLE_IPV4
            && FabricState.ListenIPv4Addr == IPAddress::Any
#endif // INET_CONFIG_ENABLE_IPV4
            )
            printf(" any\n");
        else
        {
            printf("\n");
            if (FabricState.ListenIPv6Addr != IPAddress::Any)
            {
                FabricState.ListenIPv6Addr.ToString(nodeAddrStr, sizeof(nodeAddrStr));
                printf("      %s (ipv6)\n", nodeAddrStr);
            }

#if INET_CONFIG_ENABLE_IPV4
            if (FabricState.ListenIPv4Addr != IPAddress::Any)
            {
                FabricState.ListenIPv4Addr.ToString(nodeAddrStr, sizeof(nodeAddrStr));
                printf("      %s (ipv4)\n", nodeAddrStr);
            }
#endif // INET_CONFIG_ENABLE_IPV4
        }
#else // WEAVE_CONFIG_ENABLE_TARGETED_LISTEN
        printf(" any\n");
#endif // WEAVE_CONFIG_ENABLE_TARGETED_LISTEN
    }
}

void InitWeaveStack(uint64_t aLocalNodeId, uint64_t aFabricId, uint32_t aSubnet, bool listen, bool initExchangeMgr, char *aProductName, char *aManufacturerName)
{
    WEAVE_ERROR res;
    WeaveMessageLayer::InitContext initContext;

    nl::Weave::Stats::SetObjects(&MessageLayer);

    // Seed the random number generator
    System::Timer::Epoch now = System::Timer::GetCurrentEpoch();
    srand((unsigned int) now);

    // Initialize the FabricState object.
    res = FabricState.Init();
    if (res != WEAVE_NO_ERROR)
    {
        printf("FabricState.Init failed: %s\n", ErrorStr(res));
        exit(EXIT_FAILURE);
    }

    FabricState.FabricId = aFabricId;
    FabricState.LocalNodeId = aLocalNodeId;
    FabricState.DefaultSubnet = aSubnet;
    FabricState.PairingCode = "TEST";

    // When using sockets we must listen on specific addresses, rather than ANY. Otherwise you will only be
    // able to run a single Weave application per system.
    FabricState.ListenIPv4Addr = nl::Inet::IPAddress::Any;
    FabricState.ListenIPv6Addr = IPAddress::MakeULA(WeaveFabricIdToIPv6GlobalId(aFabricId), aSubnet, WeaveNodeIdToIPv6InterfaceId(aLocalNodeId));

#if WEAVE_CONFIG_SECURITY_TEST_MODE
    FabricState.LogKeys = true;
#endif

    // Initialize the WeaveMessageLayer object.
    initContext.systemLayer = &SystemLayer;
    initContext.inet = &InetLayerObj;
    initContext.fabricState = &FabricState;
    initContext.listenTCP = listen;
    initContext.listenUDP = true;

    res = MessageLayer.Init(&initContext);
    if (res != WEAVE_NO_ERROR)
    {
        printf("WeaveMessageLayer.Init failed: %s\n", ErrorStr(res));
        exit(EXIT_FAILURE);
    }

    if (initExchangeMgr)
    {
        // Initialize the Exchange Manager object.
        res = ExchangeMgr.Init(&MessageLayer);
        if (res != WEAVE_NO_ERROR)
        {
            printf("WeaveExchangeManager.Init failed: %s\n", ErrorStr(res));
            exit(EXIT_FAILURE);
        }

        res = SecurityMgr.Init(ExchangeMgr, SystemLayer);
        if (res != WEAVE_NO_ERROR)
        {
            printf("WeaveSecurityManager.Init failed: %s\n", ErrorStr(res));
            exit(EXIT_FAILURE);
        }
    }

    MessageLayer.OnConnectionReceived = HandleConnectionReceived;

    SecurityMgr.OnSessionEstablished = HandleSecureSessionEstablished;
    SecurityMgr.OnSessionError = HandleSecureSessionError;

    PrintNodeConfig();
}

namespace nl {
namespace Weave {
namespace Platform {
namespace PersistedStorage {

static WEAVE_ERROR GetCounterValueFromFile(const char *aKey, uint32_t &aValue)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;
exit:
    return err;
}

static WEAVE_ERROR SaveCounterValueToFile(const char *aKey, uint32_t aValue)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;
exit:
    return err;
}

WEAVE_ERROR Read(const char *aKey, uint32_t &aValue)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

exit:
    return err;
}

WEAVE_ERROR Write(const char *aKey, uint32_t aValue)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

exit:
    return err;
}

} // PersistentStorage
} // Platform
} // Weave
} // nl

gboolean ServiceWeaveTimeout(void *data);

bool inHandler = false;

gboolean
ServiceWeaveChannels(GIOChannel *channel, GIOCondition cond, gpointer data)
{
    int fd = (uintptr_t)data;
    fd_set read_set, write_set, except_set;

    if (inHandler) {
        printf("Still in handler!\n");
        return true;
    }

    inHandler = true;

    FD_ZERO(&read_set);
    FD_ZERO(&write_set);
    FD_ZERO(&except_set);

    // Reverse populate the fdset from the context data passed into the function
    if (cond == G_IO_IN) {
        FD_SET(fd, &read_set);
    }
    else if (cond == G_IO_OUT) {
        FD_SET(fd, &write_set);
    }
    else if (cond == G_IO_ERR) {
        FD_SET(fd, &except_set);
    }
    else {
        printf("Unknown condition encountered! (%u)\n", cond);
        return false;
    }

    if (SystemLayer.State() == System::kLayerState_Initialized) {
        SystemLayer.HandleSelectResult(1, &read_set, &write_set, &except_set);
    }

    if (InetLayerObj.State == InetLayer::kState_Initialized) {
        InetLayerObj.HandleSelectResult(1, &read_set, &write_set, &except_set);
    }

    // Re-check the Weave timers and deduce our next wake point.
    ServiceWeaveTimeout(NULL);

    inHandler = false;

    return true;
}

//
// A timer fired - service any handlers in Weave, and re-setup the file handles to be monitored.
//
gboolean ServiceWeaveTimeout(void *data)
{
    fd_set read_set, write_set, except_set;
    struct timeval tv;
    int numFDs = 0;
   
    DEBUG_PRINT(kWeave, "<ServiceWeaveTimeout>");

    FD_ZERO(&read_set);
    FD_ZERO(&write_set);
    FD_ZERO(&except_set);
   
    if (SystemLayer.State() == System::kLayerState_Initialized)
        SystemLayer.HandleSelectResult(0, &read_set, &write_set, &except_set);

    if (InetLayerObj.State == InetLayer::kState_Initialized)
        InetLayerObj.HandleSelectResult(0, &read_set, &write_set, &except_set);

    SetupGioChannels();
    
    return false;
}

GIOChannel *gChannelReadSet[FD_SETSIZE];
GIOChannel *gChannelWriteSet[FD_SETSIZE];
GIOChannel *gChannelExceptSet[FD_SETSIZE];
guint gWeaveTimeoutHandle = 0;

//
// This sets up Glib's GIO channels that are mapped to the respective file handles opened
// by Weave to run on the main Glib context.
//
void SetupGioChannels()
{
    GError **error = NULL;
    fd_set readSet, writeSet, exceptSet;
    int numFDs = 0;
    struct timeval nextWakeDuration;

    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    FD_ZERO(&exceptSet);

    // Max out this duration and let Weave set it appropriately.
    nextWakeDuration.tv_sec = 1000;
    nextWakeDuration.tv_usec = 0;

    if (SystemLayer.State() == System::kLayerState_Initialized)
        SystemLayer.PrepareSelect(numFDs, &readSet, &writeSet, &exceptSet, nextWakeDuration);

    if (InetLayerObj.State == InetLayer::kState_Initialized)
        InetLayerObj.PrepareSelect(numFDs, &readSet, &writeSet, &exceptSet, nextWakeDuration);

    DEBUG_PRINT(kWeave, "Timer: %ld", nextWakeDuration.tv_sec * 1000 + nextWakeDuration.tv_usec / 1000);

    // Stop any previous timers that were registered. We can safely do this since we're running in a single thread
    // context
    if (gWeaveTimeoutHandle) {
       g_source_remove(gWeaveTimeoutHandle);
    }

    // Schedule a wake-up to process the Weave timers.
    gWeaveTimeoutHandle = g_timeout_add_full(G_PRIORITY_HIGH, nextWakeDuration.tv_sec * 1000 + (nextWakeDuration.tv_usec / 1000), ServiceWeaveTimeout, NULL, NULL);

    for (int i = 0; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, &readSet)) {
            if (gChannelReadSet[i] == NULL) {
                GIOChannel *channel = g_io_channel_unix_new(i);
                g_io_channel_set_encoding(channel, NULL, error);
                g_io_add_watch(channel, G_IO_IN, ServiceWeaveChannels, (gpointer)(uintptr_t)i);
                gChannelReadSet[i] = channel;
            }
        }
        else {
            if (gChannelReadSet[i]) {
                g_io_channel_unref(gChannelReadSet[i]);
                gChannelReadSet[i] = NULL;
            }
        }
        

        if (FD_ISSET(i, &writeSet)) {
            if (gChannelWriteSet[i] == NULL) {
                GIOChannel *channel = g_io_channel_unix_new(i);
                g_io_channel_set_encoding(channel, NULL, error);
                g_io_add_watch(channel, G_IO_OUT, ServiceWeaveChannels, (gpointer)(uintptr_t)i);
                gChannelWriteSet[i] = channel;
            }
        }
        else {
            if (gChannelWriteSet[i]) {
                g_io_channel_unref(gChannelWriteSet[i]);
                gChannelWriteSet[i] = NULL;
            }
        }

        if (FD_ISSET(i, &exceptSet)) {
            if (gChannelExceptSet[i] == NULL) {
                GIOChannel *channel = g_io_channel_unix_new(i);
                g_io_channel_set_encoding(channel, NULL, error);
                g_io_add_watch(channel, G_IO_ERR, ServiceWeaveChannels, (gpointer)(uintptr_t)i);
                gChannelExceptSet[i] = channel;
            }
        }
        else {
            if (gChannelExceptSet[i]) {
                g_io_channel_unref(gChannelExceptSet[i]);
                gChannelExceptSet[i] = NULL;
            }
        }
    }
}

void InitGioChannels()
{
    for (int i = 0; i < FD_SETSIZE; i++) {
        gChannelReadSet[i] = NULL;
        gChannelWriteSet[i] = NULL;
        gChannelExceptSet[i] = NULL;
    }

    SetupGioChannels();
}

uint64_t gDebugEventBuffer[192];
uint64_t gInfoEventBuffer[64];
uint64_t gProdEventBuffer[256];

void InitializeEventLogging(WeaveExchangeManager *inMgr)
{
    size_t arraySizes[] = { sizeof(gDebugEventBuffer), sizeof(gInfoEventBuffer), sizeof(gProdEventBuffer) };
    void *arrays[] = {
        static_cast<void *>(&gDebugEventBuffer[0]),
        static_cast<void *>(&gInfoEventBuffer[0]),
        static_cast<void *>(&gProdEventBuffer[0])
    };

    //nl::Weave::Profiles::DataManagement::LoggingManagement::CreateLoggingManagement(inMgr, 3, &arraySizes[0], &arrays[0], NULL, NULL, NULL);
}

namespace nl {
namespace Weave {
namespace Profiles {
namespace WeaveMakeManagedNamespaceIdentifier(DataManagement, kWeaveManagedNamespaceDesignation_Current) {
namespace Platform {
    // for this application, the dummy critical section is sufficient.
    void CriticalSectionEnter()
    {
        return;
    }

    void CriticalSectionExit()
    {
        return;
    }
} // Platform
} // WeaveMakeManagedNamespaceIdentifier(DataManagement, kWeaveManagedNamespaceDesignation_Current)
} // Profiles
} // Weave
} // nl

// DataManagement::CommandSender gCommandSender;
