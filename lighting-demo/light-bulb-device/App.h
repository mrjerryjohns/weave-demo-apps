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
 *      Header file for the main application class
 *
 */
#include <Weave/Core/WeaveError.h>
#include <Weave/Core/WeaveSecurityMgr.h>
#include <Weave/Support/CodeUtils.h>
#include <Weave/Profiles/WeaveProfiles.h>
#include <Weave/Profiles/data-management/DataManagement.h>

#include "InboundSubscriptionManager.h"
#include "ZclOnOffClusterSource.h"
#include "ViewController.h"

class App
{
public:
    App();
    WEAVE_ERROR Start(GtkApplication *app);

private:
    ViewController *_viewController;
    InboundSubscriptionManager _inboundSubscriptionManager;
    ZclOnOffClusterSource _onOffDataSource;
};
