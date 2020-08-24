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

#include <Weave/Core/WeaveError.h>
#include <Weave/Core/WeaveSecurityMgr.h>
#include <Weave/Support/CodeUtils.h>
#include <Weave/Profiles/WeaveProfiles.h>
#include <Weave/Profiles/data-management/DataManagement.h>

#include "OutboundSubscriptionManager.h"
#include "OutboundSubscriptionManager.h"
#include "ZclOnOffClusterSink.h"
#include "ViewController.h"

class App
{
public:
    App();
    WEAVE_ERROR Start(GtkApplication *application, uint64_t destinationNodeId);
    static gboolean ToggleBulbState(void *data);

private:
    nl::Weave::Binding *_binding;
    ViewController *_viewController;
    OutboundSubscriptionManager *_subscriptionManager;
    ZclOnOffClusterSink *_onOffSink;
    nl::Weave::Profiles::DataManagement::CommandSender _commandSender;
};
