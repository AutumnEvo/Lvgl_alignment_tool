/*
File:   debug_alignment_enhanced.cpp
Author: Will Jenkins
Purpose: Enhanced debug alignment overlay implementation with widget spawning
*/

#include "debug_alignment_enhanced.hpp"
#include <cstdio>
#include <algorithm>

// Static member definitions
lv_obj_t* DebugAlignmentEnhanced::debug_panel = nullptr;
lv_obj_t* DebugAlignmentEnhanced::toggle_button = nullptr;
bool DebugAlignmentEnhanced::panel_visible = false;
DebugAlignmentEnhanced::UIState DebugAlignmentEnhanced::current_state = UIState::SPAWN_MENU;
std::vector<DebugAlignmentEnhanced::SpawnedObject> DebugAlignmentEnhanced::spawned_objects;
int DebugAlignmentEnhanced::active_object_index = -1;
lv_obj_t* DebugAlignmentEnhanced::parent_screen = nullptr;
lv_obj_t* DebugAlignmentEnhanced::content_area = nullptr;
lv_obj_t* DebugAlignmentEnhanced::object_list_area = nullptr;
lv_obj_t* DebugAlignmentEnhanced::controls_area = nullptr;

void DebugAlignmentEnhanced::init(lv_obj_t* screen) {
#if ENABLE_DEBUG_ALIGNMENT
    cleanup(); // Clean up any existing instances
    parent_screen = screen;
    
    // Create toggle button (always visible in top-right corner with cherry blossom styling)
    toggle_button = lv_btn_create(screen);
    lv_obj_set_size(toggle_button, 100, 50);
    lv_obj_align(toggle_button, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_add_event_cb(toggle_button, toggle_panel_event, LV_EVENT_CLICKED, nullptr);
    
    // Cherry blossom pink gradient styling
    lv_obj_set_style_bg_color(toggle_button, lv_color_hex(0xFFB6C1), LV_PART_MAIN);  // Light pink
    lv_obj_set_style_bg_grad_color(toggle_button, lv_color_hex(0xFF69B4), LV_PART_MAIN);  // Hot pink
    lv_obj_set_style_bg_grad_dir(toggle_button, LV_GRAD_DIR_VER, LV_PART_MAIN);
    lv_obj_set_style_radius(toggle_button, 15, LV_PART_MAIN);
    lv_obj_set_style_border_color(toggle_button, lv_color_hex(0xFF1493), LV_PART_MAIN);  // Deep pink border
    lv_obj_set_style_border_width(toggle_button, 2, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(toggle_button, lv_color_hex(0xFF69B4), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(toggle_button, 5, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(toggle_button, LV_OPA_30, LV_PART_MAIN);
    
    lv_obj_t* toggle_label = lv_label_create(toggle_button);
    lv_label_set_text(toggle_label, "🌸 DBG");
    lv_obj_center(toggle_label);
    lv_obj_set_style_text_color(toggle_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);  // White text
    lv_obj_set_style_text_font(toggle_label, &lv_font_montserrat_20, LV_PART_MAIN);
    
    // Bring the button to the front to ensure it's visible
    lv_obj_move_foreground(toggle_button);
    
    // Create main debug panel (initially hidden) - bigger and more transparent
    debug_panel = lv_obj_create(screen);
    lv_obj_set_size(debug_panel, 650, 800);  // Made even bigger: 650x800 for more controls
    lv_obj_align(debug_panel, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_set_style_bg_color(debug_panel, lv_color_hex(0x1A1A1A), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(debug_panel, LV_OPA_20, LV_PART_MAIN);  // More transparent: 20% instead of 40%
    lv_obj_set_style_border_color(debug_panel, lv_color_hex(0xFF69B4), LV_PART_MAIN);  // Cherry pink border
    lv_obj_set_style_border_width(debug_panel, 2, LV_PART_MAIN);
    lv_obj_set_style_pad_all(debug_panel, 10, LV_PART_MAIN);
    lv_obj_add_flag(debug_panel, LV_OBJ_FLAG_HIDDEN);
    
    // Create object list area at top
    object_list_area = lv_obj_create(debug_panel);
    lv_obj_set_size(object_list_area, LV_PCT(100), 80);  // Made taller: 80 instead of 60
    lv_obj_align(object_list_area, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(object_list_area, lv_color_hex(0x2A2A2A), LV_PART_MAIN);
    lv_obj_set_style_pad_all(object_list_area, 5, LV_PART_MAIN);
    
    // Create main content area with scrolling support
    content_area = lv_obj_create(debug_panel);
    lv_obj_set_size(content_area, LV_PCT(100), 700);  // Much taller for all controls: 700 instead of 600
    lv_obj_align(content_area, LV_ALIGN_TOP_MID, 0, 90);  // Adjusted position: 90 instead of 70
    lv_obj_set_style_bg_color(content_area, lv_color_hex(0x2A2A2A), LV_PART_MAIN);
    lv_obj_set_style_pad_all(content_area, 5, LV_PART_MAIN);
    
    // Enable scrolling for the content area so all controls are accessible
    lv_obj_set_scroll_dir(content_area, LV_DIR_VER);  // Vertical scrolling only
    lv_obj_set_scrollbar_mode(content_area, LV_SCROLLBAR_MODE_AUTO);  // Show scrollbar when needed
    lv_obj_set_style_pad_right(content_area, 15, LV_PART_MAIN);  // Extra padding for scrollbar
    
    // Initialize with spawn menu
    current_state = UIState::SPAWN_MENU;
    create_object_list();
    create_spawn_menu();
    
    printf("\n🌸 [ENHANCED DEBUG] Cherry Blossom Debug Alignment initialized! Look for the pink button in top-right! 🌸\n");
    printf("[ENHANCED DEBUG] Button size: 100x50, Position: top-right corner\n\n");
#endif
}

void DebugAlignmentEnhanced::toggle() {
    if (!debug_panel) return;
    
    if (panel_visible) {
        lv_obj_add_flag(debug_panel, LV_OBJ_FLAG_HIDDEN);
        panel_visible = false;
    } else {
        lv_obj_clear_flag(debug_panel, LV_OBJ_FLAG_HIDDEN);
        panel_visible = true;
        update_object_list();
    }
}

void DebugAlignmentEnhanced::cleanup() {
    // Delete all spawned objects
    for (auto& obj : spawned_objects) {
        if (obj.object) {
            lv_obj_delete(obj.object);
        }
    }
    spawned_objects.clear();
    active_object_index = -1;
    
    if (debug_panel) {
        lv_obj_delete(debug_panel);
        debug_panel = nullptr;
    }
    if (toggle_button) {
        lv_obj_delete(toggle_button);
        toggle_button = nullptr;
    }
    
    content_area = nullptr;
    object_list_area = nullptr;
    controls_area = nullptr;
    panel_visible = false;
    current_state = UIState::SPAWN_MENU;
}

void DebugAlignmentEnhanced::create_spawn_menu() {
    clear_content_area();
    
    lv_obj_t* title = lv_label_create(content_area);
    lv_label_set_text(title, "Spawn Widgets");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    
    // Create spawn buttons in a grid - Made 2x bigger!
    const int btn_w = 160, btn_h = 70;  // 2x bigger: was 80x35, now 160x70
    const int cols = 3;  // Reduced columns since buttons are bigger
    const int start_y = 40;
    
    struct {
        const char* name;
        lv_event_cb_t callback;
    } widgets[] = {
        {"Arc", spawn_arc_event},
        {"Bar", spawn_bar_event},
        {"Button", spawn_button_event},
        {"Canvas", spawn_canvas_event},
        {"Chart", spawn_chart_event},
        {"Checkbox", spawn_checkbox_event},
        {"LED", spawn_led_event},
        {"MsgBox", spawn_message_box_event},
        {"Slider", spawn_slider_event},
        {"SpinBox", spawn_spin_box_event},
        {"Spinner", spawn_spinner_event},
        {"Switch", spawn_switch_event},
        {"Table", spawn_table_event},
        {"PieChart", spawn_pie_chart_event}
    };
    
    for (int i = 0; i < 14; i++) {
        int row = i / cols;
        int col = i % cols;
        int x = 10 + col * (btn_w + 10);
        int y = start_y + row * (btn_h + 10);
        
        create_button(content_area, widgets[i].name, widgets[i].callback, x, y, btn_w, btn_h);
    }
}

void DebugAlignmentEnhanced::create_object_controls() {
    clear_content_area();
    
    if (active_object_index < 0 || active_object_index >= (int)spawned_objects.size()) {
        current_state = UIState::SPAWN_MENU;
        create_spawn_menu();
        return;
    }
    
    SpawnedObject& obj = spawned_objects[active_object_index];
    
    // Title
    lv_obj_t* title = lv_label_create(content_area);
    char title_text[64];
    snprintf(title_text, sizeof(title_text), "Controls: %s", obj.name.c_str());
    lv_label_set_text(title, title_text);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    
    // Back button - Made 2x bigger!
    create_button(content_area, "Back", back_to_menu_event, 10, 30, 120, 60);  // 2x: was 60x30, now 120x60
    
    // Delete button - Made 2x bigger!
    create_button(content_area, "Delete", delete_active_event, 140, 30, 120, 60);  // 2x: was 60x30, now 120x60
    
    // Export button - Made 2x bigger!
    create_button(content_area, "Export", export_values_event, 270, 30, 120, 60);  // 2x: was 60x30, now 120x60
    
    // Common transform controls
    int y_pos = 110;  // Adjusted for bigger buttons
    
    // Movement controls
    lv_obj_t* move_label = lv_label_create(content_area);
    lv_label_set_text(move_label, "Movement:");
    lv_obj_set_pos(move_label, 10, y_pos);
    lv_obj_set_style_text_color(move_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(move_label, &lv_font_montserrat_20, LV_PART_MAIN);  // 20px font
    y_pos += 25;
    
    // Movement buttons - Made 2x bigger! Arranged in two rows to prevent overlapping
    // Row 1: Basic movements
    create_button(content_area, "L", move_left_event, 10, y_pos, 60, 60);  // 2x: was 30x30, now 60x60
    create_button(content_area, "R", move_right_event, 80, y_pos, 60, 60);  // 2x: was 30x30, now 60x60
    create_button(content_area, "U", move_up_event, 150, y_pos, 60, 60);  // 2x: was 30x30, now 60x60
    create_button(content_area, "D", move_down_event, 220, y_pos, 60, 60);  // 2x: was 30x30, now 60x60
    
    // Row 2: Fast movements (15px)
    create_button(content_area, "L15", move_left_fast_event, 10, y_pos + 70, 80, 60);  // 2x: was 40x30, now 80x60
    create_button(content_area, "R15", move_right_fast_event, 100, y_pos + 70, 80, 60);  // 2x: was 40x30, now 80x60
    create_button(content_area, "U15", move_up_fast_event, 190, y_pos + 70, 80, 60);  // 2x: was 40x30, now 80x60
    create_button(content_area, "D15", move_down_fast_event, 280, y_pos + 70, 80, 60);  // 2x: was 40x30, now 80x60
    y_pos += 140;  // Adjusted for bigger buttons
    
    // Rotation controls
    lv_obj_t* rot_label = lv_label_create(content_area);
    lv_label_set_text(rot_label, "Rotation:");
    lv_obj_set_pos(rot_label, 10, y_pos);
    lv_obj_set_style_text_color(rot_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(rot_label, &lv_font_montserrat_20, LV_PART_MAIN);  // 20px font
    y_pos += 25;
    
    // Rotation buttons - Made 2x bigger!
    create_button(content_area, "CW", rotate_cw_event, 10, y_pos, 80, 60);  // 2x: was 40x30, now 80x60
    create_button(content_area, "CCW", rotate_ccw_event, 100, y_pos, 80, 60);  // 2x: was 40x30, now 80x60
    y_pos += 80;  // Adjusted for bigger buttons
    
    // Size controls (Arc widgets use size, not separate width/height)
    lv_obj_t* size_label = lv_label_create(content_area);
    
    // Check if the selected object is an Arc widget
    SpawnedObject* active_obj = get_active_object();
    bool is_arc_widget = (active_obj && active_obj->type == WidgetType::ARC);
    
    if (is_arc_widget) {
        lv_label_set_text(size_label, "Size (Arc uses single size value):");
    } else {
        lv_label_set_text(size_label, "Size:");
    }
    lv_obj_set_pos(size_label, 10, y_pos);
    lv_obj_set_style_text_color(size_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    y_pos += 25;

    if (is_arc_widget) {
        // Arc size controls - single size value affects both dimensions
        create_button(content_area, "Size+", resize_wider_event, 10, y_pos, 120, 60);  // Reuse width event for arc size
        create_button(content_area, "Size-", resize_narrower_event, 140, y_pos, 120, 60);  // Reuse width event for arc size
    } else {
        // Regular width/height controls for other widgets
        create_button(content_area, "W+", resize_wider_event, 10, y_pos, 80, 60);  // 2x: was 40x30, now 80x60
        create_button(content_area, "W-", resize_narrower_event, 100, y_pos, 80, 60);  // 2x: was 40x30, now 80x60
        create_button(content_area, "H+", resize_taller_event, 190, y_pos, 80, 60);  // 2x: was 40x30, now 80x60
        create_button(content_area, "H-", resize_shorter_event, 280, y_pos, 80, 60);  // 2x: was 40x30, now 80x60
    }
    y_pos += 80;  // Adjusted for bigger buttons
    
    // Z-order controls - Made 2x bigger!
    create_button(content_area, "Front", bring_to_front_event, 10, y_pos, 120, 60);  // 2x: was 60x30, now 120x60
    create_button(content_area, "Back", send_to_back_event, 140, y_pos, 120, 60);  // 2x: was 60x30, now 120x60
    y_pos += 80;  // Adjusted for bigger buttons
    
    // Global visibility & style controls
    lv_obj_t* style_label = lv_label_create(content_area);
    lv_label_set_text(style_label, "Visibility & Style:");
    lv_obj_set_pos(style_label, 10, y_pos);
    lv_obj_set_style_text_color(style_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(style_label, &lv_font_montserrat_20, LV_PART_MAIN);  // 20px font
    y_pos += 25;
    
    // Visibility buttons
    create_button(content_area, "Hide", toggle_visibility_event, 10, y_pos, 80, 60);
    create_button(content_area, "Show", toggle_visibility_event, 100, y_pos, 80, 60);
    y_pos += 70;
    
    // Style controls
    lv_obj_t* style2_label = lv_label_create(content_area);
    lv_label_set_text(style2_label, "Border & Opacity:");
    lv_obj_set_pos(style2_label, 10, y_pos);
    lv_obj_set_style_text_color(style2_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(style2_label, &lv_font_montserrat_20, LV_PART_MAIN);  // 20px font
    y_pos += 25;
    
    // Border and opacity controls
    create_button(content_area, "B+", border_width_plus_event, 10, y_pos, 80, 60);
    create_button(content_area, "B-", border_width_minus_event, 100, y_pos, 80, 60);
    create_button(content_area, "R+", radius_plus_event, 190, y_pos, 80, 60);
    create_button(content_area, "R-", radius_minus_event, 280, y_pos, 80, 60);
    create_button(content_area, "O+", opacity_plus_event, 370, y_pos, 80, 60);
    create_button(content_area, "O-", opacity_minus_event, 460, y_pos, 80, 60);
    y_pos += 80;
    
    // Widget-specific controls
    update_controls_for_active_object();
    
    // Set content area scrolling properties - ensure large enough content for all controls
    lv_obj_update_layout(content_area);  // Update layout before setting scroll height
}

void DebugAlignmentEnhanced::update_controls_for_active_object() {
    if (active_object_index < 0 || active_object_index >= (int)spawned_objects.size()) return;
    
    SpawnedObject& obj = spawned_objects[active_object_index];
    int y_pos = 450;  // Start widget-specific controls well below global controls
    
    // Widget-specific controls based on type
    switch (obj.type) {
        case WidgetType::ARC: {
            lv_obj_t* arc_label = lv_label_create(content_area);
            lv_label_set_text(arc_label, "Arc Properties:");
            lv_obj_set_pos(arc_label, 10, y_pos);
            lv_obj_set_style_text_color(arc_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            y_pos += 25;
            
            // Range controls
            lv_obj_t* range_label = lv_label_create(content_area);
            lv_label_set_text(range_label, "Range:");
            lv_obj_set_pos(range_label, 10, y_pos);
            lv_obj_set_style_text_color(range_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            y_pos += 20;
            
            create_button(content_area, "Min+", arc_range_min_plus_event, 10, y_pos, 80, 60);
            create_button(content_area, "Min-", arc_range_min_minus_event, 100, y_pos, 80, 60);
            create_button(content_area, "Max+", arc_range_max_plus_event, 190, y_pos, 80, 60);
            create_button(content_area, "Max-", arc_range_max_minus_event, 280, y_pos, 80, 60);
            y_pos += 70;
            
            // Value controls
            lv_obj_t* value_label = lv_label_create(content_area);
            lv_label_set_text(value_label, "Value:");
            lv_obj_set_pos(value_label, 10, y_pos);
            lv_obj_set_style_text_color(value_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            y_pos += 20;
            
            create_button(content_area, "V+", arc_value_plus_event, 10, y_pos, 80, 60);
            create_button(content_area, "V-", arc_value_minus_event, 100, y_pos, 80, 60);
            y_pos += 70;
            
            // Rotation controls (semantic)
            lv_obj_t* rot_label = lv_label_create(content_area);
            lv_label_set_text(rot_label, "Rotation:");
            lv_obj_set_pos(rot_label, 10, y_pos);
            lv_obj_set_style_text_color(rot_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            y_pos += 20;
            
            create_button(content_area, "R+", arc_rotation_plus_event, 10, y_pos, 80, 60);
            create_button(content_area, "R-", arc_rotation_minus_event, 100, y_pos, 80, 60);
            y_pos += 70;
            
            // Sweep angles
            lv_obj_t* sweep_label = lv_label_create(content_area);
            lv_label_set_text(sweep_label, "Sweep:");
            lv_obj_set_pos(sweep_label, 10, y_pos);
            lv_obj_set_style_text_color(sweep_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            y_pos += 20;
            
            create_button(content_area, "S+", arc_start_angle_plus_event, 10, y_pos, 80, 60);
            create_button(content_area, "S-", arc_start_angle_minus_event, 100, y_pos, 80, 60);
            create_button(content_area, "E+", arc_end_angle_plus_event, 190, y_pos, 80, 60);
            create_button(content_area, "E-", arc_end_angle_minus_event, 280, y_pos, 80, 60);
            y_pos += 70;
            
            // Indicator width
            lv_obj_t* width_label = lv_label_create(content_area);
            lv_label_set_text(width_label, "Indicator:");
            lv_obj_set_pos(width_label, 10, y_pos);
            lv_obj_set_style_text_color(width_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            y_pos += 20;
            
            create_button(content_area, "W+", arc_width_plus_event, 10, y_pos, 80, 60);
            create_button(content_area, "W-", arc_width_minus_event, 100, y_pos, 80, 60);
            create_button(content_area, "Caps", arc_toggle_caps_event, 190, y_pos, 80, 60);
            y_pos += 70;
            
            // Track controls
            lv_obj_t* track_label = lv_label_create(content_area);
            lv_label_set_text(track_label, "Track:");
            lv_obj_set_pos(track_label, 10, y_pos);
            lv_obj_set_style_text_color(track_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            y_pos += 20;
            
            create_button(content_area, "TW+", arc_track_width_plus_event, 10, y_pos, 80, 60);
            create_button(content_area, "TW-", arc_track_width_minus_event, 100, y_pos, 80, 60);
            create_button(content_area, "Hide", arc_toggle_track_event, 190, y_pos, 80, 60);
            y_pos += 70;
            
            // Padding
            lv_obj_t* pad_label = lv_label_create(content_area);
            lv_label_set_text(pad_label, "Pad:");
            lv_obj_set_pos(pad_label, 10, y_pos);
            lv_obj_set_style_text_color(pad_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            y_pos += 20;
            
            create_button(content_area, "P+", arc_pad_plus_event, 10, y_pos, 80, 60);
            create_button(content_area, "P-", arc_pad_minus_event, 100, y_pos, 80, 60);
            break;
        }
        
        case WidgetType::BAR:
        case WidgetType::SLIDER: {
            lv_obj_t* bar_label = lv_label_create(content_area);
            const char* widget_name = (obj.type == WidgetType::BAR) ? "Bar" : "Slider";
            char label_text[50];
            snprintf(label_text, sizeof(label_text), "%s Properties:", widget_name);
            lv_label_set_text(bar_label, label_text);
            lv_obj_set_pos(bar_label, 10, y_pos);
            lv_obj_set_style_text_color(bar_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            y_pos += 25;
            
            // Range controls
            lv_obj_t* range_label = lv_label_create(content_area);
            lv_label_set_text(range_label, "Range:");
            lv_obj_set_pos(range_label, 10, y_pos);
            lv_obj_set_style_text_color(range_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            y_pos += 20;
            
            create_button(content_area, "Min+", bar_min_plus_event, 10, y_pos, 80, 60);
            create_button(content_area, "Min-", bar_min_minus_event, 100, y_pos, 80, 60);
            create_button(content_area, "Max+", bar_max_plus_event, 190, y_pos, 80, 60);
            create_button(content_area, "Max-", bar_max_minus_event, 280, y_pos, 80, 60);
            y_pos += 70;
            
            // Value controls
            lv_obj_t* value_label = lv_label_create(content_area);
            lv_label_set_text(value_label, "Value:");
            lv_obj_set_pos(value_label, 10, y_pos);
            lv_obj_set_style_text_color(value_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            y_pos += 20;
            
            create_button(content_area, "Val+", bar_value_plus_event, 10, y_pos, 80, 60);
            create_button(content_area, "Val-", bar_value_minus_event, 100, y_pos, 80, 60);
            y_pos += 70;
            
            // Mode and orientation
            lv_obj_t* mode_label = lv_label_create(content_area);
            lv_label_set_text(mode_label, "Mode/Orient:");
            lv_obj_set_pos(mode_label, 10, y_pos);
            lv_obj_set_style_text_color(mode_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            y_pos += 20;
            
            if (obj.type == WidgetType::BAR) {
                create_button(content_area, "Mode", bar_toggle_mode_event, 10, y_pos, 80, 60);
            }
            create_button(content_area, "Flip", bar_toggle_orientation_event, obj.type == WidgetType::BAR ? 100 : 10, y_pos, 80, 60);
            break;
        }
        
        case WidgetType::BUTTON:
        case WidgetType::SWITCH:
        case WidgetType::CHECKBOX:
        case WidgetType::LED: {
            lv_obj_t* state_label = lv_label_create(content_area);
            lv_label_set_text(state_label, "State Properties:");
            lv_obj_set_pos(state_label, 10, y_pos);
            lv_obj_set_style_text_color(state_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            y_pos += 25;
            
            if (obj.type == WidgetType::BUTTON) {
                create_button(content_area, "Toggle", button_toggle_state_event, 10, y_pos, 60, 30);
            } else if (obj.type == WidgetType::SWITCH) {
                create_button(content_area, "Toggle", switch_toggle_state_event, 10, y_pos, 60, 30);
            } else if (obj.type == WidgetType::CHECKBOX) {
                create_button(content_area, "Toggle", checkbox_toggle_state_event, 10, y_pos, 60, 30);
            } else if (obj.type == WidgetType::LED) {
                create_button(content_area, "Toggle", led_toggle_state_event, 10, y_pos, 60, 30);
            }
            break;
        }
        
        default:
            // For other widget types, just show a placeholder
            lv_obj_t* placeholder = lv_label_create(content_area);
            lv_label_set_text(placeholder, "Basic controls only");
            lv_obj_set_pos(placeholder, 10, y_pos);
            lv_obj_set_style_text_color(placeholder, lv_color_hex(0xAAAAA), LV_PART_MAIN);
            break;
    }
}

void DebugAlignmentEnhanced::create_object_list() {
    lv_obj_clean(object_list_area);
    
    // Create "New" button
    lv_obj_t* new_btn = lv_btn_create(object_list_area);
    lv_obj_set_size(new_btn, 60, 25);
    lv_obj_set_pos(new_btn, 5, 5);
    lv_obj_add_event_cb(new_btn, back_to_menu_event, LV_EVENT_CLICKED, nullptr);
    lv_obj_set_style_bg_color(new_btn, lv_color_hex(0x00AA00), LV_PART_MAIN);
    
    lv_obj_t* new_label = lv_label_create(new_btn);
    lv_label_set_text(new_label, "New");
    lv_obj_center(new_label);
    
    update_object_list();
}

void DebugAlignmentEnhanced::update_object_list() {
    if (!object_list_area) return;
    
    printf("[DEBUG] Updating object list...\n");
    
    // Clear existing object buttons (but keep the "New" button at index 0)
    uint32_t child_count = lv_obj_get_child_count(object_list_area);
    for (uint32_t i = child_count; i > 1; i--) {
        lv_obj_t* child = lv_obj_get_child(object_list_area, i - 1);
        if (child) {
            lv_obj_delete(child);
        }
    }
    
    printf("[DEBUG] Cleared existing buttons, creating new ones...\n");
    
    // Create buttons for each spawned object
    int x_pos = 70;
    for (size_t i = 0; i < spawned_objects.size(); i++) {
        lv_obj_t* obj_btn = lv_btn_create(object_list_area);
        lv_obj_set_size(obj_btn, 80, 25);
        lv_obj_set_pos(obj_btn, x_pos, 5);
        
        // Highlight active object
        if ((int)i == active_object_index) {
            lv_obj_set_style_bg_color(obj_btn, lv_color_hex(0x0000AA), LV_PART_MAIN);
        } else {
            lv_obj_set_style_bg_color(obj_btn, lv_color_hex(0x444444), LV_PART_MAIN);
        }
        
        // Store index in user data for the event handler
        lv_obj_set_user_data(obj_btn, (void*)(uintptr_t)i);
        lv_obj_add_event_cb(obj_btn, [](lv_event_t* e) {
            lv_obj_t* target = (lv_obj_t*)lv_event_get_target(e);
            int index = (int)(uintptr_t)lv_obj_get_user_data(target);
            set_active_object(index);
        }, LV_EVENT_CLICKED, nullptr);
        
        lv_obj_t* obj_label = lv_label_create(obj_btn);
        lv_label_set_text(obj_label, spawned_objects[i].name.c_str());
        lv_obj_center(obj_label);
        
        x_pos += 85;
        if (x_pos > 300) break; // Don't overflow the area
    }
}

void DebugAlignmentEnhanced::spawn_widget(WidgetType type) {
    if (!parent_screen) return;
    
    lv_obj_t* widget = create_widget_instance(type, parent_screen);
    if (!widget) return;
    
    SpawnedObject obj;
    obj.type = type;
    obj.object = widget;
    obj.index = spawned_objects.size();
    obj.name = get_widget_type_name(type) + " #" + std::to_string(obj.index + 1);
    
    // Center the widget on screen
    lv_obj_center(widget);
    
    // Initialize transform values with actual widget position and size
    obj.transform.x_offset = lv_obj_get_x(widget);
    obj.transform.y_offset = lv_obj_get_y(widget);
    obj.transform.width = lv_obj_get_width(widget);
    obj.transform.height = lv_obj_get_height(widget);
    
    spawned_objects.push_back(obj);
    set_active_object(spawned_objects.size() - 1);
    
    printf("[DEBUG] Spawned %s\n", obj.name.c_str());
}

lv_obj_t* DebugAlignmentEnhanced::create_widget_instance(WidgetType type, lv_obj_t* parent) {
    switch (type) {
        case WidgetType::ARC: {
            lv_obj_t* arc = lv_arc_create(parent);
            lv_obj_set_size(arc, 150, 150);
            lv_arc_set_range(arc, 0, 100);
            lv_arc_set_value(arc, 50);
            return arc;
        }
        
        case WidgetType::BAR: {
            lv_obj_t* bar = lv_bar_create(parent);
            lv_obj_set_size(bar, 200, 20);
            lv_bar_set_range(bar, 0, 100);
            lv_bar_set_value(bar, 50, LV_ANIM_OFF);
            return bar;
        }
        
        case WidgetType::BUTTON: {
            lv_obj_t* btn = lv_btn_create(parent);
            lv_obj_set_size(btn, 100, 50);
            lv_obj_t* label = lv_label_create(btn);
            lv_label_set_text(label, "Button");
            lv_obj_center(label);
            return btn;
        }
        
        case WidgetType::CANVAS: {
            lv_obj_t* canvas = lv_canvas_create(parent);
            static lv_color_t canvas_buf[200 * 150];
            lv_canvas_set_buffer(canvas, canvas_buf, 200, 150, LV_COLOR_FORMAT_RGB565);
            lv_canvas_fill_bg(canvas, lv_color_hex(0x333333), LV_OPA_COVER);
            return canvas;
        }
        
        case WidgetType::CHART: {
            lv_obj_t* chart = lv_chart_create(parent);
            lv_obj_set_size(chart, 200, 150);
            lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
            lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
            return chart;
        }
        
        case WidgetType::CHECKBOX: {
            lv_obj_t* cb = lv_checkbox_create(parent);
            lv_checkbox_set_text(cb, "Checkbox");
            return cb;
        }
        
        case WidgetType::LED: {
            lv_obj_t* led = lv_led_create(parent);
            lv_obj_set_size(led, 50, 50);
            lv_led_set_color(led, lv_palette_main(LV_PALETTE_RED));
            return led;
        }
        
        case WidgetType::MESSAGE_BOX: {
            // For message box, create a simple container
            lv_obj_t* msgbox = lv_obj_create(parent);
            lv_obj_set_size(msgbox, 250, 150);
            lv_obj_set_style_bg_color(msgbox, lv_color_hex(0x444444), LV_PART_MAIN);
            lv_obj_t* label = lv_label_create(msgbox);
            lv_label_set_text(label, "Message Box");
            lv_obj_center(label);
            return msgbox;
        }
        
        case WidgetType::SLIDER: {
            lv_obj_t* slider = lv_slider_create(parent);
            lv_obj_set_size(slider, 200, 20);
            lv_slider_set_range(slider, 0, 100);
            lv_slider_set_value(slider, 50, LV_ANIM_OFF);
            return slider;
        }
        
        case WidgetType::SPIN_BOX: {
            lv_obj_t* spinbox = lv_spinbox_create(parent);
            lv_obj_set_size(spinbox, 120, 40);
            lv_spinbox_set_range(spinbox, 0, 100);
            lv_spinbox_set_value(spinbox, 50);
            return spinbox;
        }
        
        case WidgetType::SPINNER: {
            lv_obj_t* spinner = lv_spinner_create(parent);
            lv_obj_set_size(spinner, 80, 80);
            return spinner;
        }
        
        case WidgetType::SWITCH: {
            lv_obj_t* sw = lv_switch_create(parent);
            return sw;
        }
        
        case WidgetType::TABLE: {
            lv_obj_t* table = lv_table_create(parent);
            lv_obj_set_size(table, 200, 150);
            lv_table_set_cell_value(table, 0, 0, "A");
            lv_table_set_cell_value(table, 0, 1, "B");
            lv_table_set_cell_value(table, 1, 0, "C");
            lv_table_set_cell_value(table, 1, 1, "D");
            return table;
        }
        
        case WidgetType::PIE_CHART: {
            // Create a simple arc to simulate pie chart
            lv_obj_t* pie = lv_arc_create(parent);
            lv_obj_set_size(pie, 150, 150);
            lv_arc_set_range(pie, 0, 360);
            lv_arc_set_value(pie, 90);
            lv_arc_set_bg_angles(pie, 0, 360);
            return pie;
        }
        
        default:
            return nullptr;
    }
}

std::string DebugAlignmentEnhanced::get_widget_type_name(WidgetType type) {
    switch (type) {
        case WidgetType::ARC: return "Arc";
        case WidgetType::BAR: return "Bar";
        case WidgetType::BUTTON: return "Button";
        case WidgetType::CANVAS: return "Canvas";
        case WidgetType::CHART: return "Chart";
        case WidgetType::CHECKBOX: return "Checkbox";
        case WidgetType::LED: return "LED";
        case WidgetType::MESSAGE_BOX: return "MsgBox";
        case WidgetType::SLIDER: return "Slider";
        case WidgetType::SPIN_BOX: return "SpinBox";
        case WidgetType::SPINNER: return "Spinner";
        case WidgetType::SWITCH: return "Switch";
        case WidgetType::TABLE: return "Table";
        case WidgetType::PIE_CHART: return "PieChart";
        default: return "Unknown";
    }
}

void DebugAlignmentEnhanced::set_active_object(int index) {
    if (index < 0 || index >= (int)spawned_objects.size()) return;
    
    active_object_index = index;
    current_state = UIState::OBJECT_CONTROLS;
    
    printf("[DEBUG] Setting active object to index %d: %s\n", index, spawned_objects[index].name.c_str());
    
    create_object_controls();
    update_object_list();
    
    printf("[DEBUG] Active object set successfully\n");
}

void DebugAlignmentEnhanced::delete_active_object() {
    if (active_object_index < 0 || active_object_index >= (int)spawned_objects.size()) return;
    
    SpawnedObject& obj = spawned_objects[active_object_index];
    printf("[DEBUG] Deleting object: %s\n", obj.name.c_str());
    
    if (obj.object) {
        lv_obj_delete(obj.object);
    }
    
    spawned_objects.erase(spawned_objects.begin() + active_object_index);
    
    // Update indices
    for (size_t i = 0; i < spawned_objects.size(); i++) {
        spawned_objects[i].index = i;
        spawned_objects[i].name = get_widget_type_name(spawned_objects[i].type) + " #" + std::to_string(i + 1);
    }
    
    // Set new active object or go back to menu
    if (spawned_objects.empty()) {
        active_object_index = -1;
        current_state = UIState::SPAWN_MENU;
        create_spawn_menu();
    } else {
        if (active_object_index >= (int)spawned_objects.size()) {
            active_object_index = spawned_objects.size() - 1;
        }
        create_object_controls();
    }
    
    update_object_list();
}

void DebugAlignmentEnhanced::apply_transform_to_object(SpawnedObject& obj) {
    if (!obj.object) return;
    
    // Apply position - use the transform values directly, not relative to current position
    lv_obj_set_pos(obj.object, obj.transform.x_offset, obj.transform.y_offset);
    
    if (obj.transform.rotation != 0) {
        // Apply rotation if supported by the widget
        if (obj.type == WidgetType::ARC) {
            lv_arc_set_rotation(obj.object, obj.transform.rotation);
        }
    }
    
    lv_obj_set_size(obj.object, obj.transform.width, obj.transform.height);
    lv_obj_set_style_pad_all(obj.object, obj.transform.pad, LV_PART_MAIN);
}

void DebugAlignmentEnhanced::apply_properties_to_object(SpawnedObject& obj) {
    if (!obj.object) return;
    
    switch (obj.type) {
        case WidgetType::ARC:
            lv_arc_set_bg_angles(obj.object, obj.props.start_angle, obj.props.end_angle);
            lv_obj_set_style_arc_width(obj.object, obj.props.arc_width, LV_PART_INDICATOR);
            lv_arc_set_value(obj.object, obj.props.value);
            lv_arc_set_range(obj.object, obj.props.range_min, obj.props.range_max);
            break;
            
        case WidgetType::BAR:
            lv_bar_set_range(obj.object, obj.props.min_value, obj.props.max_value);
            lv_bar_set_value(obj.object, obj.props.current_value, LV_ANIM_OFF);
            break;
            
        case WidgetType::SLIDER:
            lv_slider_set_range(obj.object, obj.props.min_value, obj.props.max_value);
            lv_slider_set_value(obj.object, obj.props.current_value, LV_ANIM_OFF);
            break;
            
        case WidgetType::BUTTON:
            // Button state is handled via events
            break;
            
        case WidgetType::SWITCH:
            if (obj.props.state) {
                lv_obj_add_state(obj.object, LV_STATE_CHECKED);
            } else {
                lv_obj_clear_state(obj.object, LV_STATE_CHECKED);
            }
            break;
            
        case WidgetType::CHECKBOX:
            if (obj.props.state) {
                lv_obj_add_state(obj.object, LV_STATE_CHECKED);
            } else {
                lv_obj_clear_state(obj.object, LV_STATE_CHECKED);
            }
            break;
            
        case WidgetType::LED:
            if (obj.props.state) {
                lv_led_on(obj.object);
            } else {
                lv_led_off(obj.object);
            }
            break;
            
        default:
            break;
    }
}

DebugAlignmentEnhanced::SpawnedObject* DebugAlignmentEnhanced::get_active_object() {
    if (active_object_index < 0 || active_object_index >= (int)spawned_objects.size()) {
        return nullptr;
    }
    return &spawned_objects[active_object_index];
}

lv_obj_t* DebugAlignmentEnhanced::create_button(lv_obj_t* parent, const char* text, lv_event_cb_t callback, int x, int y, int w, int h) {
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_size(btn, w, h);
    lv_obj_set_pos(btn, x, y);
    lv_obj_add_event_cb(btn, callback, LV_EVENT_CLICKED, nullptr);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_PART_MAIN);
    
    lv_obj_t* label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN);
    
    return btn;
}

void DebugAlignmentEnhanced::clear_content_area() {
    if (content_area) {
        lv_obj_clean(content_area);
    }
}

void DebugAlignmentEnhanced::export_object_values(const SpawnedObject& obj) {
    char export_text[1024];
    int n = snprintf(export_text, sizeof(export_text),
        "\n[DEBUG EXPORT] ===== %s VALUES =====\n"
        "// Transform\n"
        "#define %s_X_OFFSET     %d\n"
        "#define %s_Y_OFFSET     %d\n"
        "#define %s_WIDTH        %d\n"
        "#define %s_HEIGHT       %d\n"
        "#define %s_ROTATION     %d\n"
        "#define %s_PAD          %d\n",
        obj.name.c_str(),
        obj.name.c_str(), obj.transform.x_offset,
        obj.name.c_str(), obj.transform.y_offset,
        obj.name.c_str(), obj.transform.width,
        obj.name.c_str(), obj.transform.height,
        obj.name.c_str(), obj.transform.rotation,
        obj.name.c_str(), obj.transform.pad);
    
    // Add widget-specific properties
    switch (obj.type) {
        case WidgetType::ARC:
            n += snprintf(export_text + n, sizeof(export_text) - n,
                "// Arc Properties\n"
                "#define %s_START_ANGLE  %d\n"
                "#define %s_END_ANGLE    %d\n"
                "#define %s_ARC_WIDTH    %d\n"
                "#define %s_VALUE        %d\n"
                "#define %s_RANGE_MIN    %d\n"
                "#define %s_RANGE_MAX    %d\n",
                obj.name.c_str(), obj.props.start_angle,
                obj.name.c_str(), obj.props.end_angle,
                obj.name.c_str(), obj.props.arc_width,
                obj.name.c_str(), obj.props.value,
                obj.name.c_str(), obj.props.range_min,
                obj.name.c_str(), obj.props.range_max);
            break;
            
        case WidgetType::BAR:
        case WidgetType::SLIDER:
            n += snprintf(export_text + n, sizeof(export_text) - n,
                "// Bar/Slider Properties\n"
                "#define %s_MIN_VALUE    %d\n"
                "#define %s_MAX_VALUE    %d\n"
                "#define %s_CURRENT_VAL  %d\n"
                "#define %s_HORIZONTAL   %s\n",
                obj.name.c_str(), obj.props.min_value,
                obj.name.c_str(), obj.props.max_value,
                obj.name.c_str(), obj.props.current_value,
                obj.name.c_str(), obj.props.horizontal ? "true" : "false");
            break;
            
        default:
            break;
    }
    
    n += snprintf(export_text + n, sizeof(export_text) - n, "==============================\n\n");
    
    if (n > 0) {
        fwrite(export_text, 1, (size_t)(n < (int)sizeof(export_text) ? n : (int)sizeof(export_text)), stdout);
    }
}

// Event handler implementations
void DebugAlignmentEnhanced::toggle_panel_event(lv_event_t*) {
    toggle();
}

void DebugAlignmentEnhanced::back_to_menu_event(lv_event_t*) {
    current_state = UIState::SPAWN_MENU;
    create_spawn_menu();
    update_object_list();
}

// Widget spawn event handlers
void DebugAlignmentEnhanced::spawn_arc_event(lv_event_t*) { spawn_widget(WidgetType::ARC); }
void DebugAlignmentEnhanced::spawn_bar_event(lv_event_t*) { spawn_widget(WidgetType::BAR); }
void DebugAlignmentEnhanced::spawn_button_event(lv_event_t*) { spawn_widget(WidgetType::BUTTON); }
void DebugAlignmentEnhanced::spawn_canvas_event(lv_event_t*) { spawn_widget(WidgetType::CANVAS); }
void DebugAlignmentEnhanced::spawn_chart_event(lv_event_t*) { spawn_widget(WidgetType::CHART); }
void DebugAlignmentEnhanced::spawn_checkbox_event(lv_event_t*) { spawn_widget(WidgetType::CHECKBOX); }
void DebugAlignmentEnhanced::spawn_led_event(lv_event_t*) { spawn_widget(WidgetType::LED); }
void DebugAlignmentEnhanced::spawn_message_box_event(lv_event_t*) { spawn_widget(WidgetType::MESSAGE_BOX); }
void DebugAlignmentEnhanced::spawn_slider_event(lv_event_t*) { spawn_widget(WidgetType::SLIDER); }
void DebugAlignmentEnhanced::spawn_spin_box_event(lv_event_t*) { spawn_widget(WidgetType::SPIN_BOX); }
void DebugAlignmentEnhanced::spawn_spinner_event(lv_event_t*) { spawn_widget(WidgetType::SPINNER); }
void DebugAlignmentEnhanced::spawn_switch_event(lv_event_t*) { spawn_widget(WidgetType::SWITCH); }
void DebugAlignmentEnhanced::spawn_table_event(lv_event_t*) { spawn_widget(WidgetType::TABLE); }
void DebugAlignmentEnhanced::spawn_pie_chart_event(lv_event_t*) { spawn_widget(WidgetType::PIE_CHART); }

// Transform control event handlers
void DebugAlignmentEnhanced::move_left_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    lv_obj_set_x(obj->object, lv_obj_get_x(obj->object) - 1);
    obj->transform.x_offset = lv_obj_get_x(obj->object);
}

void DebugAlignmentEnhanced::move_right_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    lv_obj_set_x(obj->object, lv_obj_get_x(obj->object) + 1);
    obj->transform.x_offset = lv_obj_get_x(obj->object);
}

void DebugAlignmentEnhanced::move_up_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    lv_obj_set_y(obj->object, lv_obj_get_y(obj->object) - 1);
    obj->transform.y_offset = lv_obj_get_y(obj->object);
}

void DebugAlignmentEnhanced::move_down_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    lv_obj_set_y(obj->object, lv_obj_get_y(obj->object) + 1);
    obj->transform.y_offset = lv_obj_get_y(obj->object);
}

void DebugAlignmentEnhanced::move_left_fast_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    lv_obj_set_x(obj->object, lv_obj_get_x(obj->object) - 15);
    obj->transform.x_offset = lv_obj_get_x(obj->object);
}

void DebugAlignmentEnhanced::move_right_fast_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    lv_obj_set_x(obj->object, lv_obj_get_x(obj->object) + 15);
    obj->transform.x_offset = lv_obj_get_x(obj->object);
}

void DebugAlignmentEnhanced::move_up_fast_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    lv_obj_set_y(obj->object, lv_obj_get_y(obj->object) - 15);
    obj->transform.y_offset = lv_obj_get_y(obj->object);
}

void DebugAlignmentEnhanced::move_down_fast_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    lv_obj_set_y(obj->object, lv_obj_get_y(obj->object) + 15);
    obj->transform.y_offset = lv_obj_get_y(obj->object);
}

void DebugAlignmentEnhanced::rotate_cw_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    obj->transform.rotation += 5;
    apply_transform_to_object(*obj);
}

void DebugAlignmentEnhanced::rotate_ccw_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    obj->transform.rotation -= 5;
    apply_transform_to_object(*obj);
}

void DebugAlignmentEnhanced::resize_wider_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    
    if (obj->type == WidgetType::ARC) {
        // For Arc widgets, increase both width and height together (size)
        obj->transform.width += 10;
        obj->transform.height += 10;
        lv_obj_set_size(obj->object, obj->transform.width, obj->transform.height);
    } else {
        // For other widgets, only increase width
        obj->transform.width += 10;
        lv_obj_set_width(obj->object, obj->transform.width);
    }
}

void DebugAlignmentEnhanced::resize_narrower_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    
    if (obj->type == WidgetType::ARC) {
        // For Arc widgets, decrease both width and height together (size)
        obj->transform.width = std::max(10, obj->transform.width - 10);
        obj->transform.height = std::max(10, obj->transform.height - 10);
        lv_obj_set_size(obj->object, obj->transform.width, obj->transform.height);
    } else {
        // For other widgets, only decrease width
        obj->transform.width = std::max(10, obj->transform.width - 10);
        lv_obj_set_width(obj->object, obj->transform.width);
    }
}

void DebugAlignmentEnhanced::resize_taller_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    obj->transform.height += 10;
    lv_obj_set_height(obj->object, obj->transform.height);
}

void DebugAlignmentEnhanced::resize_shorter_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    obj->transform.height = std::max(10, obj->transform.height - 10);
    lv_obj_set_height(obj->object, obj->transform.height);
}

void DebugAlignmentEnhanced::bring_to_front_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    lv_obj_move_foreground(obj->object);
}

void DebugAlignmentEnhanced::send_to_back_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    lv_obj_move_background(obj->object);
}

void DebugAlignmentEnhanced::delete_active_event(lv_event_t*) {
    delete_active_object();
}

void DebugAlignmentEnhanced::export_values_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    export_object_values(*obj);
}

// Global style and visibility controls
void DebugAlignmentEnhanced::toggle_visibility_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    
    if (lv_obj_has_flag(obj->object, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_clear_flag(obj->object, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(obj->object, LV_OBJ_FLAG_HIDDEN);
    }
}

void DebugAlignmentEnhanced::border_width_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || !obj->object) return;
    
    lv_coord_t current_border = lv_obj_get_style_border_width(obj->object, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj->object, current_border + 1, LV_PART_MAIN);
}

void DebugAlignmentEnhanced::border_width_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || !obj->object) return;
    
    lv_coord_t current_border = lv_obj_get_style_border_width(obj->object, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj->object, std::max(0, (int)current_border - 1), LV_PART_MAIN);
}

void DebugAlignmentEnhanced::radius_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || !obj->object) return;
    
    lv_coord_t current_radius = lv_obj_get_style_radius(obj->object, LV_PART_MAIN);
    lv_obj_set_style_radius(obj->object, current_radius + 2, LV_PART_MAIN);
}

void DebugAlignmentEnhanced::radius_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || !obj->object) return;
    
    lv_coord_t current_radius = lv_obj_get_style_radius(obj->object, LV_PART_MAIN);
    lv_obj_set_style_radius(obj->object, std::max(0, (int)current_radius - 2), LV_PART_MAIN);
}

