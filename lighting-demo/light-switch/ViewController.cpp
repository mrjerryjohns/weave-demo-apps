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

#include "ViewController.h"
#include "Debug.h"
#include "App.h"

void
ViewController::SwitchPressed(GtkWidget *button, GdkEventButton *event, void *data)
{
    ViewController *_this = static_cast<ViewController *>(data);
    _this->_app->ToggleBulbState(_this->_app);
}

void SetMargin(GtkWidget *widget)
{
    gtk_widget_set_margin_top(widget, 20);
    gtk_widget_set_margin_bottom(widget, 20);
    gtk_widget_set_margin_start(widget, 20);
    gtk_widget_set_margin_end(widget, 20);
}

gboolean
ViewController::ChangedCellsColorResetTimeout(void *data)
{
    GtkTreeIter iter;
    gboolean iterValid = true;
    GValue fieldNameValue;
    const gchar *fieldName;
    GValue fieldValueValue;
    const gchar *fieldValue;
    ViewController *_this = static_cast<ViewController *>(data);

    for (iterValid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(_this->_store), &iter); iterValid == true; iterValid = gtk_tree_model_iter_next(GTK_TREE_MODEL(_this->_store), &iter)) {
        fieldNameValue = G_VALUE_INIT;

        gtk_tree_model_get_value(GTK_TREE_MODEL(_this->_store), &iter, kStoreColumn_FieldName, &fieldNameValue);
        fieldName = g_value_get_string(&fieldNameValue);

        fieldValueValue = G_VALUE_INIT;
        g_value_init(&fieldValueValue, G_TYPE_STRING);
        g_value_set_string(&fieldValueValue, "LightSlateGray");
        gtk_tree_store_set_value(_this->_store, &iter, kStoreColumn_Color, &fieldValueValue);
        g_value_unset(&fieldValueValue);
    }

    return false;
}

void
ViewController::ProcessChanges(std::map<std::string, std::string> &changeList)
{
    GtkTreeIter iter;
    gboolean iterValid = true;
    GValue fieldNameValue;
    const gchar *fieldName;
    GValue fieldValueValue;
    const gchar *fieldValue;

    for (auto it = changeList.begin(); it != changeList.end(); it++) {
        if (it->first == "on_off") {
            if (it->second == "on") {
                gtk_image_set_from_file(GTK_IMAGE(_image), "LightSwitch-on.png");
            }
            else {
                gtk_image_set_from_file(GTK_IMAGE(_image), "LightSwitch.png");
            }
        }

        for (iterValid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(_store), &iter); iterValid == true; iterValid = gtk_tree_model_iter_next(GTK_TREE_MODEL(_store), &iter)) {
            fieldNameValue = G_VALUE_INIT;
            
            gtk_tree_model_get_value(GTK_TREE_MODEL(_store), &iter, kStoreColumn_FieldName, &fieldNameValue);
            fieldName = g_value_get_string(&fieldNameValue);

            if (std::string(fieldName) == it->first) {
                printf("Storing %s\n", fieldName);

                fieldValueValue = G_VALUE_INIT;
                g_value_init(&fieldValueValue, G_TYPE_STRING);
                g_value_set_string(&fieldValueValue, it->second.c_str());
                gtk_tree_store_set_value(_store, &iter, kStoreColumn_FieldValue, &fieldValueValue);
                g_value_unset(&fieldValueValue);

                fieldValueValue = G_VALUE_INIT;
                g_value_init(&fieldValueValue, G_TYPE_STRING);
                g_value_set_string(&fieldValueValue, "Green");
                gtk_tree_store_set_value(_store, &iter, kStoreColumn_Color, &fieldValueValue);
                g_value_unset(&fieldValueValue);
            }
        }
    }

    if (_changedCellsColorResetTimeoutHandle) {
        g_source_remove(_changedCellsColorResetTimeoutHandle);
    }

    g_timeout_add(1500, ChangedCellsColorResetTimeout, this);
}

