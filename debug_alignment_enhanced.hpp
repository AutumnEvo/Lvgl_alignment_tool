/*
File:   debug_alignment_enhanced.hpp
Author: Will Jenkins
Purpose: Enhanced debug alignment overlay with widget spawning and management
*/

#pragma once

#include "lvgl/lvgl.h"
#include <functional>
#include <vector>
#include <string>

// Enable/disable debug alignment overlay globally
#define ENABLE_DEBUG_ALIGNMENT 1

class DebugAlignmentEnhanced {
public:
    // Widget types we can spawn
    enum class WidgetType {
        ARC,
        BAR,
        BUTTON,
        CANVAS,
        CHART,
        CHECKBOX,
        LED,
        MESSAGE_BOX,
        SLIDER,
        SPIN_BOX,
        SPINNER,
        SWITCH,
        TABLE,
        PIE_CHART
    };

    // Spawned object data
    struct SpawnedObject {
        WidgetType type;
        lv_obj_t* object;
        std::string name;
        int index;
        
        // Widget-specific properties
        struct {
            // Arc properties
            int start_angle = 0;
            int end_angle = 280;
            int arc_width = 22;
            int track_width = 5;
            bool track_visible = true;
            bool rounded_caps = false;
            int value = 50;
            int range_min = 0;
            int range_max = 100;
            int rotation = 0;
            int pad = 0;
            
            // Bar/Slider properties
            int min_value = 0;
            int max_value = 100;
            int current_value = 50;
            bool horizontal = true;
            int indicator_width = 20;
            int bar_mode = 0; // 0=normal, 1=range
            
            // Button/Switch/Checkbox/LED properties
            bool state = false;
            bool checked = false;
            bool disabled = false;
            std::string label_text = "Label";
            int pad_all = 5;
            int brightness = 255; // LED
            int color_index = 0;  // LED color cycling
            
            // Canvas properties
            int canvas_width = 200;
            int canvas_height = 150;
            int fill_color = 0x000000;
            bool test_drawing = false;
            
            // Chart properties
            int chart_points = 10;
            int chart_type = 0; // 0=line, 1=bar, 2=scatter
            int y_min = 0;
            int y_max = 100;
            int h_div_lines = 5;
            int v_div_lines = 5;
            int update_mode = 0; // 0=circular, 1=shift
            int series_count = 1;
            
            // Table properties
            int table_rows = 3;
            int table_cols = 3;
            int selected_row = 0;
            int selected_col = 0;
            int col_width = 80;
            int cell_align = 0; // 0=left, 1=center, 2=right
            
            // Spinner properties
            int spinner_speed = 1000;
            int spinner_arc_length = 60;
            
            // Spinbox properties
            int spinbox_min = 0;
            int spinbox_max = 100;
            int spinbox_step = 1;
            bool spinbox_rollover = false;
            int spinbox_digits = 4;
            int spinbox_sep_pos = 2;
            
            // Switch properties
            int anim_time = 200;
            int knob_pad = 2;
            
            // Message Box properties
            std::string msgbox_title = "Title";
            std::string msgbox_text = "Message";
            bool msgbox_modal = false;
            int msgbox_button_count = 2;
            
            // Pie chart properties
            int pie_segments = 4;
            bool pie_normalize = true;
            int pie_start_angle = 0;
            int pie_gap_angle = 2;
            int pie_donut_thickness = 0; // 0 = full pie
            bool pie_antialias = true;
        } props;
        
        // Common transform properties
        struct {
            int x_offset = 0;
            int y_offset = 0;
            int rotation = 0;
            int width = 200;
            int height = 200;
            int pad = 0;
        } transform;
    };

    // Current UI state
    enum class UIState {
        SPAWN_MENU,      // Show widget spawn buttons
        OBJECT_CONTROLS  // Show controls for active object
    };

private:
    static lv_obj_t* debug_panel;
    static lv_obj_t* toggle_button;
    static bool panel_visible;
    static UIState current_state;
    static std::vector<SpawnedObject> spawned_objects;
    static int active_object_index;
    static lv_obj_t* parent_screen;
    