void DebugAlignmentEnhanced::opacity_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || !obj->object) return;
    
    lv_opa_t current_opa = lv_obj_get_style_opa(obj->object, LV_PART_MAIN);
    lv_obj_set_style_opa(obj->object, std::min(255, (int)current_opa + 25), LV_PART_MAIN);
}

void DebugAlignmentEnhanced::opacity_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || !obj->object) return;
    
    lv_opa_t current_opa = lv_obj_get_style_opa(obj->object, LV_PART_MAIN);
    lv_obj_set_style_opa(obj->object, std::max(25, (int)current_opa - 25), LV_PART_MAIN);
}

// Arc-specific property controls
void DebugAlignmentEnhanced::arc_start_angle_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.start_angle += 5;
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::arc_start_angle_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.start_angle -= 5;
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::arc_end_angle_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.end_angle += 5;
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::arc_end_angle_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.end_angle -= 5;
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::arc_width_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.arc_width += 2;
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::arc_width_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.arc_width = std::max(1, obj->props.arc_width - 2);
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::arc_toggle_caps_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.rounded_caps = !obj->props.rounded_caps;
    if (obj->props.rounded_caps) {
        lv_obj_set_style_arc_rounded(obj->object, true, LV_PART_INDICATOR);
    } else {
        lv_obj_set_style_arc_rounded(obj->object, false, LV_PART_INDICATOR);
    }
}

