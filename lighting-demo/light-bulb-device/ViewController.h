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
 *      Main entity that manages the window and its UI elements.
 *
 */
#ifndef _VIEW_CONTROLLER_H
#define _VIEW_CONTROLLER_H

#include <map>
#include <string>
#include <gtk/gtk.h>
#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "ZclOnOffClusterSource.h"

class ViewController
{
public:
    ViewController(GtkApplication *app, ZclOnOffClusterSource *dataSource);

    enum {
        kStoreColumn_FieldName,
        kStoreColumn_FieldValue,
        kStoreColumn_NumColumns
    };

    static void  CellEditedCallback(GtkCellRendererText *cell,
                                    gchar               *path_string,
                                    gchar               *new_text,
                                    gpointer             user_data);

    static gboolean FlushChanges(void *data);
    static void FlushChangesBtnPressed(GtkWidget *event_box, GdkEventButton *event, void *data);
    static void BulbPressed(GtkWidget *event_box, GdkEventButton *event, void *data);

private:
    GtkWidget *_window;
    GtkCssProvider *_cssProvider;
    GtkStyleContext *_context;
    GtkWidget *_grid;
    GtkWidget *_frame;
    GtkWidget *_label;
    GtkWidget *_image;
    GtkWidget *_flushChangesButton;
    GtkWidget *_imageEventBox;
    GtkWidget *_numSubscribersLabel;

    GtkTreeStore *_store;
    GtkWidget *_dataList;
    GtkTreeViewColumn *_column;
    GtkCellRenderer *_renderer;

    guint _changeFlushTimerHandle;
    ZclOnOffClusterSource *_dataSource;
    std::map<std::string, std::string> _changedItems;
};

#endif // _VIEW_CONTROLLER_H