    // UI components
    static lv_obj_t* content_area;
    static lv_obj_t* object_list_area;
    static lv_obj_t* controls_area;
    
    // Create UI components
    static void create_spawn_menu();
    static void create_object_controls();
    static void create_object_list();
    static void update_object_list();
    static void update_controls_for_active_object();
    
    // Modern UI helpers
    static lv_obj_t* create_modern_button(lv_obj_t* parent, const char* text, lv_event_cb_t callback, uint32_t color, int width, int height);
    static void create_transform_controls(lv_obj_t* parent);
    static void create_style_controls(lv_obj_t* parent);
    static void create_widget_specific_controls(lv_obj_t* parent);
    static lv_obj_t* create_control_section(lv_obj_t* parent, const char* title, int x, int y);
    
    // Widget spawning
    static void spawn_widget(WidgetType type);
    static lv_obj_t* create_widget_instance(WidgetType type, lv_obj_t* parent);
    static std::string get_widget_type_name(WidgetType type);
    
    // Object management
    static void set_active_object(int index);
    static void delete_active_object();
    static void apply_transform_to_object(SpawnedObject& obj);
    static void apply_properties_to_object(SpawnedObject& obj);
    
    // Event handlers
    static void toggle_panel_event(lv_event_t* e);
    static void back_to_menu_event(lv_event_t* e);
    static void spawn_arc_event(lv_event_t* e);
    static void spawn_bar_event(lv_event_t* e);
    static void spawn_button_event(lv_event_t* e);
    static void spawn_canvas_event(lv_event_t* e);
    static void spawn_chart_event(lv_event_t* e);
    static void spawn_checkbox_event(lv_event_t* e);
    static void spawn_led_event(lv_event_t* e);
    static void spawn_message_box_event(lv_event_t* e);
    static void spawn_slider_event(lv_event_t* e);
    static void spawn_spin_box_event(lv_event_t* e);
    static void spawn_spinner_event(lv_event_t* e);
    static void spawn_switch_event(lv_event_t* e);
    static void spawn_table_event(lv_event_t* e);
    static void spawn_pie_chart_event(lv_event_t* e);
    
    // Transform controls
    static void move_left_event(lv_event_t* e);
    static void move_right_event(lv_event_t* e);
    static void move_up_event(lv_event_t* e);
    static void move_down_event(lv_event_t* e);
    static void move_left_fast_event(lv_event_t* e);
    static void move_right_fast_event(lv_event_t* e);
    static void move_up_fast_event(lv_event_t* e);
    static void move_down_fast_event(lv_event_t* e);
    // Snap helpers: position object to parent edges
    static void snap_left_event(lv_event_t* e);
    static void snap_right_event(lv_event_t* e);
    static void snap_top_event(lv_event_t* e);
    static void snap_bottom_event(lv_event_t* e);
    static void snap_center_h_event(lv_event_t* e);
    static void snap_center_v_event(lv_event_t* e);
    static void snap_center_event(lv_event_t* e);
    // Grid snapping
    static bool grid_snap_enabled;
    static int grid_size;
    static void snap_to_grid_event(lv_event_t* e);
    static void grid_size_plus_event(lv_event_t* e);
    static void grid_size_minus_event(lv_event_t* e);
    static void rotate_cw_event(lv_event_t* e);
    static void rotate_ccw_event(lv_event_t* e);
    static void resize_wider_event(lv_event_t* e);
    static void resize_narrower_event(lv_event_t* e);
    static void resize_taller_event(lv_event_t* e);
    static void resize_shorter_event(lv_event_t* e);
    static void bring_to_front_event(lv_event_t* e);
    static void send_to_back_event(lv_event_t* e);
    static void delete_active_event(lv_event_t* e);
    static void export_values_event(lv_event_t* e);
    