ViewController::ViewController(App *app, GtkApplication *gtkApp)
    : _changedCellsColorResetTimeoutHandle(0),
    _app(app)
{
    GtkTreeIter iter;

    _window = gtk_application_window_new (gtkApp);

    gtk_window_set_title (GTK_WINDOW (_window), "Light Switch");
    gtk_window_set_default_size (GTK_WINDOW (_window), 300, 200);
    gtk_widget_show_all (_window);

    _grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(_grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(_grid), 12);
    
    gtk_container_add(GTK_CONTAINER(_window), _grid);

    _image = gtk_image_new_from_file("LightSwitch.png");
    g_object_set (_image, "margin", 12, NULL);

    _imageEventBox = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(_imageEventBox), _image);
    gtk_grid_attach(GTK_GRID(_grid), _imageEventBox, 1, 1, 1, 2);
    
    g_signal_connect(_imageEventBox, "button_press_event", G_CALLBACK(SwitchPressed), this);

    //_numSubscribersLabel = gtk_label_new("Num Suscribers:");
    //gtk_grid_attach(GTK_GRID(_grid), _numSubscribersLabel, 3, 2, 1, 1);

    // Init the GTK tree store that houses the model for our data list
    _store = gtk_tree_store_new(kStoreColumn_NumColumns,
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_STRING);

    gtk_tree_store_append(_store, &iter, NULL);
    gtk_tree_store_set(_store, &iter,
                       kStoreColumn_Color, "LightSlateGray",
                       kStoreColumn_FieldName, "global_scene_control",
                       kStoreColumn_FieldValue, "true",
                       -1);
    gtk_tree_store_append(_store, &iter, NULL);
    gtk_tree_store_set(_store, &iter,
                       kStoreColumn_Color, "LightSlateGray",
                       kStoreColumn_FieldName, "on_time",
                       kStoreColumn_FieldValue, "30",
                       -1);
    gtk_tree_store_append(_store, &iter, NULL);
    gtk_tree_store_set(_store, &iter,
                       kStoreColumn_Color, "LightSlateGray",
                       kStoreColumn_FieldName, "off_wait_time",
                       kStoreColumn_FieldValue, "100",
                       -1);

    gtk_tree_store_append(_store, &iter, NULL);
    gtk_tree_store_set(_store, &iter,
                       kStoreColumn_Color, "LightSlateGray",
                       kStoreColumn_FieldName, "ramp_info.type",
                       kStoreColumn_FieldValue, "2",
                       -1);

    gtk_tree_store_append(_store, &iter, NULL);
    gtk_tree_store_set(_store, &iter,
                       kStoreColumn_Color, "LightSlateGray",
                       kStoreColumn_FieldName, "ramp_info.duration",
                       kStoreColumn_FieldValue, "60",
                       -1);
    
    _dataList = gtk_tree_view_new_with_model(GTK_TREE_MODEL(_store));

    _renderer = gtk_cell_renderer_text_new();
    _column = gtk_tree_view_column_new_with_attributes ("Field Name",
                                                       _renderer,
                                                       "text", kStoreColumn_FieldName,
                                                       NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(_dataList), _column); 

    _column = gtk_tree_view_column_new_with_attributes ("Value",
                                                       _renderer,
                                                       "text", kStoreColumn_FieldValue,
                                                       NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(_dataList), _column); 

    // Setup the colors
    _column = gtk_tree_view_column_new_with_attributes ("Value",
                                                       _renderer,
                                                       "text", kStoreColumn_FieldValue,
                                                       "cell-background",
                                                       kStoreColumn_Color,
                                                       NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(_dataList), _column); 

    SetMargin(_dataList);
    SetMargin(_imageEventBox);

    GtkTreeSelection *select;
    
    gtk_grid_attach(GTK_GRID(_grid), _dataList, 2, 1, 1, 2);

    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    
    _cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(_cssProvider, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(_cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show_all (_window);
}
