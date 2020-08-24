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
 *      This file contains tables to colorize debug output
 *
 */

#include "Debug.h"

bool gTab = false;

const char *ModuleColorTable[] = {
    "\x1B[31m",

    "\x1B[32m",

    "\x1B[33m",

    "\x1B[35m",

    "\x1B[36m",

    "\x1B[37m",

    "\x1B[34m"
};

const char *ModuleToString(Module aModule)
{
    switch (aModule) {
        case kAPP:
            return "APP";
            break;

        case kWDM:
            return "WDM";
            break;

        case kWeave:
            return "WEAV";
            break;
            
        default:
            return "Unknown";
            break;
    }
}