    // Global style and visibility controls
    static void toggle_visibility_event(lv_event_t* e);
    static void border_width_plus_event(lv_event_t* e);
    static void border_width_minus_event(lv_event_t* e);
    static void radius_plus_event(lv_event_t* e);
    static void radius_minus_event(lv_event_t* e);
    static void opacity_plus_event(lv_event_t* e);
    static void opacity_minus_event(lv_event_t* e);
    
    // Widget-specific property controls
    // Arc controls
    static void arc_start_angle_plus_event(lv_event_t* e);
    static void arc_start_angle_minus_event(lv_event_t* e);
    static void arc_end_angle_plus_event(lv_event_t* e);
    static void arc_end_angle_minus_event(lv_event_t* e);
    static void arc_width_plus_event(lv_event_t* e);
    static void arc_width_minus_event(lv_event_t* e);
    static void arc_toggle_caps_event(lv_event_t* e);
    static void arc_value_plus_event(lv_event_t* e);
    static void arc_value_minus_event(lv_event_t* e);
    static void arc_range_min_plus_event(lv_event_t* e);
    static void arc_range_min_minus_event(lv_event_t* e);
    static void arc_range_max_plus_event(lv_event_t* e);
    static void arc_range_max_minus_event(lv_event_t* e);
    static void arc_rotation_plus_event(lv_event_t* e);
    static void arc_rotation_minus_event(lv_event_t* e);
    static void arc_track_width_plus_event(lv_event_t* e);
    static void arc_track_width_minus_event(lv_event_t* e);
    static void arc_toggle_track_event(lv_event_t* e);
    static void arc_pad_plus_event(lv_event_t* e);
    static void arc_pad_minus_event(lv_event_t* e);
    
    // Bar/Slider controls
    static void bar_min_plus_event(lv_event_t* e);
    static void bar_min_minus_event(lv_event_t* e);
    static void bar_max_plus_event(lv_event_t* e);
    static void bar_max_minus_event(lv_event_t* e);
    static void bar_value_plus_event(lv_event_t* e);
    static void bar_value_minus_event(lv_event_t* e);
    static void bar_toggle_orientation_event(lv_event_t* e);
    static void bar_toggle_mode_event(lv_event_t* e);
    
    // Button/State controls
    static void button_toggle_state_event(lv_event_t* e);
    static void button_toggle_checked_event(lv_event_t* e);
    static void button_toggle_disabled_event(lv_event_t* e);
    static void button_edit_text_event(lv_event_t* e);
    static void button_pad_plus_event(lv_event_t* e);
    static void button_pad_minus_event(lv_event_t* e);
    
    // Canvas controls
    static void canvas_fill_color_event(lv_event_t* e);
    static void canvas_clear_event(lv_event_t* e);
    static void canvas_draw_test_event(lv_event_t* e);
    static void canvas_recreate_event(lv_event_t* e);
    
    // Chart controls
    static void chart_toggle_type_event(lv_event_t* e);
    static void chart_add_series_event(lv_event_t* e);
    static void chart_remove_series_event(lv_event_t* e);
    static void chart_point_count_plus_event(lv_event_t* e);
    static void chart_point_count_minus_event(lv_event_t* e);
    static void chart_y_min_plus_event(lv_event_t* e);
    static void chart_y_min_minus_event(lv_event_t* e);
    static void chart_y_max_plus_event(lv_event_t* e);
    static void chart_y_max_minus_event(lv_event_t* e);
    static void chart_div_lines_plus_event(lv_event_t* e);
    static void chart_div_lines_minus_event(lv_event_t* e);
    static void chart_toggle_update_mode_event(lv_event_t* e);
    
    // Checkbox controls
    static void checkbox_toggle_state_event(lv_event_t* e);
    static void checkbox_toggle_disabled_event(lv_event_t* e);
    static void checkbox_edit_text_event(lv_event_t* e);
    
