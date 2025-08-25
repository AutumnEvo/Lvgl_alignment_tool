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
bool DebugAlignmentEnhanced::grid_snap_enabled = false;
int DebugAlignmentEnhanced::grid_size = 10;
lv_obj_t* DebugAlignmentEnhanced::widget_controls_parent = nullptr;
lv_obj_t* DebugAlignmentEnhanced::grid_label_ptr = nullptr;

void DebugAlignmentEnhanced::init(lv_obj_t* screen) {
#if ENABLE_DEBUG_ALIGNMENT
    cleanup(); // Clean up any existing instances
    parent_screen = screen;
    
    // Create modern toggle button with sleek design
    toggle_button = lv_btn_create(screen);
    lv_obj_set_size(toggle_button, 120, 60);
    lv_obj_align(toggle_button, LV_ALIGN_TOP_RIGHT, -15, 15);
    lv_obj_add_event_cb(toggle_button, toggle_panel_event, LV_EVENT_CLICKED, nullptr);
    
    // Modern dark theme with blue accent
    lv_obj_set_style_bg_color(toggle_button, lv_color_hex(0x2C3E50), LV_PART_MAIN);  // Dark blue-gray
    lv_obj_set_style_bg_grad_color(toggle_button, lv_color_hex(0x3498DB), LV_PART_MAIN);  // Bright blue
    lv_obj_set_style_bg_grad_dir(toggle_button, LV_GRAD_DIR_VER, LV_PART_MAIN);
    lv_obj_set_style_radius(toggle_button, 20, LV_PART_MAIN);
    lv_obj_set_style_border_color(toggle_button, lv_color_hex(0x3498DB), LV_PART_MAIN);
    lv_obj_set_style_border_width(toggle_button, 2, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(toggle_button, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(toggle_button, 8, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(toggle_button, LV_OPA_40, LV_PART_MAIN);
    
    lv_obj_t* toggle_label = lv_label_create(toggle_button);
    lv_label_set_text(toggle_label, " DEBUG");
    lv_obj_center(toggle_label);
    lv_obj_set_style_text_color(toggle_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(toggle_label, &lv_font_montserrat_20, LV_PART_MAIN);
    
    // Bring the button to the front to ensure it's visible
    lv_obj_move_foreground(toggle_button);
    
    // Create sleek main debug panel with modern glassmorphism design
    debug_panel = lv_obj_create(screen);
    lv_obj_set_size(debug_panel, 900, 750);  // Wider and better proportioned
    lv_obj_align(debug_panel, LV_ALIGN_CENTER, 0, 0);  // Center it for better UX
    
    // Glassmorphism effect - semi-transparent with blur-like appearance
    lv_obj_set_style_bg_color(debug_panel, lv_color_hex(0x1E1E1E), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(debug_panel, LV_OPA_40, LV_PART_MAIN);  // Semi-transparent
    lv_obj_set_style_border_color(debug_panel, lv_color_hex(0x3498DB), LV_PART_MAIN);
    lv_obj_set_style_border_width(debug_panel, 3, LV_PART_MAIN);
    lv_obj_set_style_radius(debug_panel, 25, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(debug_panel, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(debug_panel, 15, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(debug_panel, LV_OPA_60, LV_PART_MAIN);
    lv_obj_set_style_pad_all(debug_panel, 20, LV_PART_MAIN);
    lv_obj_add_flag(debug_panel, LV_OBJ_FLAG_HIDDEN);
    
    // Create modern header area with title and object list
    object_list_area = lv_obj_create(debug_panel);
    lv_obj_set_size(object_list_area, LV_PCT(100), 90);
    lv_obj_align(object_list_area, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(object_list_area, lv_color_hex(0x2C3E50), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(object_list_area, LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_radius(object_list_area, 15, LV_PART_MAIN);
    lv_obj_set_style_border_color(object_list_area, lv_color_hex(0x3498DB), LV_PART_MAIN);
    lv_obj_set_style_border_width(object_list_area, 2, LV_PART_MAIN);
    lv_obj_set_style_pad_all(object_list_area, 15, LV_PART_MAIN);
    
    // Create main content area with modern card design
    content_area = lv_obj_create(debug_panel);
    lv_obj_set_size(content_area, LV_PCT(100), 630);
    lv_obj_align(content_area, LV_ALIGN_TOP_MID, 0, 100);
    
    // Modern card styling
    lv_obj_set_style_bg_color(content_area, lv_color_hex(0x34495E), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(content_area, LV_OPA_40, LV_PART_MAIN);
    lv_obj_set_style_radius(content_area, 15, LV_PART_MAIN);
    lv_obj_set_style_border_color(content_area, lv_color_hex(0x3498DB), LV_PART_MAIN);
    lv_obj_set_style_border_width(content_area, 2, LV_PART_MAIN);
    lv_obj_set_style_pad_all(content_area, 20, LV_PART_MAIN);
    
    // Enable smooth scrolling
    lv_obj_set_scroll_dir(content_area, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(content_area, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_style_pad_right(content_area, 20, LV_PART_MAIN);
    
    // Initialize with spawn menu
    current_state = UIState::SPAWN_MENU;
    create_object_list();
    create_spawn_menu();
    
    printf("\n🌸 [ENHANCED DEBUG] Cherry Blossom Debug Alignment initialized! Look for the pink button in top-right! 🌸\n");
    printf("[DEBUG TOOLS] Interface size: 900x750, Modern glassmorphism design\n\n");
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
    
    // Modern title with better typography
    lv_obj_t* title = lv_label_create(content_area);
    lv_label_set_text(title, "Widget Toolkit");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_color(title, lv_color_hex(0xECF0F1), LV_PART_MAIN);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_32, LV_PART_MAIN);
    
    // Subtitle
    lv_obj_t* subtitle = lv_label_create(content_area);
    lv_label_set_text(subtitle, "Select a widget to create and customize");
    lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0xBDC3C7), LV_PART_MAIN);
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_20, LV_PART_MAIN);
    
    // Create a grid container for widget cards
    lv_obj_t* grid_container = lv_obj_create(content_area);
    lv_obj_set_size(grid_container, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_align(grid_container, LV_ALIGN_TOP_MID, 0, 90);
    lv_obj_set_style_bg_opa(grid_container, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(grid_container, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(grid_container, 10, LV_PART_MAIN);
    
    // Set up flex layout for responsive grid
    lv_obj_set_flex_flow(grid_container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(grid_container, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_column(grid_container, 15, LV_PART_MAIN);
    
    // Widget definitions with modern design
    struct {
        const char* name;
        const char* icon;
        const char* description;
        lv_event_cb_t callback;
        uint32_t color;
    } widgets[] = {
        {"Arc", "ARC", "Progress arcs", spawn_arc_event, 0xE74C3C},
        {"Bar", "BAR", "Progress bars", spawn_bar_event, 0x9B59B6},
        {"Button", "BTN", "Interactive buttons", spawn_button_event, 0x3498DB},
        {"Canvas", "CVS", "Drawing canvas", spawn_canvas_event, 0xF39C12},
        {"Chart", "CHT", "Data charts", spawn_chart_event, 0x27AE60},
        {"Checkbox", "CHK", "Check boxes", spawn_checkbox_event, 0x2ECC71},
        {"LED", "LED", "LED indicators", spawn_led_event, 0xE67E22},
        {"MsgBox", "MSG", "Message dialogs", spawn_message_box_event, 0x8E44AD},
        {"Slider", "SLD", "Value sliders", spawn_slider_event, 0x16A085},
        {"SpinBox", "NUM", "Number input", spawn_spin_box_event, 0xD35400},
        {"Spinner", "SPN", "Loading spinners", spawn_spinner_event, 0x2980B9},
        {"Switch", "SWT", "Toggle switches", spawn_switch_event, 0x27AE60},
        {"Table", "TBL", "Data tables", spawn_table_event, 0x34495E},
        {"PieChart", "PIE", "Pie charts", spawn_pie_chart_event, 0xC0392B}
    };
    
    // Create modern widget cards
    for (int i = 0; i < 14; i++) {
        // Create card container
        lv_obj_t* card = lv_obj_create(grid_container);
        lv_obj_set_size(card, 180, 120);  // Compact card size
        
        // Modern card styling with gradient
        lv_obj_set_style_bg_color(card, lv_color_hex(widgets[i].color), LV_PART_MAIN);
        lv_obj_set_style_bg_grad_color(card, lv_color_hex(widgets[i].color & 0x7F7F7F), LV_PART_MAIN);
        lv_obj_set_style_bg_grad_dir(card, LV_GRAD_DIR_VER, LV_PART_MAIN);
        lv_obj_set_style_radius(card, 15, LV_PART_MAIN);
        lv_obj_set_style_border_color(card, lv_color_hex(0xECF0F1), LV_PART_MAIN);
        lv_obj_set_style_border_width(card, 2, LV_PART_MAIN);
        lv_obj_set_style_border_opa(card, LV_OPA_30, LV_PART_MAIN);
        lv_obj_set_style_shadow_color(card, lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_set_style_shadow_width(card, 10, LV_PART_MAIN);
        lv_obj_set_style_shadow_opa(card, LV_OPA_30, LV_PART_MAIN);
        lv_obj_set_style_pad_all(card, 15, LV_PART_MAIN);
        
        // Hover effect
        lv_obj_set_style_transform_zoom(card, 105, LV_STATE_PRESSED);
        lv_obj_set_style_shadow_width(card, 15, LV_STATE_PRESSED);
        
        // Add click event
        lv_obj_add_event_cb(card, widgets[i].callback, LV_EVENT_CLICKED, nullptr);
        lv_obj_add_flag(card, LV_OBJ_FLAG_CLICKABLE);
        
        // Name (centered since we removed the icon)
        lv_obj_t* name = lv_label_create(card);
        lv_label_set_text(name, widgets[i].name);
        lv_obj_align(name, LV_ALIGN_CENTER, 0, -10);
        lv_obj_set_style_text_font(name, &lv_font_montserrat_20, LV_PART_MAIN);
        lv_obj_set_style_text_color(name, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        
        // Description
        lv_obj_t* desc = lv_label_create(card);
        lv_label_set_text(desc, widgets[i].description);
        lv_obj_align(desc, LV_ALIGN_BOTTOM_MID, 0, -5);
        lv_obj_set_style_text_font(desc, &lv_font_montserrat_14, LV_PART_MAIN);
        lv_obj_set_style_text_color(desc, lv_color_hex(0xECF0F1), LV_PART_MAIN);
        lv_obj_set_style_text_align(desc, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
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
    
    // Modern header with object info
    lv_obj_t* header = lv_obj_create(content_area);
    lv_obj_set_size(header, LV_PCT(100), 80);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x2C3E50), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(header, LV_OPA_90, LV_PART_MAIN);
    lv_obj_set_style_radius(header, 12, LV_PART_MAIN);
    lv_obj_set_style_border_color(header, lv_color_hex(0x3498DB), LV_PART_MAIN);
    lv_obj_set_style_border_width(header, 2, LV_PART_MAIN);
    lv_obj_set_style_pad_all(header, 15, LV_PART_MAIN);
    
    // Object title
    lv_obj_t* title = lv_label_create(header);
    char title_text[64];
    snprintf(title_text, sizeof(title_text), " %s", obj.name.c_str());
    lv_label_set_text(title, title_text);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xECF0F1), LV_PART_MAIN);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, LV_PART_MAIN);
    
    // Action buttons in header
    lv_obj_t* btn_container = lv_obj_create(header);
    lv_obj_set_size(btn_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(btn_container, LV_ALIGN_TOP_RIGHT, 0, -5);
    lv_obj_set_style_bg_opa(btn_container, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(btn_container, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(btn_container, 5, LV_PART_MAIN);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(btn_container, 10, LV_PART_MAIN);
    
    // Create modern action buttons
    create_modern_button(btn_container, " Back", back_to_menu_event, 0x95A5A6, 100, 40);
    create_modern_button(btn_container, " Delete", delete_active_event, 0xE74C3C, 100, 40);
    create_modern_button(btn_container, " Export", export_values_event, 0x27AE60, 100, 40);
    
    // Create tabbed interface for different control categories (simplified for older LVGL)
    lv_obj_t* tab_view = lv_tabview_create(content_area);
    lv_obj_set_size(tab_view, LV_PCT(100), 520);
    lv_obj_align(tab_view, LV_ALIGN_TOP_MID, 0, 90);
    lv_obj_set_style_bg_color(tab_view, lv_color_hex(0x34495E), LV_PART_MAIN);
    lv_obj_set_style_radius(tab_view, 12, LV_PART_MAIN);
    
    // Style the tab buttons
    lv_obj_t* tab_btns = lv_tabview_get_tab_btns(tab_view);
    lv_obj_set_style_bg_color(tab_btns, lv_color_hex(0x2C3E50), LV_PART_MAIN);
    lv_obj_set_style_text_color(tab_btns, lv_color_hex(0xECF0F1), LV_PART_MAIN);
    lv_obj_set_style_text_font(tab_btns, &lv_font_montserrat_16, LV_PART_MAIN);
    
    // Tab 1: Transform Controls
    lv_obj_t* tab_transform = lv_tabview_add_tab(tab_view, "Transform");
    create_transform_controls(tab_transform);
    
    // Tab 2: Style Controls
    lv_obj_t* tab_style = lv_tabview_add_tab(tab_view, "Style");
    create_style_controls(tab_style);
    
    // Tab 3: Widget-specific Controls
    lv_obj_t* tab_widget = lv_tabview_add_tab(tab_view, "Properties");
    create_widget_specific_controls(tab_widget);
}

// Helper function to create modern styled buttons
lv_obj_t* DebugAlignmentEnhanced::create_modern_button(lv_obj_t* parent, const char* text, lv_event_cb_t callback, uint32_t color, int width, int height) {
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_size(btn, width, height);
    
    // Modern button styling
    lv_obj_set_style_bg_color(btn, lv_color_hex(color), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_color(btn, lv_color_hex(color & 0x7F7F7F), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(btn, LV_GRAD_DIR_VER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 8, LV_PART_MAIN);
    lv_obj_set_style_border_color(btn, lv_color_hex(0xECF0F1), LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 1, LV_PART_MAIN);
    lv_obj_set_style_border_opa(btn, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(btn, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 3, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_30, LV_PART_MAIN);
    
    // Hover effect
    lv_obj_set_style_transform_zoom(btn, 105, LV_STATE_PRESSED);
    
    lv_obj_t* label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);
    
    if (callback) {
        lv_obj_add_event_cb(btn, callback, LV_EVENT_CLICKED, nullptr);
    }
    
    return btn;
}

// Create transform controls tab content
void DebugAlignmentEnhanced::create_transform_controls(lv_obj_t* parent) {
    lv_obj_set_style_pad_all(parent, 20, LV_PART_MAIN);
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x34495E), LV_PART_MAIN);
    
    // Movement section
    lv_obj_t* move_section = create_control_section(parent, " Position", 0, 0);
    
    // Grid layout for movement buttons
    lv_obj_t* move_grid = lv_obj_create(move_section);
    lv_obj_set_size(move_grid, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(move_grid, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(move_grid, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(move_grid, 10, LV_PART_MAIN);
    lv_obj_set_flex_flow(move_grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(move_grid, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(move_grid, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_column(move_grid, 10, LV_PART_MAIN);
    
    // Movement buttons with icons
    create_modern_button(move_grid, " L", move_left_event, 0x3498DB, 70, 50);
    create_modern_button(move_grid, " R", move_right_event, 0x3498DB, 70, 50);
    create_modern_button(move_grid, " U", move_up_event, 0x3498DB, 70, 50);
    create_modern_button(move_grid, " D", move_down_event, 0x3498DB, 70, 50);
    create_modern_button(move_grid, " L15", move_left_fast_event, 0x2980B9, 80, 50);
    create_modern_button(move_grid, " R15", move_right_fast_event, 0x2980B9, 80, 50);
    create_modern_button(move_grid, " U15", move_up_fast_event, 0x2980B9, 80, 50);
    create_modern_button(move_grid, " D15", move_down_fast_event, 0x2980B9, 80, 50);
    
    // Size section
    lv_obj_t* size_section = create_control_section(parent, " Size", 0, 150);
    
    lv_obj_t* size_grid = lv_obj_create(size_section);
    lv_obj_set_size(size_grid, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(size_grid, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(size_grid, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(size_grid, 10, LV_PART_MAIN);
    lv_obj_set_flex_flow(size_grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(size_grid, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(size_grid, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_column(size_grid, 10, LV_PART_MAIN);
    
    // Check if the selected object is an Arc widget
    SpawnedObject* active_obj = get_active_object();
    bool is_arc_widget = (active_obj && active_obj->type == WidgetType::ARC);
    
    if (is_arc_widget) {
        create_modern_button(size_grid, " Size+", resize_wider_event, 0x27AE60, 100, 50);
        create_modern_button(size_grid, " Size-", resize_narrower_event, 0xE74C3C, 100, 50);
    } else {
        create_modern_button(size_grid, " W+", resize_wider_event, 0x27AE60, 80, 50);
        create_modern_button(size_grid, " W-", resize_narrower_event, 0xE74C3C, 80, 50);
        create_modern_button(size_grid, " H+", resize_taller_event, 0x27AE60, 80, 50);
        create_modern_button(size_grid, " H-", resize_shorter_event, 0xE74C3C, 80, 50);
    }
    
    // Alignment section
    lv_obj_t* align_section = create_control_section(parent, " Alignment", 0, 280);
    
    lv_obj_t* align_grid = lv_obj_create(align_section);
    lv_obj_set_size(align_grid, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(align_grid, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(align_grid, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(align_grid, 10, LV_PART_MAIN);
    lv_obj_set_flex_flow(align_grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(align_grid, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(align_grid, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_column(align_grid, 10, LV_PART_MAIN);
    
    // Snap buttons
    create_modern_button(align_grid, " Snap L", snap_left_event, 0x9B59B6, 100, 50);
    create_modern_button(align_grid, " Snap R", snap_right_event, 0x9B59B6, 100, 50);
    create_modern_button(align_grid, " Snap T", snap_top_event, 0x9B59B6, 100, 50);
    create_modern_button(align_grid, " Snap B", snap_bottom_event, 0x9B59B6, 100, 50);
    create_modern_button(align_grid, " Center H", snap_center_h_event, 0x8E44AD, 110, 50);
    create_modern_button(align_grid, " Center V", snap_center_v_event, 0x8E44AD, 110, 50);
    create_modern_button(align_grid, " Center", snap_center_event, 0x8E44AD, 100, 50);
}

// Create style controls tab content  
void DebugAlignmentEnhanced::create_style_controls(lv_obj_t* parent) {
    lv_obj_set_style_pad_all(parent, 20, LV_PART_MAIN);
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x34495E), LV_PART_MAIN);
    
    // Visibility section
    lv_obj_t* vis_section = create_control_section(parent, " Visibility", 0, 0);
    
    lv_obj_t* vis_grid = lv_obj_create(vis_section);
    lv_obj_set_size(vis_grid, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(vis_grid, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(vis_grid, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(vis_grid, 10, LV_PART_MAIN);
    lv_obj_set_flex_flow(vis_grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(vis_grid, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(vis_grid, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_column(vis_grid, 10, LV_PART_MAIN);
    
    create_modern_button(vis_grid, " Hide", toggle_visibility_event, 0xE74C3C, 100, 50);
    create_modern_button(vis_grid, " Show", [](lv_event_t*) { 
        SpawnedObject* obj = get_active_object();
        if (!obj) return; 
        lv_obj_clear_flag(obj->object, LV_OBJ_FLAG_HIDDEN);
    }, 0x27AE60, 100, 50);
    
    create_modern_button(vis_grid, " Front", bring_to_front_event, 0x3498DB, 100, 50);
    create_modern_button(vis_grid, " Back", send_to_back_event, 0x3498DB, 100, 50);
    
    // Border & Effects section
    lv_obj_t* border_section = create_control_section(parent, " Border & Effects", 0, 120);
    
    lv_obj_t* border_grid = lv_obj_create(border_section);
    lv_obj_set_size(border_grid, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(border_grid, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(border_grid, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(border_grid, 10, LV_PART_MAIN);
    lv_obj_set_flex_flow(border_grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(border_grid, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(border_grid, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_column(border_grid, 10, LV_PART_MAIN);
    
    create_modern_button(border_grid, " B+", border_width_plus_event, 0xF39C12, 80, 50);
    create_modern_button(border_grid, " B-", border_width_minus_event, 0xF39C12, 80, 50);
    create_modern_button(border_grid, " R+", radius_plus_event, 0xE67E22, 80, 50);
    create_modern_button(border_grid, " R-", radius_minus_event, 0xE67E22, 80, 50);
    create_modern_button(border_grid, " O+", opacity_plus_event, 0x16A085, 80, 50);
    create_modern_button(border_grid, " O-", opacity_minus_event, 0x16A085, 80, 50);
    
    // Grid section
    lv_obj_t* grid_section = create_control_section(parent, " Grid Snap", 0, 240);
    
    lv_obj_t* grid_container = lv_obj_create(grid_section);
    lv_obj_set_size(grid_container, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(grid_container, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(grid_container, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(grid_container, 10, LV_PART_MAIN);
    lv_obj_set_flex_flow(grid_container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(grid_container, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_column(grid_container, 10, LV_PART_MAIN);
    
    create_modern_button(grid_container, grid_snap_enabled ? " Grid:On" : " Grid:Off", snap_to_grid_event, grid_snap_enabled ? 0x27AE60 : 0xE74C3C, 120, 50);
    create_modern_button(grid_container, " Size+", grid_size_plus_event, 0x3498DB, 100, 50);
    create_modern_button(grid_container, " Size-", grid_size_minus_event, 0x3498DB, 100, 50);
    
    // Grid size label
    if (!grid_label_ptr) {
        grid_label_ptr = lv_label_create(grid_container);
    } else {
        lv_obj_set_parent(grid_label_ptr, grid_container);
    }
    char gtxt[32];
    snprintf(gtxt, sizeof(gtxt), " Size: %d", grid_size);
    lv_label_set_text(grid_label_ptr, gtxt);
    lv_obj_set_style_text_color(grid_label_ptr, lv_color_hex(0xECF0F1), LV_PART_MAIN);
    lv_obj_set_style_text_font(grid_label_ptr, &lv_font_montserrat_16, LV_PART_MAIN);
}

// Create widget-specific controls tab content
void DebugAlignmentEnhanced::create_widget_specific_controls(lv_obj_t* parent) {
    lv_obj_set_style_pad_all(parent, 20, LV_PART_MAIN);
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x34495E), LV_PART_MAIN);
    
    if (active_object_index < 0 || active_object_index >= (int)spawned_objects.size()) return;
    
    widget_controls_parent = parent;  // Set the parent for widget controls
    update_controls_for_active_object();
}

// Helper function to create control sections with modern styling
lv_obj_t* DebugAlignmentEnhanced::create_control_section(lv_obj_t* parent, const char* title, int x, int y) {
    lv_obj_t* section = lv_obj_create(parent);
    lv_obj_set_size(section, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_align(section, LV_ALIGN_TOP_LEFT, x, y);
    
    // Modern section styling
    lv_obj_set_style_bg_color(section, lv_color_hex(0x2C3E50), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(section, LV_OPA_60, LV_PART_MAIN);
    lv_obj_set_style_radius(section, 10, LV_PART_MAIN);
    lv_obj_set_style_border_color(section, lv_color_hex(0x3498DB), LV_PART_MAIN);
    lv_obj_set_style_border_width(section, 1, LV_PART_MAIN);
    lv_obj_set_style_border_opa(section, LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_pad_all(section, 15, LV_PART_MAIN);
    
    // Section title
    lv_obj_t* title_label = lv_label_create(section);
    lv_label_set_text(title_label, title);
    lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xECF0F1), LV_PART_MAIN);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_20, LV_PART_MAIN);
    
    return section;
}

void DebugAlignmentEnhanced::update_controls_for_active_object() {
    if (active_object_index < 0 || active_object_index >= (int)spawned_objects.size()) return;
    if (!widget_controls_parent) return;
    
    // Clear existing widget-specific controls
    lv_obj_clean(widget_controls_parent);
    
    SpawnedObject& obj = spawned_objects[active_object_index];
    
    // Widget-specific controls based on type
    switch (obj.type) {
        case WidgetType::ARC: {
            // Arc Properties section
            lv_obj_t* arc_section = create_control_section(widget_controls_parent, " Arc Properties", 0, 0);
            
            lv_obj_t* arc_grid = lv_obj_create(arc_section);
            lv_obj_set_size(arc_grid, LV_PCT(100), LV_SIZE_CONTENT);
            lv_obj_set_style_bg_opa(arc_grid, LV_OPA_TRANSP, LV_PART_MAIN);
            lv_obj_set_style_border_opa(arc_grid, LV_OPA_TRANSP, LV_PART_MAIN);
            lv_obj_set_style_pad_all(arc_grid, 10, LV_PART_MAIN);
            lv_obj_set_flex_flow(arc_grid, LV_FLEX_FLOW_ROW_WRAP);
            lv_obj_set_flex_align(arc_grid, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
            lv_obj_set_style_pad_row(arc_grid, 10, LV_PART_MAIN);
            lv_obj_set_style_pad_column(arc_grid, 10, LV_PART_MAIN);
            
            // Range controls
            create_modern_button(arc_grid, " Min+", arc_range_min_plus_event, 0x3498DB, 100, 50);
            create_modern_button(arc_grid, " Min-", arc_range_min_minus_event, 0x3498DB, 100, 50);
            create_modern_button(arc_grid, " Max+", arc_range_max_plus_event, 0x3498DB, 100, 50);
            create_modern_button(arc_grid, " Max-", arc_range_max_minus_event, 0x3498DB, 100, 50);
            
            // Value controls  
            create_modern_button(arc_grid, " V+", arc_value_plus_event, 0x27AE60, 90, 50);
            create_modern_button(arc_grid, " V-", arc_value_minus_event, 0xE74C3C, 90, 50);
            
            // Rotation controls
            create_modern_button(arc_grid, " R+", arc_rotation_plus_event, 0x9B59B6, 90, 50);
            create_modern_button(arc_grid, " R-", arc_rotation_minus_event, 0x9B59B6, 90, 50);
            
            // Sweep angles
            create_modern_button(arc_grid, " S+", arc_start_angle_plus_event, 0xF39C12, 90, 50);
            create_modern_button(arc_grid, " S-", arc_start_angle_minus_event, 0xF39C12, 90, 50);
            create_modern_button(arc_grid, " E+", arc_end_angle_plus_event, 0xF39C12, 90, 50);
            create_modern_button(arc_grid, " E-", arc_end_angle_minus_event, 0xF39C12, 90, 50);
            
            // Indicator width
            create_modern_button(arc_grid, " W+", arc_width_plus_event, 0x16A085, 90, 50);
            create_modern_button(arc_grid, " W-", arc_width_minus_event, 0x16A085, 90, 50);
            create_modern_button(arc_grid, " Caps", arc_toggle_caps_event, 0x16A085, 100, 50);
            
            // Track controls
            create_modern_button(arc_grid, " TW+", arc_track_width_plus_event, 0xE67E22, 100, 50);
            create_modern_button(arc_grid, " TW-", arc_track_width_minus_event, 0xE67E22, 100, 50);
            create_modern_button(arc_grid, " Track", arc_toggle_track_event, 0xE67E22, 100, 50);
            
            // Padding
            create_modern_button(arc_grid, " P+", arc_pad_plus_event, 0x95A5A6, 90, 50);
            create_modern_button(arc_grid, " P-", arc_pad_minus_event, 0x95A5A6, 90, 50);
            break;
        }
        
        case WidgetType::BAR:
        case WidgetType::SLIDER: {
            const char* widget_name = (obj.type == WidgetType::BAR) ? "Bar" : "Slider";
            char section_title[50];
            snprintf(section_title, sizeof(section_title), " %s Properties", widget_name);
            
            lv_obj_t* bar_section = create_control_section(widget_controls_parent, section_title, 0, 0);
            
            lv_obj_t* bar_grid = lv_obj_create(bar_section);
            lv_obj_set_size(bar_grid, LV_PCT(100), LV_SIZE_CONTENT);
            lv_obj_set_style_bg_opa(bar_grid, LV_OPA_TRANSP, LV_PART_MAIN);
            lv_obj_set_style_border_opa(bar_grid, LV_OPA_TRANSP, LV_PART_MAIN);
            lv_obj_set_style_pad_all(bar_grid, 10, LV_PART_MAIN);
            lv_obj_set_flex_flow(bar_grid, LV_FLEX_FLOW_ROW_WRAP);
            lv_obj_set_flex_align(bar_grid, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
            lv_obj_set_style_pad_row(bar_grid, 10, LV_PART_MAIN);
            lv_obj_set_style_pad_column(bar_grid, 10, LV_PART_MAIN);
            
            // Range controls
            create_modern_button(bar_grid, " Min+", bar_min_plus_event, 0x3498DB, 100, 50);
            create_modern_button(bar_grid, " Min-", bar_min_minus_event, 0x3498DB, 100, 50);
            create_modern_button(bar_grid, " Max+", bar_max_plus_event, 0x3498DB, 100, 50);
            create_modern_button(bar_grid, " Max-", bar_max_minus_event, 0x3498DB, 100, 50);
            
            // Value controls
            create_modern_button(bar_grid, " Val+", bar_value_plus_event, 0x27AE60, 100, 50);
            create_modern_button(bar_grid, " Val-", bar_value_minus_event, 0xE74C3C, 100, 50);
            
            // Mode and orientation
            if (obj.type == WidgetType::BAR) {
                create_modern_button(bar_grid, " Mode", bar_toggle_mode_event, 0x9B59B6, 110, 50);
            }
            create_modern_button(bar_grid, " Flip", bar_toggle_orientation_event, 0x9B59B6, 100, 50);
            break;
        }
        
        case WidgetType::BUTTON:
        case WidgetType::SWITCH:
        case WidgetType::CHECKBOX:
        case WidgetType::LED: {
            const char* names[] = {"Button", "Switch", "Checkbox", "LED"};
            int type_index = (int)obj.type - (int)WidgetType::BUTTON;
            if (type_index < 0 || type_index > 3) type_index = 0;
            
            char section_title[50];
            snprintf(section_title, sizeof(section_title), "%s Properties", names[type_index]);
            
            lv_obj_t* state_section = create_control_section(widget_controls_parent, section_title, 0, 0);
            
            lv_obj_t* state_grid = lv_obj_create(state_section);
            lv_obj_set_size(state_grid, LV_PCT(100), LV_SIZE_CONTENT);
            lv_obj_set_style_bg_opa(state_grid, LV_OPA_TRANSP, LV_PART_MAIN);
            lv_obj_set_style_border_opa(state_grid, LV_OPA_TRANSP, LV_PART_MAIN);
            lv_obj_set_style_pad_all(state_grid, 10, LV_PART_MAIN);
            lv_obj_set_flex_flow(state_grid, LV_FLEX_FLOW_ROW_WRAP);
            lv_obj_set_flex_align(state_grid, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
            lv_obj_set_style_pad_row(state_grid, 10, LV_PART_MAIN);
            lv_obj_set_style_pad_column(state_grid, 10, LV_PART_MAIN);
            
            if (obj.type == WidgetType::BUTTON) {
                create_modern_button(state_grid, " Toggle", button_toggle_state_event, 0x3498DB, 120, 50);
            } else if (obj.type == WidgetType::SWITCH) {
                create_modern_button(state_grid, " Toggle", switch_toggle_state_event, 0x27AE60, 120, 50);
            } else if (obj.type == WidgetType::CHECKBOX) {
                create_modern_button(state_grid, " Toggle", checkbox_toggle_state_event, 0x9B59B6, 120, 50);
            } else if (obj.type == WidgetType::LED) {
                create_modern_button(state_grid, " Toggle", led_toggle_state_event, 0xF39C12, 120, 50);
            }
            break;
        }
        
        default:
            // For other widget types, show a modern placeholder
            lv_obj_t* placeholder_section = create_control_section(widget_controls_parent, " Widget Properties", 0, 0);
            
            lv_obj_t* placeholder = lv_label_create(placeholder_section);
            lv_label_set_text(placeholder, " Basic controls only\nUse Transform and Style tabs for customization");
            lv_obj_align(placeholder, LV_ALIGN_CENTER, 0, 10);
            lv_obj_set_style_text_color(placeholder, lv_color_hex(0xBDC3C7), LV_PART_MAIN);
            lv_obj_set_style_text_font(placeholder, &lv_font_montserrat_14, LV_PART_MAIN);
            lv_obj_set_style_text_align(placeholder, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
            break;
    }
}

void DebugAlignmentEnhanced::create_object_list() {
    lv_obj_clean(object_list_area);
    
    // Modern header title
    lv_obj_t* header_title = lv_label_create(object_list_area);
    lv_label_set_text(header_title, " Debug Toolkit");
    lv_obj_align(header_title, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_color(header_title, lv_color_hex(0xECF0F1), LV_PART_MAIN);
    lv_obj_set_style_text_font(header_title, &lv_font_montserrat_24, LV_PART_MAIN);
    
    // Subtitle
    lv_obj_t* subtitle = lv_label_create(object_list_area);
    lv_label_set_text(subtitle, "Manage your UI elements");
    lv_obj_align(subtitle, LV_ALIGN_TOP_LEFT, 0, 30);
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0xBDC3C7), LV_PART_MAIN);
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_14, LV_PART_MAIN);
    
    // Create "New Widget" button with modern styling
    lv_obj_t* new_btn = lv_btn_create(object_list_area);
    lv_obj_set_size(new_btn, 120, 45);
    lv_obj_align(new_btn, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_event_cb(new_btn, back_to_menu_event, LV_EVENT_CLICKED, nullptr);
    
    // Modern button styling
    lv_obj_set_style_bg_color(new_btn, lv_color_hex(0x27AE60), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_color(new_btn, lv_color_hex(0x2ECC71), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(new_btn, LV_GRAD_DIR_VER, LV_PART_MAIN);
    lv_obj_set_style_radius(new_btn, 12, LV_PART_MAIN);
    lv_obj_set_style_border_color(new_btn, lv_color_hex(0xECF0F1), LV_PART_MAIN);
    lv_obj_set_style_border_width(new_btn, 2, LV_PART_MAIN);
    lv_obj_set_style_border_opa(new_btn, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(new_btn, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(new_btn, 5, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(new_btn, LV_OPA_30, LV_PART_MAIN);
    
    // Hover effect
    lv_obj_set_style_transform_zoom(new_btn, 105, LV_STATE_PRESSED);
    
    lv_obj_t* new_label = lv_label_create(new_btn);
    lv_label_set_text(new_label, " New Widget");
    lv_obj_center(new_label);
    lv_obj_set_style_text_color(new_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(new_label, &lv_font_montserrat_14, LV_PART_MAIN);
    
    update_object_list();
}

void DebugAlignmentEnhanced::update_object_list() {
    if (!object_list_area) return;
    
    printf("[DEBUG] Updating object list...\n");
    
    // Clear existing object buttons (but keep the header and "New" button)
    uint32_t child_count = lv_obj_get_child_count(object_list_area);
    for (uint32_t i = child_count; i > 3; i--) {  // Keep first 3 children (title, subtitle, new button)
        lv_obj_t* child = lv_obj_get_child(object_list_area, i - 1);
        if (child) {
            lv_obj_delete(child);
        }
    }
    
    printf("[DEBUG] Cleared existing buttons, creating new ones...\n");
    
    if (spawned_objects.empty()) {
        // Show "No objects" message
        lv_obj_t* empty_label = lv_label_create(object_list_area);
        lv_label_set_text(empty_label, "No widgets created yet");
        lv_obj_align(empty_label, LV_ALIGN_TOP_LEFT, 200, 20);
        lv_obj_set_style_text_color(empty_label, lv_color_hex(0x95A5A6), LV_PART_MAIN);
        lv_obj_set_style_text_font(empty_label, &lv_font_montserrat_14, LV_PART_MAIN);
        return;
    }
    
    // Create object chips/buttons in a horizontal layout
    int x_pos = 10;
    int y_pos = 55;  // Below the header
    
    for (size_t i = 0; i < spawned_objects.size() && i < 6; i++) {  // Limit to 6 visible objects
        lv_obj_t* obj_chip = lv_btn_create(object_list_area);
        lv_obj_set_size(obj_chip, 100, 30);
        lv_obj_set_pos(obj_chip, x_pos, y_pos);
        
        // Chip styling
        if ((int)i == active_object_index) {
            // Active object styling
            lv_obj_set_style_bg_color(obj_chip, lv_color_hex(0x3498DB), LV_PART_MAIN);
            lv_obj_set_style_bg_grad_color(obj_chip, lv_color_hex(0x2980B9), LV_PART_MAIN);
            lv_obj_set_style_border_color(obj_chip, lv_color_hex(0xECF0F1), LV_PART_MAIN);
            lv_obj_set_style_border_width(obj_chip, 2, LV_PART_MAIN);
        } else {
            // Inactive object styling
            lv_obj_set_style_bg_color(obj_chip, lv_color_hex(0x95A5A6), LV_PART_MAIN);
            lv_obj_set_style_bg_grad_color(obj_chip, lv_color_hex(0x7F8C8D), LV_PART_MAIN);
            lv_obj_set_style_border_color(obj_chip, lv_color_hex(0xBDC3C7), LV_PART_MAIN);
            lv_obj_set_style_border_width(obj_chip, 1, LV_PART_MAIN);
        }
        
        lv_obj_set_style_bg_grad_dir(obj_chip, LV_GRAD_DIR_VER, LV_PART_MAIN);
        lv_obj_set_style_radius(obj_chip, 15, LV_PART_MAIN);
        lv_obj_set_style_border_opa(obj_chip, LV_OPA_50, LV_PART_MAIN);
        lv_obj_set_style_shadow_color(obj_chip, lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_set_style_shadow_width(obj_chip, 3, LV_PART_MAIN);
        lv_obj_set_style_shadow_opa(obj_chip, LV_OPA_20, LV_PART_MAIN);
        
        // Hover effect
        lv_obj_set_style_transform_zoom(obj_chip, 105, LV_STATE_PRESSED);
        
        // Store index in user data for the event handler
        lv_obj_set_user_data(obj_chip, (void*)(uintptr_t)i);
        lv_obj_add_event_cb(obj_chip, [](lv_event_t* e) {
            lv_obj_t* target = (lv_obj_t*)lv_event_get_target(e);
            int index = (int)(uintptr_t)lv_obj_get_user_data(target);
            set_active_object(index);
        }, LV_EVENT_CLICKED, nullptr);
        
        lv_obj_t* obj_label = lv_label_create(obj_chip);
        
        // Create shortened name for display
        std::string display_name = spawned_objects[i].name;
        if (display_name.length() > 8) {
            display_name = display_name.substr(0, 8) + "...";
        }
        
        lv_label_set_text(obj_label, display_name.c_str());
        lv_obj_center(obj_label);
        lv_obj_set_style_text_color(obj_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_text_font(obj_label, &lv_font_montserrat_14, LV_PART_MAIN);
        
        x_pos += 110;
        if (x_pos > 650) {  // Wrap to next row if needed
            x_pos = 10;
            y_pos += 35;
        }
    }
    
    // Show count if there are more objects
    if (spawned_objects.size() > 6) {
        lv_obj_t* more_label = lv_label_create(object_list_area);
        char more_text[32];
        snprintf(more_text, sizeof(more_text), "+%d more", (int)(spawned_objects.size() - 6));
        lv_label_set_text(more_label, more_text);
        lv_obj_set_pos(more_label, x_pos, y_pos);
        lv_obj_set_style_text_color(more_label, lv_color_hex(0x95A5A6), LV_PART_MAIN);
        lv_obj_set_style_text_font(more_label, &lv_font_montserrat_14, LV_PART_MAIN);
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
    
    // Position the widget at center manually (not using alignment)
    lv_coord_t screen_w = lv_obj_get_width(parent_screen);
    lv_coord_t screen_h = lv_obj_get_height(parent_screen);
    lv_coord_t widget_w = lv_obj_get_width(widget);
    lv_coord_t widget_h = lv_obj_get_height(widget);
    
    // Set initial position manually without using alignment
    int center_x = (screen_w - widget_w) / 2;
    int center_y = (screen_h - widget_h) / 2;
    lv_obj_set_pos(widget, center_x, center_y);
    
    // Initialize transform values with actual widget position and size
    obj.transform.x_offset = lv_obj_get_x(widget);
    obj.transform.y_offset = lv_obj_get_y(widget);
    obj.transform.width = lv_obj_get_width(widget);
    obj.transform.height = lv_obj_get_height(widget);
    obj.transform.rotation = 0;  // Fix: Initialize rotation to 0
    obj.transform.pad = 0;       // Also initialize pad for completeness
    
    // Initialize widget-specific properties based on type
    switch (type) {
        case WidgetType::ARC:
            obj.props.start_angle = 0;
            obj.props.end_angle = 270;
            obj.props.arc_width = 15;
            obj.props.track_width = 5;
            obj.props.track_visible = true;
            obj.props.rounded_caps = false;
            obj.props.value = 50;
            obj.props.range_min = 0;
            obj.props.range_max = 100;
            obj.props.rotation = 0;  // Also store in props for consistency
            break;
        case WidgetType::BAR:
        case WidgetType::SLIDER:
            obj.props.min_value = 0;
            obj.props.max_value = 100;
            obj.props.current_value = 50;
            obj.props.horizontal = true;
            break;
        default:
            // Initialize common properties to safe defaults
            obj.props.state = false;
            break;
    }
    
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
    // Apply position using lv_obj_set_pos to avoid alignment conflicts
    lv_obj_set_pos(obj.object, obj.transform.x_offset, obj.transform.y_offset);

    // Apply rotation: use semantic rotation for widgets that expose it (Arc),
    // otherwise use style transform angle (LVGL uses 0.1° units).
    if (obj.type == WidgetType::ARC) {
        // Arc uses its own semantic rotation API; don't combine with style transform
        lv_arc_set_rotation(obj.object, obj.transform.rotation);
        // Ensure style transform angle is cleared so we don't combine them
        lv_obj_set_style_transform_angle(obj.object, 0, LV_PART_MAIN);
    } else {
        // Use style transform angle for generic rotation (degrees * 10)
        lv_obj_set_style_transform_angle(obj.object, obj.transform.rotation * 10, LV_PART_MAIN);
    }

    // Apply size using width/height helpers so widgets that have single size semantics
    // (if any) can be handled elsewhere. For now, set both.
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
        "// Transform (common)\n"
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
                "#define %s_TRACK_WIDTH  %d\n"
                "#define %s_TRACK_VISIBLE %d\n"
                "#define %s_ROUNDED_CAPS %d\n"
                "#define %s_VALUE        %d\n"
                "#define %s_RANGE_MIN    %d\n"
                "#define %s_RANGE_MAX    %d\n",
                obj.name.c_str(), obj.props.start_angle,
                obj.name.c_str(), obj.props.end_angle,
                obj.name.c_str(), obj.props.arc_width,
                obj.name.c_str(), obj.props.track_width,
                obj.name.c_str(), obj.props.track_visible ? 1 : 0,
                obj.name.c_str(), obj.props.rounded_caps ? 1 : 0,
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
                "#define %s_HORIZONTAL   %d\n"
                "#define %s_INDICATOR_W  %d\n",
                obj.name.c_str(), obj.props.min_value,
                obj.name.c_str(), obj.props.max_value,
                obj.name.c_str(), obj.props.current_value,
                obj.name.c_str(), obj.props.horizontal ? 1 : 0,
                obj.name.c_str(), obj.props.indicator_width);
            break;
            
        default:
            // Emit placeholder zeros for unsupported widget-specific fields when exporting
            n += snprintf(export_text + n, sizeof(export_text) - n,
                "// No specific properties for this widget type\n"
                "#define %s_SPECIFIC_1 0\n"
                "#define %s_SPECIFIC_2 0\n",
                obj.name.c_str(), obj.name.c_str());
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

// Transform control event handlers (using 2px steps as suggested)
void DebugAlignmentEnhanced::move_left_event(lv_event_t*) {
    printf("[DEBUG MOVE] LEFT event triggered\n");
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    int old_x = lv_obj_get_x(obj->object);
    int old_y = lv_obj_get_y(obj->object);
    int new_x = old_x - 2;
    lv_obj_set_pos(obj->object, new_x, old_y);  // Move LEFT by subtracting from X
    printf("[DEBUG MOVE] LEFT: %d -> %d\n", old_x, new_x);
    obj->transform.x_offset = new_x;
}

void DebugAlignmentEnhanced::move_right_event(lv_event_t*) {
    printf("[DEBUG MOVE] RIGHT event triggered\n");
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    int old_x = lv_obj_get_x(obj->object);
    int old_y = lv_obj_get_y(obj->object);
    int new_x = old_x + 2;
    lv_obj_set_pos(obj->object, new_x, old_y);  // Move RIGHT by adding to X
    printf("[DEBUG MOVE] RIGHT: %d -> %d\n", old_x, new_x);
    obj->transform.x_offset = new_x;
}

void DebugAlignmentEnhanced::move_up_event(lv_event_t*) {
    printf("[DEBUG MOVE] UP event triggered\n");
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    int old_x = lv_obj_get_x(obj->object);
    int old_y = lv_obj_get_y(obj->object);
    int new_y = old_y - 2;
    lv_obj_set_pos(obj->object, old_x, new_y);  // Move UP by subtracting from Y
    printf("[DEBUG MOVE] UP: %d -> %d\n", old_y, new_y);
    obj->transform.y_offset = new_y;
}

void DebugAlignmentEnhanced::move_down_event(lv_event_t*) {
    printf("[DEBUG MOVE] DOWN event triggered\n");
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    int old_x = lv_obj_get_x(obj->object);
    int old_y = lv_obj_get_y(obj->object);
    int new_y = old_y + 2;
    lv_obj_set_pos(obj->object, old_x, new_y);  // Move DOWN by adding to Y
    printf("[DEBUG MOVE] DOWN: %d -> %d\n", old_y, new_y);
    obj->transform.y_offset = new_y;
}

void DebugAlignmentEnhanced::move_left_fast_event(lv_event_t*) {
    printf("[DEBUG MOVE] LEFT FAST event triggered\n");
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    int old_x = lv_obj_get_x(obj->object);
    int old_y = lv_obj_get_y(obj->object);
    int new_x = old_x - 15;
    lv_obj_set_pos(obj->object, new_x, old_y);  // Fast LEFT movement
    printf("[DEBUG MOVE] LEFT FAST: %d -> %d\n", old_x, new_x);
    obj->transform.x_offset = new_x;
}

void DebugAlignmentEnhanced::move_right_fast_event(lv_event_t*) {
    printf("[DEBUG MOVE] RIGHT FAST event triggered\n");
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    int old_x = lv_obj_get_x(obj->object);
    int old_y = lv_obj_get_y(obj->object);
    int new_x = old_x + 15;
    lv_obj_set_pos(obj->object, new_x, old_y);  // Fast RIGHT movement
    printf("[DEBUG MOVE] RIGHT FAST: %d -> %d\n", old_x, new_x);
    obj->transform.x_offset = new_x;
}

void DebugAlignmentEnhanced::move_up_fast_event(lv_event_t*) {
    printf("[DEBUG MOVE] UP FAST event triggered\n");
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    int old_x = lv_obj_get_x(obj->object);
    int old_y = lv_obj_get_y(obj->object);
    int new_y = old_y - 15;
    lv_obj_set_pos(obj->object, old_x, new_y);  // Fast UP movement
    printf("[DEBUG MOVE] UP FAST: %d -> %d\n", old_y, new_y);
    obj->transform.y_offset = new_y;
}

void DebugAlignmentEnhanced::move_down_fast_event(lv_event_t*) {
    printf("[DEBUG MOVE] DOWN FAST event triggered\n");
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    int old_x = lv_obj_get_x(obj->object);
    int old_y = lv_obj_get_y(obj->object);
    int new_y = old_y + 15;
    lv_obj_set_pos(obj->object, old_x, new_y);  // Fast DOWN movement
    printf("[DEBUG MOVE] DOWN FAST: %d -> %d\n", old_y, new_y);
    obj->transform.y_offset = new_y;
}

void DebugAlignmentEnhanced::rotate_cw_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj) return;
    // rotation stored in degrees; add 5 degrees per step
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

// Snap helpers
void DebugAlignmentEnhanced::snap_left_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || !obj->object) return;
    lv_obj_t* parent = lv_obj_get_parent(obj->object);
    if (!parent) parent = parent_screen;
    // Small margin
    const int margin = 5;
    obj->transform.x_offset = margin;
    lv_obj_set_x(obj->object, obj->transform.x_offset);
}

void DebugAlignmentEnhanced::snap_right_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || !obj->object) return;
    lv_obj_t* parent = lv_obj_get_parent(obj->object);
    if (!parent) parent = parent_screen;
    lv_coord_t pw = lv_obj_get_width(parent);
    lv_coord_t ow = lv_obj_get_width(obj->object);
    const int margin = 5;
    obj->transform.x_offset = pw - ow - margin;
    lv_obj_set_x(obj->object, obj->transform.x_offset);
}

void DebugAlignmentEnhanced::snap_top_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || !obj->object) return;
    const int margin = 5;
    obj->transform.y_offset = margin;
    lv_obj_set_y(obj->object, obj->transform.y_offset);
}

void DebugAlignmentEnhanced::snap_bottom_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || !obj->object) return;
    lv_obj_t* parent = lv_obj_get_parent(obj->object);
    if (!parent) parent = parent_screen;
    lv_coord_t ph = lv_obj_get_height(parent);
    lv_coord_t oh = lv_obj_get_height(obj->object);
    const int margin = 5;
    obj->transform.y_offset = ph - oh - margin;
    lv_obj_set_y(obj->object, obj->transform.y_offset);
}

void DebugAlignmentEnhanced::snap_center_h_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || !obj->object) return;
    lv_obj_t* parent = lv_obj_get_parent(obj->object);
    if (!parent) parent = parent_screen;
    lv_coord_t pw = lv_obj_get_width(parent);
    lv_coord_t ow = lv_obj_get_width(obj->object);
    obj->transform.x_offset = (pw - ow) / 2;
    apply_transform_to_object(*obj);
}

void DebugAlignmentEnhanced::snap_center_v_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || !obj->object) return;
    lv_obj_t* parent = lv_obj_get_parent(obj->object);
    if (!parent) parent = parent_screen;
    lv_coord_t ph = lv_obj_get_height(parent);
    lv_coord_t oh = lv_obj_get_height(obj->object);
    obj->transform.y_offset = (ph - oh) / 2;
    apply_transform_to_object(*obj);
}

void DebugAlignmentEnhanced::snap_center_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || !obj->object) return;
    lv_obj_t* parent = lv_obj_get_parent(obj->object);
    if (!parent) parent = parent_screen;
    lv_coord_t pw = lv_obj_get_width(parent);
    lv_coord_t ph = lv_obj_get_height(parent);
    lv_coord_t ow = lv_obj_get_width(obj->object);
    lv_coord_t oh = lv_obj_get_height(obj->object);
    obj->transform.x_offset = (pw - ow) / 2;
    obj->transform.y_offset = (ph - oh) / 2;
    apply_transform_to_object(*obj);
}

void DebugAlignmentEnhanced::snap_to_grid_event(lv_event_t* e) {
    // Toggle grid snapping for active object; if enabling immediately snap the object
    grid_snap_enabled = !grid_snap_enabled;
    SpawnedObject* obj = get_active_object();
    if (obj && obj->object && grid_snap_enabled) {
        // Snap current position to grid
        int gx = (obj->transform.x_offset + grid_size/2) / grid_size * grid_size;
        int gy = (obj->transform.y_offset + grid_size/2) / grid_size * grid_size;
        obj->transform.x_offset = gx;
        obj->transform.y_offset = gy;
        apply_transform_to_object(*obj);
    }
    // Update persistent grid label if present
    if (grid_label_ptr) {
        char gtxt[32];
        snprintf(gtxt, sizeof(gtxt), "Size: %d%s", grid_size, grid_snap_enabled ? " (On)" : " (Off)");
        lv_label_set_text(grid_label_ptr, gtxt);
    }
    // Update the caller button label if we have an event
    if (e) {
        lv_obj_t* target = (lv_obj_t*)lv_event_get_target(e);
        if (target) {
            lv_obj_t* lbl = lv_obj_get_child(target, 0);
            if (lbl) lv_label_set_text(lbl, grid_snap_enabled ? "Grid:On" : "Grid:Off");
        }
    }
}

void DebugAlignmentEnhanced::grid_size_plus_event(lv_event_t* e) {
    grid_size = std::min(200, grid_size + 5);
    // If grid snapping is active, optionally snap current object
    SpawnedObject* obj = get_active_object();
    if (obj && obj->object && grid_snap_enabled) {
        int gx = (obj->transform.x_offset + grid_size/2) / grid_size * grid_size;
        int gy = (obj->transform.y_offset + grid_size/2) / grid_size * grid_size;
        obj->transform.x_offset = gx;
        obj->transform.y_offset = gy;
        apply_transform_to_object(*obj);
    }
    if (grid_label_ptr) {
        char gtxt[32];
        snprintf(gtxt, sizeof(gtxt), "Size: %d%s", grid_size, grid_snap_enabled ? " (On)" : " (Off)");
        lv_label_set_text(grid_label_ptr, gtxt);
    }
    (void)e;
}

void DebugAlignmentEnhanced::grid_size_minus_event(lv_event_t* e) {
    grid_size = std::max(1, grid_size - 5);
    SpawnedObject* obj = get_active_object();
    if (obj && obj->object && grid_snap_enabled) {
        int gx = (obj->transform.x_offset + grid_size/2) / grid_size * grid_size;
        int gy = (obj->transform.y_offset + grid_size/2) / grid_size * grid_size;
        obj->transform.x_offset = gx;
        obj->transform.y_offset = gy;
        apply_transform_to_object(*obj);
    }
    if (grid_label_ptr) {
        char gtxt[32];
        snprintf(gtxt, sizeof(gtxt), "Size: %d%s", grid_size, grid_snap_enabled ? " (On)" : " (Off)");
        lv_label_set_text(grid_label_ptr, gtxt);
    }
    (void)e;
}

void DebugAlignmentEnhanced::delete_active_event(lv_event_t*) {
    delete_active_object();
}

void DebugAlignmentEnhanced::toggle_section_event(lv_event_t* e) {
    if (!widget_controls_parent) return;
    if (lv_obj_has_flag(widget_controls_parent, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_clear_flag(widget_controls_parent, LV_OBJ_FLAG_HIDDEN);
        // Update toggle button label if possible
        if (e) {
            lv_obj_t* tgt = (lv_obj_t*)lv_event_get_target(e);
            if (tgt) {
                lv_obj_t* lbl = lv_obj_get_child(tgt, 0);
                if (lbl) lv_label_set_text(lbl, "Hide Widget Controls");
            }
        }
    } else {
        lv_obj_add_flag(widget_controls_parent, LV_OBJ_FLAG_HIDDEN);
        if (e) {
            lv_obj_t* tgt = (lv_obj_t*)lv_event_get_target(e);
            if (tgt) {
                lv_obj_t* lbl = lv_obj_get_child(tgt, 0);
                if (lbl) lv_label_set_text(lbl, "Show Widget Controls");
            }
        }
    }
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

void DebugAlignmentEnhanced::button_toggle_disabled_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::BUTTON) return;
    obj->props.disabled = !obj->props.disabled;
    if (obj->props.disabled) {
        lv_obj_add_state(obj->object, LV_STATE_DISABLED);
    } else {
        lv_obj_clear_state(obj->object, LV_STATE_DISABLED);
    }
}

void DebugAlignmentEnhanced::checkbox_toggle_disabled_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::CHECKBOX) return;
    obj->props.disabled = !obj->props.disabled;
    if (obj->props.disabled) {
        lv_obj_add_state(obj->object, LV_STATE_DISABLED);
    } else {
        lv_obj_clear_state(obj->object, LV_STATE_DISABLED);
    }
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
    obj->transform.rotation += 5;  // Fix: store in transform, not props
    obj->props.rotation = obj->transform.rotation;  // Keep props in sync
    lv_arc_set_rotation(obj->object, obj->transform.rotation);
}

void DebugAlignmentEnhanced::arc_rotation_minus_event(lv_event_t*) {
    SpawnedObject* obj = get_active_object();
    if (!obj || obj->type != WidgetType::ARC) return;
    obj->transform.rotation -= 5;  // Fix: store in transform, not props
    obj->props.rotation = obj->transform.rotation;  // Keep props in sync
    lv_arc_set_rotation(obj->object, obj->transform.rotation);
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

#endif