void DebugAlignmentEnhanced::arc_value_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.value = std::min(obj->props.range_max, obj->props.value + 5);
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::arc_value_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.value = std::max(obj->props.range_min, obj->props.value - 5);
    apply_properties_to_object(*obj);
}

// Bar/Slider property controls
void DebugAlignmentEnhanced::bar_min_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || (obj->type != WidgetType::BAR && obj->type != WidgetType::SLIDER)) return;
    obj->props.min_value += 5;
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::bar_min_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || (obj->type != WidgetType::BAR && obj->type != WidgetType::SLIDER)) return;
    obj->props.min_value -= 5;
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::bar_max_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || (obj->type != WidgetType::BAR && obj->type != WidgetType::SLIDER)) return;
    obj->props.max_value += 5;
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::bar_max_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || (obj->type != WidgetType::BAR && obj->type != WidgetType::SLIDER)) return;
    obj->props.max_value -= 5;
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::bar_value_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || (obj->type != WidgetType::BAR && obj->type != WidgetType::SLIDER)) return;
    obj->props.current_value = std::min(obj->props.max_value, obj->props.current_value + 5);
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::bar_value_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || (obj->type != WidgetType::BAR && obj->type != WidgetType::SLIDER)) return;
    obj->props.current_value = std::max(obj->props.min_value, obj->props.current_value - 5);
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::bar_toggle_orientation_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || (obj->type != WidgetType::BAR && obj->type != WidgetType::SLIDER)) return;
    
    obj->props.horizontal = !obj->props.horizontal;
    
    // Swap width and height for orientation change
    int temp = obj->transform.width;
    obj->transform.width = obj->transform.height;
    obj->transform.height = temp;
    
    lv_obj_set_size(obj->object, obj->transform.width, obj->transform.height);
}

