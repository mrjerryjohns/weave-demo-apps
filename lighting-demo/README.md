
# Weave Lighting Demo Applications

[Overview](#overview)

[Building Code](#building-code)

[Run Applications](#run-applications)

[Scenarios](#scenarios)

## Overview

This contains two demo applications that mimic a ZCL-based Light Switch and Light Bulb  respectively. These are modeled and implemented using the Weave Data Language (WDL) and Weave Data Management (WDM) protocol, all running on-top of the Weave stack. The goal is to demonstrate Weave application API and Protocol concepts with a tangible, real-world example.

![enter image description here](https://i.imgur.com/uHdGQgv.png)

![enter image description here](https://i.imgur.com/9xUPAlA.gif)
### :o: Light Bulb

The LightBulb application models a Zigbee light bulb with on/off state that can be toggled either locally from within the app, or remotely through a command. It also contains a UI that lists all the various attributes/properties in the on/off cluster/trait that can be modified by the user.

### :o: Light Switch
The LightSwitch application models a Zigbee light-switch that both shows the state of the bulb as well as being able to remotely toggle it. It also subscribes to the various other attributes/properties in that trait and shows it on its UI.

The apps demonstrate:
* Intuitive, simple application APIs
* Powerful frameworks to automatically manage, aggregate and notify subscribers of data changes
*  Compact deltas
*  Resilience to publisher/subscriber going away

### Pre-requisites
* Linux/Mac OSX
* GCC

## Building Code

### Build OpenWeave
1. Clone this [fork](https://github.com/mrjerryjohns/openweave-core/tree/feature/chip-demo-lighting) of openweave-core.
2. Checkout the 'feature/chip-demo-lighting' branch:
```
$ git checkout feature/chip-demo-lighting
```
3. Follow the steps [here](https://openweave.io/guides/build) to build OpenWeave. Specifically follow the steps to build for [Project Linking](https://openweave.io/guides/build#project_linking). Make sure to stage the built output to a specific staging folder.

To use the top-level helper Makefile to build openweave-core as a local submodule:
```
# From weave-demo-apps directory:
make weave
```

### Build/Run WDL Compiler

> :warning: *The generated files have already been checked into the source base, so only do this if  you want to re-generate device code for the relevant schema files.*

#### Build WDLC
1. Clone this [fork]([https://github.com/mrjerryjohns/openweave-wdlc/tree/feature/chip-demo-lighting](https://github.com/mrjerryjohns/openweave-wdlc/tree/feature/chip-demo-lighting)) of openweave-wdlc.
2. Checkout the 'feature/chip-demo-lighting' branch:
```
$ git checkout feature/chip-demo-lighting
```
3. Follow the steps to build **and** install the `wdlc` compiler to a system location.

#### Generate Code

1. In this top-level folder, run:
```
$ wdlc --include-schema-path schema --gen weave-device-cpp --output schema schema/zigbee/cluster/general/on_off_cluster.proto
```
That should update the auto-generated code files located in `schema/zigbee/cluster/general/`:
* OnOffCluster.h
* OnOffCluster.cpp

### Build Demo Applications

#### Pre-requisites:

*Linux:*

```
$ sudo apt-get install libgtk-3-dev libjsoncpp-dev
```
*Mac:*

We recommend using [Homebrew](https://brew.sh/) to install these dependencies.

```
$ brew install gtk+3 jsoncpp
```
#### Build Demo Apps:

1. In the top-level 'lighting-demo' folder, edit `WEAVE_BASE` in `Defines.mak` to point to the right staging folder which contains the built output of `openweave-core`.
1. In the top-level 'lighting-demo' folder, run:
```
$ make -j8 all
```

The resultant light-bulb-device and light-switch applications can be found in the `light-bulb-device/` and `light-switch/` folders.

## Run Applications

### Setup IPv6 Addresses:

For the applications to work, they each need a unique 'Node ID'. They also need a common Weave Fabric to communicate over (identified by a Fabric ID). This is used to construct the IPv6 ULA (unique-local-address).

More details on the above can be found [here](https://openweave.io/guides/weave-primer/fabric#ipv6_addressing).

For the purposes of this demo, we'll bind each application to unique, specially constructed IPv6 addresses and the applications will each 'reverse compute' the Node ID and Fabric ID from the bound address.

**Fabric ID**: 0x1
**Subnet**: 0x1

**Light Bulb**: 
* NodeID = 0x1
* IPv6 Address: `fd01:0000:0001:1::1`

**Light Switch #1**: 
* NodeID = 0x2
* IPv6 Address: `fd00:0000:0001:1::2`

**Light Switch #2 (if needed)**: 
* NodeID = 0x3
* IPv6 Address: `fd00:0000:0001:1::3`

*Linux:* 
```
$ ifconfig eth0 inet6 add fd00:0000:0001:1::1/64
$ ifconfig eth0 inet6 add fd00:0000:0001:1::2/64
$ ifconfig eth0 inet6 add fd00:0000:0001:1::3/64
```

*Mac:* 
```
$ sudo ifconfig en0 inet6 add fd00:0000:0001:1::1/64
$ sudo ifconfig en0 inet6 add fd00:0000:0001:1::2/64
$ sudo ifconfig en0 inet6 add fd00:0000:0001:1::3/64
```

### Run Demo

1. Launch the bulb device:
```console
$ cd light-bulb-device
$ ./light-bulb-device --node-addr fd00:0000:0001:1::1
WEAVE:ML: WoBLE disabled (BLE layer not initialized)
Weave Node Configuration:
  Fabric Id: 1
  Subnet Number: 1
  Node Id: 1
  Listening Addresses:
      fd01:0:1:1::1 (ipv6)
[APP] Inited
```

The following window appears:

![enter image description here](https://i.imgur.com/UlNk20u.png)

The bulb icon indicates the state of the bulb. Clicking it alters the state of it from within the logical bulb device.

There is also a table of attributes/properties that are mutable. These can be altered by double-clicking on the appropriate row/column and changing it.

Without a switch subscribed to the bulb, the changes made stay within the confines of the bulb.

2. Launch the switch:
```
$ cd light-switch
$ ./light-switch --node-addr fd00:0000:0001:1::2 --dest-node-id 1
WEAVE:ML: WoBLE disabled (BLE layer not initialized)
Weave Node Configuration:
  Fabric Id: 1
  Subnet Number: 1
  Node Id: 2
  Listening Addresses:
      fd00:0:1:1::2 (ipv6)
[APP] Inited
[WDM] Resubscribing in 0 seconds
[WDM] Client->kEvent_OnSubscribeRequestPrepareNeeded
[WDM] Client->kEvent_OnExchangeStart
WEAVE:EM: Msg sent WDM:SubscribeRequest 26 0000000000000001 0000 BC59 0 MsgId:B8C97064
WEAVE:EM: Msg rcvd WDM:Notify 61 0000000000000001 0000 BC59 0 MsgId:151D581A
...
...
[APP] << OnOff = off
[APP] << GlobalSceneControl = on
[APP] << OnTime = 30
[APP] << OffWaitTime = 100
[APP] << RampInfo.Duration = 60
[APP] << RampInfo.Type = 2
Storing global_scene_control
Storing off_wait_time
Storing on_time
Storing ramp_info.duration
Storing ramp_info.type
```

On boot, the switch directly subscribes to the light bulb for the Zcl On/Off cluster. On successful subscription establishment, it then is notified on ensuing changes to any attribute within that cluster.

The following UI appears:

![enter image description here](https://i.imgur.com/XLdQ35P.png)

Clicking on the switch sends out a 'Toggle' command to the bulb. Since the switch app is also subscribed to the on/off state from the bulb, it's UI will update to indicate the state of the bulb appropriately.

The switch is also subscribed to all other attributes on that cluster, depicted in the attributes table. Modifying any of the attributes on the bulb, and clicking "Flush Changes" will trigger a notification to the switch, updating it's set of subscribed attributes (highlighted momentarily in green).

## Scenarios

### State Reporting/Notification

The switch is subscribed to the bulb for state notifications. Anytime any attribute changes on the 
bulb, the switch is notified of it immediately.

This starts with the switch sending out a SubscribeRequest, received by the bulb:
```
WEAVE:EM: Msg rcvd WDM:SubscribeRequest 26 0000000000000002 0000 BC59 0 MsgId:B8C97064
[WDM] Engine->kEvent_OnIncomingSubscribeRequest
WEAVE:DMG: {
WEAVE:DMG: 		SubscriptionTimeoutMin = 3,
WEAVE:DMG: 		SubscriptionTimeoutMax = 3,
WEAVE:DMG: 		PathList =
WEAVE:DMG: 		[
WEAVE:DMG: 			<Resource = {ProfileId = 0x10006,}>,
WEAVE:DMG: 		],
WEAVE:DMG: 		VersionList =
WEAVE:DMG: 		[
WEAVE:DMG: 			Null,
WEAVE:DMG: 		],
WEAVE:DMG: }
[WDM] Incoming Subscription from 2
```

The bulb responds back with a Notify message that contains a full snapshot of the current state of the On/Off cluster, as received by the switch:
```
[WDM] Client->kEvent_OnNotificationRequest
WEAVE:DMG: {
WEAVE:DMG: 	SubscriptionId = 0xa5a6fb5ee3b40eef,
WEAVE:DMG: 	DataList =
WEAVE:DMG: 	[
WEAVE:DMG: 		{
WEAVE:DMG: 			DataElementPath = <Resource = {ProfileId = 0x10006,}>,
WEAVE:DMG: 			DataElementVersion = 0x351eb34b6bc6edc2,
WEAVE:DMG: 			Data =
WEAVE:DMG: 			{
WEAVE:DMG: 				0x1 = false,
WEAVE:DMG: 				0x2 = true,
WEAVE:DMG: 				0x3 = 30,
WEAVE:DMG: 				0x4 = 100,
WEAVE:DMG: 				0x5 =
WEAVE:DMG: 				{
WEAVE:DMG: 					0x1 = 60,
WEAVE:DMG: 					0x2 = 2,
WEAVE:DMG: 				},
WEAVE:DMG: 			},
WEAVE:DMG: 		},
WEAVE:DMG: 	],
WEAVE:DMG: }
```

Anytime the state changes, just the relevant attributes that changed are conveyed to the switch (e.g on/off):

```
WEAVE:EM: Msg rcvd WDM:Notify 43 0000000000000001 0000 2597 0 MsgId:151D582B
[WDM] Client->kEvent_OnSubscriptionActivity
[WDM] Client->kEvent_OnNotificationRequest
WEAVE:DMG: {
WEAVE:DMG: 	SubscriptionId = 0xff766605754ad727,
WEAVE:DMG: 	DataList =
WEAVE:DMG: 	[
WEAVE:DMG: 		{
WEAVE:DMG: 			DataElementPath = <Resource = {ProfileId = 0x10006,}/0x1 = null>,
WEAVE:DMG: 			DataElementVersion = 0x351eb34b6bc6edc3,
WEAVE:DMG: 			Data = true,
WEAVE:DMG: 		},
WEAVE:DMG: 	],
WEAVE:DMG: }
[APP] << OnOff = on
[WDM] Client->kEvent_OnNotificationProcessed
```

### Commands

Clicking on the switch icon results in the 'Toggle' command being sent out by the switch and received on the bulb:
```
WEAVE:EM: Msg rcvd WDM:CommandRequest 20 0000000000000002 0000 FC27 0 MsgId:00B36B23
WEAVE:DMG: {
WEAVE:DMG: 	Command Path = <Resource = {ProfileId = 0x10006,InstanceId = 0x0,}>,
WEAVE:DMG: 	Command Type = 0x2,
WEAVE:DMG: }
[APP] Setting trait handler on/off
[APP] >> OnOff = on
```

This in turn results in Notifies being sent back out to the switch:

```
[WDM] Client->kEvent_OnNotificationRequest
WEAVE:DMG: {
WEAVE:DMG: 	SubscriptionId = 0xf8d46fcae8a0f7bf,
WEAVE:DMG: 	DataList =
WEAVE:DMG: 	[
WEAVE:DMG: 		{
WEAVE:DMG: 			DataElementPath = <Resource = {ProfileId = 0x10006,}/0x1 = null>,
WEAVE:DMG: 			DataElementVersion = 0x3bf4556c43fc005f,
WEAVE:DMG: 			Data = true,
WEAVE:DMG: 		},
WEAVE:DMG: 	],
WEAVE:DMG: }
[APP] << OnOff = on
```

### Compact Deltas

Because WDM has a 'priming' phase, it allows it to then send compact deltas there-after.

E.g on/off and global_scene_control changing:
```
[WDM] Client->kEvent_OnNotificationRequest
WEAVE:DMG: {
WEAVE:DMG: 	SubscriptionId = 0x979861a6d4d18b3e,
WEAVE:DMG: 	DataList =
WEAVE:DMG: 	[
WEAVE:DMG: 		{
WEAVE:DMG: 			DataElementPath = <Resource = {ProfileId = 0x10006,}>,
WEAVE:DMG: 			DataElementVersion = 0x351eb34b6bc6edc5,
WEAVE:DMG: 			Data =
WEAVE:DMG: 			{
WEAVE:DMG: 				0x2 = true,
WEAVE:DMG: 				0x3 = 45,
WEAVE:DMG: 			},
WEAVE:DMG: 		},
WEAVE:DMG: 	],
WEAVE:DMG: }
[APP] << GlobalSceneControl = on
[APP] << OnTime = 45
```

The UI on the switch changes appropriately:

Play around by changing different items and see how it affects the messages sent out.

### Subscription Synchronization

Unlike ZCL, WDM uses Subscribe Confirm messages to keep the subscription synchronized. This prevents the need to use data as a means to keep both parties synchronized.

In this demo, the switch sends SubscribeConfirm messages every 1s.

```
WEAVE:EM: Msgg sent WDM:SubscribeConfirm 12 0000000000000001 0000 BC5A 0 MsgId:B8C97067
WEAVE:EM: Msg rcvd Common:StatusReport 6 0000000000000001 0000 BC5A 0 MsgId:151D581D
WEAVE:EM: Msgg sent Common:ACK 0 0000000000000001 0000 BC5A 0 MsgId:B8C97068
[WDM] Client->kEvent_OnSubscriptionActivity
```

### Failure Resiliency

WDM is built to be tolerant to communication issues between the publisher and subscriber,  and re-establishing synchronization once that is resolved.

### Publisher Restart

In this scenario, we modify the state of the bulb from its initial power-on state (e.g turn it on, alter a few properties) and ensure the switch sees those changes, then kill the bulb application.

Pretty soon, the switch detects this:
```
WEAVE:EM: Msgg sent WDM:SubscribeConfirm 12 0000000000000001 0000 5C41 0 MsgId:97BEE725
WEAVE:EM: Retransmit MsgId:97BEE725 Send Cnt 2
WEAVE:EM: Retransmit MsgId:97BEE725 Send Cnt 3
WEAVE:EM: Retransmit MsgId:97BEE725 Send Cnt 4
WEAVE:EM: Failed to Send Weave MsgId:97BEE725 sendCount: 4 max retries: 3
WEAVE:-: Weave Error 4099 (0x00001003): Message not acknowledged after max retries at ../../src/lib/profiles/data-management/Current/SubscriptionClient.cpp:1797
<OutboundSubscriptionManager.cpp:171> Client->kEvent_OnSubscriptionTerminated reason=0x1003
```

It starts re-subscribing to attempt to re-synchronize:
```
[WDM] Resubscribing in 3 seconds
[WDM] Client->kEvent_OnSubscribeRequestPrepareNeeded
[WDM] Client->kEvent_OnExchangeStart
WEAVE:EM: Msgg sent WDM:SubscribeRequest 26 0000000000000001 0000 4624 0 MsgId:25D32C6F
WEAVE:EM: Retransmit MsgId:25D32C6F Send Cnt 2
WEAVE:EM: Retransmit MsgId:25D32C6F Send Cnt 3
WEAVE:EM: Retransmit MsgId:25D32C6F Send Cnt 4
WEAVE:EM: Failed to Send Weave MsgId:25D32C6F sendCount: 4 max retries: 3
WEAVE:-: Weave Error 4099 (0x00001003): Message not acknowledged after max retries at ../../src/lib/profiles/data-management/Current/SubscriptionClient.cpp:1797
<OutboundSubscriptionManager.cpp:171> Client->kEvent_OnSubscriptionTerminated reason=0x1003
[WDM] Resubscribing in 3 seconds
```

Restart the bulb application - right away, the subscription is re-established. Notice how the state on the switch 'resets' to the state of the bulb currently.



### Subscriber Restart

In this scenario, we do the reverse - the switch is restarted. Make sure to modify the state of the bulb sufficiently before killing the switch application.

On ensuing restart, the switch now correctly shows the state of the bulb.

### Multiple Switches

You can launch a 2nd switch instance and have it subscribe as well to the light bulb. Notice how nothing has to change on the publisher to make this happen. All behaviors described so far scale naturally in the multi-switch example.


## Source Code

### Organization

|File/Folder| Description |
|--|--|
|**common/**  | Common stack initialization and debug macros  |
|**schema/zigbee/cluster/general** | Contains the schema definitions + generated code |
|:heavy_plus_sign: on_off_cluster.proto*  | ZCL On/Off cluster definition
|:heavy_plus_sign: OnOffCluster.h, .cpp* | Code-generated files 
| **light-bulb-device/** | Light Bulb Device application code
|:heavy_plus_sign: Main.cpp | Main entrypoint for the app, contains Weave stack init |
|:heavy_plus_sign: App.cpp | Core application code
|:heavy_plus_sign: ViewController.cpp | Code to manage the UI/Window
|:heavy_plus_sign: InboundSubscriptionManager.cpp | Manages incoming subscriptions
|:heavy_plus_sign: ZclOnOffClusterSource.cpp | Trait handler for the on/off cluster (server side)
| **light-switch/** | Light Bulb Device application code
|:heavy_plus_sign: Main.cpp | Main entrypoint for the app, contains Weave stack init |
|:heavy_plus_sign: App.cpp | Core application code
|:heavy_plus_sign: ViewController.cpp | Code to manage the UI/Window
|:heavy_plus_sign: OutboundSubscriptionManager.cpp | Manages outbound subscription
|:heavy_plus_sign: ZclOnOffClusterSink.cpp | Trait handler for the on/off cluster (client side)