    // LED controls
    static void led_toggle_state_event(lv_event_t* e);
    static void led_brightness_plus_event(lv_event_t* e);
    static void led_brightness_minus_event(lv_event_t* e);
    static void led_color_cycle_event(lv_event_t* e);
    
    // Message Box controls
    static void msgbox_edit_title_event(lv_event_t* e);
    static void msgbox_edit_text_event(lv_event_t* e);
    static void msgbox_add_button_event(lv_event_t* e);
    static void msgbox_toggle_modal_event(lv_event_t* e);
    static void msgbox_close_event(lv_event_t* e);
    
    // Spinbox controls
    static void spinbox_range_min_plus_event(lv_event_t* e);
    static void spinbox_range_min_minus_event(lv_event_t* e);
    static void spinbox_range_max_plus_event(lv_event_t* e);
    static void spinbox_range_max_minus_event(lv_event_t* e);
    static void spinbox_step_plus_event(lv_event_t* e);
    static void spinbox_step_minus_event(lv_event_t* e);
    static void spinbox_toggle_rollover_event(lv_event_t* e);
    static void spinbox_increment_event(lv_event_t* e);
    static void spinbox_decrement_event(lv_event_t* e);
    
    // Spinner controls
    static void spinner_speed_plus_event(lv_event_t* e);
    static void spinner_speed_minus_event(lv_event_t* e);
    static void spinner_arc_length_plus_event(lv_event_t* e);
    static void spinner_arc_length_minus_event(lv_event_t* e);
    static void spinner_recreate_event(lv_event_t* e);
    
    // Switch controls
    static void switch_toggle_state_event(lv_event_t* e);
    static void switch_anim_time_plus_event(lv_event_t* e);
    static void switch_anim_time_minus_event(lv_event_t* e);
    static void switch_knob_pad_plus_event(lv_event_t* e);
    static void switch_knob_pad_minus_event(lv_event_t* e);
    
    // Table controls
    static void table_rows_plus_event(lv_event_t* e);
    static void table_rows_minus_event(lv_event_t* e);
    static void table_cols_plus_event(lv_event_t* e);
    static void table_cols_minus_event(lv_event_t* e);
    static void table_edit_cell_event(lv_event_t* e);
    static void table_toggle_align_event(lv_event_t* e);
    static void table_col_width_plus_event(lv_event_t* e);
    static void table_col_width_minus_event(lv_event_t* e);
    
    // Pie Chart (custom) controls
    static void pie_add_segment_event(lv_event_t* e);
    static void pie_remove_segment_event(lv_event_t* e);
    static void pie_toggle_normalize_event(lv_event_t* e);
    static void pie_start_angle_plus_event(lv_event_t* e);
    static void pie_start_angle_minus_event(lv_event_t* e);
    static void pie_gap_angle_plus_event(lv_event_t* e);
    static void pie_gap_angle_minus_event(lv_event_t* e);
    static void pie_donut_thickness_plus_event(lv_event_t* e);
    static void pie_donut_thickness_minus_event(lv_event_t* e);
    static void pie_toggle_antialias_event(lv_event_t* e);
    
    // Helper functions
    static lv_obj_t* create_button(lv_obj_t* parent, const char* text, lv_event_cb_t callback, int x, int y, int w = 100, int h = 40);
    static void clear_content_area();
    static void export_object_values(const SpawnedObject& obj);
    // Section toggle helper
    static void toggle_section_event(lv_event_t* e);

    // Widget-specific controls parent (used so we can create a nested container)
    static lv_obj_t* widget_controls_parent;

    // Persistent grid label pointer so grid toggles update in-place
    static lv_obj_t* grid_label_ptr;

public:
    // Initialize the debug alignment system on any screen
    static void init(lv_obj_t* screen);
    
    // Toggle the debug panel
    static void toggle();
    
    // Clean up when changing screens
    static void cleanup();
    
    // Get current spawned objects count
    static int get_spawned_count() { return spawned_objects.size(); }
    
    // Get active object (if any)
    static SpawnedObject* get_active_object();
};