// State toggle controls
void DebugAlignmentEnhanced::button_toggle_state_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::BUTTON) return;
    obj->props.state = !obj->props.state;
    if (obj->props.state) {
        lv_obj_add_state(obj->object, LV_STATE_PRESSED);
    } else {
        lv_obj_clear_state(obj->object, LV_STATE_PRESSED);
    }
}

void DebugAlignmentEnhanced::switch_toggle_state_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::SWITCH) return;
    obj->props.state = !obj->props.state;
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::checkbox_toggle_state_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::CHECKBOX) return;
    obj->props.state = !obj->props.state;
    apply_properties_to_object(*obj);
}

void DebugAlignmentEnhanced::led_toggle_state_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::LED) return;
    obj->props.state = !obj->props.state;
    apply_properties_to_object(*obj);
}

// Additional Arc controls
void DebugAlignmentEnhanced::arc_range_min_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.range_min += 5;
    if (obj->props.range_min >= obj->props.range_max) {
        obj->props.range_min = obj->props.range_max - 1;
    }
    lv_arc_set_range(obj->object, obj->props.range_min, obj->props.range_max);
}

void DebugAlignmentEnhanced::arc_range_min_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.range_min -= 5;
    lv_arc_set_range(obj->object, obj->props.range_min, obj->props.range_max);
}

