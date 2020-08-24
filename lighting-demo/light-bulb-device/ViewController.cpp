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

void
ViewController::BulbPressed(GtkWidget *button, GdkEventButton *event, void *data)
{
    ViewController *_this = static_cast<ViewController *>(data);

    // This ensures that Weave wont' start making calls to GetLeafData on the source mid-way through 
    // setting the below properties.
    _this->_dataSource->Lock();

    DEBUG_PRINT(kAPP, "Setting trait handler on/off");

    _this->_dataSource->_onOff = !_this->_dataSource->_onOff;

    if (_this->_dataSource->_onOff) {
        gtk_image_set_from_file(GTK_IMAGE(_this->_image), "LightBulb-on.png");
    }
    else {
        gtk_image_set_from_file(GTK_IMAGE(_this->_image), "LightBulb.png");
    }

    // Convey to WDM that we've now modified this property
    _this->_dataSource->SetDirty(Schema::Zigbee::Cluster::General::OnOffCluster::kPropertyHandle_OnOff);
    _this->_dataSource->Unlock();
   
    // Trigger the notification engine to start processing these changes. 
    nl::Weave::Profiles::DataManagement::SubscriptionEngine::GetInstance()->GetNotificationEngine()->Run();
}

void
ViewController::FlushChangesBtnPressed(GtkWidget *button, GdkEventButton *event, void *data)
{
    ViewController *_this = static_cast<ViewController *>(data);

    _this->_dataSource->Lock();

    for (auto it = _this->_changedItems.begin(); it != _this->_changedItems.end(); it++) {
        DEBUG_PRINT(kAPP, "Setting trait handler %s", it->first.c_str());
        
        if (strstr(it->first.c_str(), "global_scene_control")) {
            if (strstr(it->second.c_str(), "true")) {
                _this->_dataSource->_globalSceneControl = true;
            }
            else {
                _this->_dataSource->_globalSceneControl = false;
            }

            _this->_dataSource->SetDirty(Schema::Zigbee::Cluster::General::OnOffCluster::kPropertyHandle_GlobalSceneControl);
        }

        if (strstr(it->first.c_str(), "on_time")) {
            _this->_dataSource->_onTime = stoi(it->second);
            _this->_dataSource->SetDirty(Schema::Zigbee::Cluster::General::OnOffCluster::kPropertyHandle_OnTime);
        }

        if (strstr(it->first.c_str(), "off_wait_time")) {
            _this->_dataSource->_offWaitTime= stoi(it->second);
            _this->_dataSource->SetDirty(Schema::Zigbee::Cluster::General::OnOffCluster::kPropertyHandle_OffWaitTime);
        }

        if (strstr(it->first.c_str(), "ramp_info.type")) {
            _this->_dataSource->_rampInfo.type = stoi(it->second);
            _this->_dataSource->SetDirty(Schema::Zigbee::Cluster::General::OnOffCluster::kPropertyHandle_RampInfo_Type);
        }

        if (strstr(it->first.c_str(), "ramp_info.duration")) {
            _this->_dataSource->_rampInfo.rampDuration = stoi(it->second);
            _this->_dataSource->SetDirty(Schema::Zigbee::Cluster::General::OnOffCluster::kPropertyHandle_RampInfo_RampDuration);
        }
    }

    _this->_changedItems.clear();

    _this->_dataSource->Unlock();
    
    nl::Weave::Profiles::DataManagement::SubscriptionEngine::GetInstance()->GetNotificationEngine()->Run();
}

//
// Really lame way of setting up some margin around the widget
//
void SetMargin(GtkWidget *widget)
{
    gtk_widget_set_margin_top(widget, 20);
    gtk_widget_set_margin_bottom(widget, 20);
    gtk_widget_set_margin_start(widget, 20);
    gtk_widget_set_margin_end(widget, 20);
}

