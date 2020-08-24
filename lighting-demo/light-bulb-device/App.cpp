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
 *      Main application code for the light-bulb device.
 *
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

#include "App.h"
#include "ViewController.h"

using namespace nl::Weave;
using namespace nl::Weave::Profiles::DataManagement;

extern SubscriptionEngine gSubscriptionEngine;

App::App()
    : _viewController(NULL)
{
}

WEAVE_ERROR
App::Start(GtkApplication *app)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;
    TraitDataHandle handle;

    // Create the window and UI for our application
    _viewController = new ViewController(app, &_onOffDataSource);

    // The on/off data source is going to interact with the view contoller for inbound commands
    // that it has to handle (like toggle).
    _onOffDataSource.SetViewController(_viewController);

    err = _inboundSubscriptionManager.Init();
    GOTO_ON_ERR_WMSG(err, "Failed to init subscription manager"); 

    // Add the data source to the source catalog to make it available for subscription
    err = _inboundSubscriptionManager.GetSourceCatalog().Add(0, &_onOffDataSource, handle);
    GOTO_ON_ERR_WMSG(err, "Failed to add source");

exit:
    return err;
} 
