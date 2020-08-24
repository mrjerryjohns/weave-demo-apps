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

#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <termios.h>
#include <string.h>

#include <sys/time.h>

#include <Weave/Profiles/data-management/DataManagement.h>
#include <Weave/Common/ResourceTypeEnum.h>
#include <SystemLayer/SystemTimer.h>
#include <SystemLayer/SystemFaultInjection.h>
#include <Weave/Support/WeaveFaultInjection.h>
#include <InetLayer/InetFaultInjection.h>
#include <InetLayer/InetLayer.h>
#include <SystemLayer/SystemFaultInjection.h>
#include <Weave/Support/WeaveFaultInjection.h>
#include <Weave/Support/nlargparser.hpp>
#include <Weave/Profiles/data-management/DataManagement.h>

#include <functional>
#include <vector>

#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#define GETTEXT_PACKAGE "gtk20"
#include <glib/gi18n-lib.h>

#include "Debug.h"
#include "App.h"

using namespace nl::Inet;
using namespace nl::Weave;
using namespace nl::Weave::TLV;
using namespace nl::Weave::Profiles;
using namespace nl::Weave::Profiles::DataManagement;


void InitSystemLayer();
void InitNetwork();
void InitWeaveStack(uint64_t aLocalNodeId, uint64_t aFabricId, uint32_t aSubnet, bool listen, bool initExchangeMgr, char *aProductName, char *aManufacturerName);
void InitGioChannels();
void InitializeEventLogging(WeaveExchangeManager *inMgr);

IPAddress gAddr;
uint64_t gDestinationNodeId;

void
EngineEventCallback (void * const aAppState,
                     SubscriptionEngine::EventID aEvent,
                     const SubscriptionEngine::InEventParam & aInParam,
                     SubscriptionEngine::OutEventParam & aOutParam)
{
    OutboundSubscriptionManager *_this = static_cast<OutboundSubscriptionManager*>(aAppState);

    switch (aEvent) {
        case SubscriptionEngine::kEvent_OnIncomingSubscribeRequest :
            DEBUG_PRINT(kAPP, "Engine->kEvent_OnIncomingSubscribeRequest");
            aOutParam.mIncomingSubscribeRequest.mRejectRequest = true;
            break;

        default:
            SubscriptionEngine::DefaultEventHandler(aEvent, aInParam, aOutParam);
            break;
    }
}

void
Activate(GtkApplication *application, gpointer user_data)
{
    WEAVE_ERROR err;
    uint64_t nodeId = IPv6InterfaceIdToWeaveNodeId(gAddr.InterfaceId());
    char const *vendor = "Google";
    char const *device = "TestDevice";
    App *app;

    srand(time(NULL));

    InitSystemLayer();
    InitNetwork();
    InitWeaveStack(nodeId, gAddr.GlobalId(), gAddr.Subnet(), true, true, (char *)vendor, (char *)device);
    InitGioChannels();

    err = SubscriptionEngine::GetInstance()->Init(&ExchangeMgr, NULL, EngineEventCallback);
    SuccessOrExit(err);

    InitializeEventLogging(&ExchangeMgr);

    DEBUG_PRINT(kAPP, "Inited");

    app = new App();
    app->Start(application, gDestinationNodeId);
   
    // Run the main GTK event loop
    gtk_main();

exit:
    return;
}

gboolean
cmdline_ipaddr (const gchar   *option,
                  const gchar   *value,
                  gpointer data,
                  GError       **error)
{
    nl::ArgParser::ParseIPAddress(value, gAddr);
    return true;
}

static GOptionEntry entries[] =
{
  { "node-addr", 'r', 0, G_OPTION_ARG_CALLBACK, (gpointer)cmdline_ipaddr, "Node Address", NULL },
  { "dest-node-id", 'r', 0, G_OPTION_ARG_INT64, &gDestinationNodeId, "Destination Node Id", NULL },
  { NULL }
};

int
main (int argc, char **argv)
{
  GError *error = NULL;
  GtkApplication *app;
  GOptionContext *context;
  char appName[256];
  int status;

  context = g_option_context_new ("Weave Virtual Node");
  g_option_context_add_main_entries (context, entries, NULL);
  g_option_context_add_group (context, gtk_get_option_group (TRUE));

  if (!g_option_context_parse (context, &argc, &argv, &error))
  {
      g_print ("option parsing failed: %s\n", error->message);
      exit (1);
  }

  sprintf(appName, "weave.demoapp.LightSwitch%llu", IPv6InterfaceIdToWeaveNodeId(gAddr.InterfaceId()));
  DEBUG_PRINT(kAPP, "Starting GTK App %s", appName);

  app = gtk_application_new (appName, G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (Activate), NULL);
  status = g_application_run (G_APPLICATION(app), argc, argv);


  g_object_unref (app);

  return status;
}