void
ViewController::CellEditedCallback(GtkCellRendererText *cell,
                                  gchar               *path_string,
                                  gchar               *new_text,
                                  gpointer             user_data)
{
    ViewController *_this = (ViewController *)user_data;
    GtkTreeIter iter;
    GValue val = G_VALUE_INIT;
    const gchar *fieldName;

    g_value_init(&val, G_TYPE_STRING);
    g_value_set_string(&val, new_text);

    // Just clicking on the cell and editing it won't automatically change the contents. We have to go
    // and update the store manually.
    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(_this->_store), &iter, path_string)) {
        gtk_tree_store_set_value(_this->_store, &iter, kStoreColumn_FieldValue, &val);
        g_value_unset(&val);

        val = G_VALUE_INIT;
        gtk_tree_model_get_value(GTK_TREE_MODEL(_this->_store), &iter, kStoreColumn_FieldName, &val);
        fieldName = g_value_get_string(&val);

        _this->_changedItems[std::string(fieldName)] = std::string(new_text);
    }

    g_value_unset(&val);
}

ViewController::ViewController(GtkApplication *app, ZclOnOffClusterSource *dataSource)
    : _changeFlushTimerHandle(0),
    _dataSource(dataSource)
{
    GtkTreeIter iter;

    _window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (_window), "Light Bulb");
    gtk_window_set_default_size (GTK_WINDOW (_window), 500, 200);
    gtk_widget_show_all (_window);

    _grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(_grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(_grid), 12);
    
    gtk_container_add(GTK_CONTAINER(_window), _grid);

    _flushChangesButton = gtk_button_new_with_label("Flush Changes");
    gtk_grid_attach(GTK_GRID(_grid), _flushChangesButton, 3, 1, 1, 2);
    g_signal_connect(_flushChangesButton, "button_press_event", G_CALLBACK(FlushChangesBtnPressed), this);
    

    _image = gtk_image_new_from_file("LightBulb.png");
    g_object_set (_image, "margin", 12, NULL);

    _imageEventBox = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(_imageEventBox), _image);
    gtk_grid_attach(GTK_GRID(_grid), _imageEventBox, 1, 1, 1, 2);
    
    g_signal_connect(_imageEventBox, "button_press_event", G_CALLBACK(BulbPressed), this);

    //_numSubscribersLabel = gtk_label_new("Num Suscribers:");
    //gtk_grid_attach(GTK_GRID(_grid), _numSubscribersLabel, 3, 2, 1, 1);

    // Init the GTK tree store that houses the model for our data list
    _store = gtk_tree_store_new(kStoreColumn_NumColumns,
                                G_TYPE_STRING,
                                G_TYPE_STRING);
    
    gtk_tree_store_append(_store, &iter, NULL);
    gtk_tree_store_set(_store, &iter,
                       kStoreColumn_FieldName, "global_scene_control",
                       kStoreColumn_FieldValue, "true",
                       -1);
    gtk_tree_store_append(_store, &iter, NULL);
    gtk_tree_store_set(_store, &iter,
                       kStoreColumn_FieldName, "on_time",
                       kStoreColumn_FieldValue, "30",
                       -1);
    gtk_tree_store_append(_store, &iter, NULL);
    gtk_tree_store_set(_store, &iter,
                       kStoreColumn_FieldName, "off_wait_time",
                       kStoreColumn_FieldValue, "100",
                       -1);

    gtk_tree_store_append(_store, &iter, NULL);
    gtk_tree_store_set(_store, &iter,
                       kStoreColumn_FieldName, "ramp_info.type",
                       kStoreColumn_FieldValue, "2",
                       -1);

    gtk_tree_store_append(_store, &iter, NULL);
    gtk_tree_store_set(_store, &iter,
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

    _renderer = gtk_cell_renderer_text_new();
    g_object_set(_renderer, "editable", TRUE, NULL);

    g_signal_connect(_renderer, "edited", (GCallback)CellEditedCallback, this);
    
    _column = gtk_tree_view_column_new_with_attributes ("Value",
                                                       _renderer,
                                                       "text", kStoreColumn_FieldValue,
                                                       NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(_dataList), _column); 

    SetMargin(_dataList);
    SetMargin(_imageEventBox);
    SetMargin(_flushChangesButton);

    GtkTreeSelection *select;
    
    gtk_grid_attach(GTK_GRID(_grid), _dataList, 2, 1, 1, 2);

    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    
    _cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(_cssProvider, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(_cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show_all (_window);
}
