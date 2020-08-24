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
 *      This file contains macros to emit debug output
 *
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#define _ACTION_ON_COND(condition, action)      do {                                    \
                                                    if ((condition))        \
                                                    {                                   \
                                                        action;                         \
                                                    }                                   \
                                                } while (0);

#define _ACTION_ON_ERR(err, action)             _ACTION_ON_COND((err) < 0, action)

enum Module {
    kAPP = 1,
    kWDM = 2,
    kWeave = 3
};

#define DEBUG_MASK (0xff & ~(1 << kWeave))
#define FILE_DEBUG_MASK (0xff)

const char *ModuleToString(Module aModule);
extern const char *ModuleColorTable[];
extern bool gTab;
 
#define DEBUG_PRINT(module, fmt, ...) if (DEBUG_MASK & (1 << module))  fprintf(stdout, "%s%s[%s] " fmt "\e[0m\n", gTab ? "\t\t\t" : "", ModuleColorTable[module], ModuleToString(module), ##__VA_ARGS__); \
                                      //if (FILE_DEBUG_MASK & (1 << module)) fprintf(stderr, "[%s] " fmt "\n", ModuleToString(module), ##__VA_ARGS__); \

#define DEBUG_PRINT_BEGIN(module, fmt, ...) if (DEBUG_MASK & (1 << module)) fprintf(stdout, "%s%s[%s] " fmt "", gTab ? "\t\t\t" : "", ModuleColorTable[module], ModuleToString(module), ##__VA_ARGS__); \
                                            // if (FILE_DEBUG_MASK & (1 << module)) fprintf(stderr, "[%s] " fmt "", ModuleToString(module), ##__VA_ARGS__);
                                               
#define DEBUG_PRINT_MID(module, fmt, ...) if (DEBUG_MASK & (1 << module)) fprintf(stdout, fmt, ##__VA_ARGS__); \
                                          // if (FILE_DEBUG_MASK & (1 << module)) fprintf(stderr, fmt, ##__VA_ARGS__);

#define DEBUG_PRINT_END(module, fmt, ...) if (DEBUG_MASK & (1 << module)) fprintf(stdout, fmt "\e[0m\n", ##__VA_ARGS__); \
                                          // if (FILE_DEBUG_MASK & (1 << module)) fprintf(stderr, fmt "\n", ##__VA_ARGS__);

#define ERROR_PRINT(fmt, ...) fprintf(stdout, "%s%s<%s:%d> " fmt "\e[0m\n", gTab ? "\t\t\t" : "", ModuleColorTable[0], __FILE__, __LINE__, ##__VA_ARGS__)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    ASSERT MACROS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define RET_ON_ERR(err)                                     _ACTION_ON_ERR(err, return err)
#define RET_ON_COND(condition, ret)                         _ACTION_ON_COND(condition, return ret)
#define RET_ON_ERR_WMSG(err,  msg, ...)                     _ACTION_ON_ERR(err, ERROR_PRINT(msg, ##__VA_ARGS__); return err)
#define RET_ON_COND_WMSG(condition, ret, msg, ...)          _ACTION_ON_COND(condition, ERROR_PRINT(msg, ##__VA_ARGS__); return ret)

#define GOTO_ON_ERR(err)                                    _ACTION_ON_ERR(err, goto exit)
#define GOTO_ON_COND(condition, action)                     _ACTION_ON_COND(condition, action; goto exit)
#define GOTO_ON_ERR_WMSG(err, msg, ...)                     _ACTION_ON_ERR(err, ERROR_PRINT(msg, ##__VA_ARGS__); goto exit)
#define GOTO_ON_COND_WMSG(condition, action, msg, ...)      _ACTION_ON_COND(condition, ERROR_PRINT(msg, ##__VA_ARGS__); action; goto exit)

#endif