void DebugAlignmentEnhanced::arc_range_max_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.range_max += 5;
    lv_arc_set_range(obj->object, obj->props.range_min, obj->props.range_max);
}

void DebugAlignmentEnhanced::arc_range_max_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.range_max -= 5;
    if (obj->props.range_max <= obj->props.range_min) {
        obj->props.range_max = obj->props.range_min + 1;
    }
    lv_arc_set_range(obj->object, obj->props.range_min, obj->props.range_max);
}

void DebugAlignmentEnhanced::arc_rotation_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.rotation += 5;
    lv_arc_set_rotation(obj->object, obj->props.rotation);
}

void DebugAlignmentEnhanced::arc_rotation_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.rotation -= 5;
    lv_arc_set_rotation(obj->object, obj->props.rotation);
}

void DebugAlignmentEnhanced::arc_track_width_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.track_width += 2;
    lv_obj_set_style_arc_width(obj->object, obj->props.track_width, LV_PART_MAIN);
}

void DebugAlignmentEnhanced::arc_track_width_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.track_width = std::max(1, obj->props.track_width - 2);
    lv_obj_set_style_arc_width(obj->object, obj->props.track_width, LV_PART_MAIN);
}

void DebugAlignmentEnhanced::arc_toggle_track_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.track_visible = !obj->props.track_visible;
    if (obj->props.track_visible) {
        lv_obj_set_style_arc_opa(obj->object, LV_OPA_COVER, LV_PART_MAIN);
    } else {
        lv_obj_set_style_arc_opa(obj->object, LV_OPA_TRANSP, LV_PART_MAIN);
    }
}

void DebugAlignmentEnhanced::arc_pad_plus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.pad += 2;
    lv_obj_set_style_pad_all(obj->object, obj->props.pad, LV_PART_MAIN);
}

void DebugAlignmentEnhanced::arc_pad_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->props.pad = std::max(0, obj->props.pad - 2);
    lv_obj_set_style_pad_all(obj->object, obj->props.pad, LV_PART_MAIN);
}

// Additional Bar controls
void DebugAlignmentEnhanced::bar_toggle_mode_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::BAR) return;
    obj->props.bar_mode = (obj->props.bar_mode == 0) ? 1 : 0;
    lv_bar_set_mode(obj->object, obj->props.bar_mode == 0 ? LV_BAR_MODE_NORMAL : LV_BAR_MODE_RANGE);
}
