/*
File: debug_alignment_example.cpp
Author: Will Jenkins
Purpose: Example integration of the enhanced debug alignment system

Usage:
1. Include debug_alignment_enhanced.hpp in your screen class
2. Call DebugAlignmentEnhanced::init(screen) when creating your screen
3. Call DebugAlignmentEnhanced::cleanup() when destroying your screen
4. Optionally call DebugAlignmentEnhanced::toggle() to show/hide programmatically

The debug overlay will show a "DBG" button in the top-right corner that toggles the debug panel.

Features:
- Spawn new widgets from a menu (Arc, Bar, Button, Canvas, Chart, etc.)
- Select and manage multiple spawned objects
- Move, resize, rotate objects with precise controls
- Widget-specific property controls (arc angles, bar values, etc.)
- Export alignment constants to copy into production code
- Z-order management (bring to front/send to back)
- Clean object deletion and management
*/

#include "debug_alignment_enhanced.hpp"
#include "lvgl_screen_root.hpp"  // Your existing screen

class ExampleScreenWithDebug {
private:
    lv_obj_t* screen;
    
public:
    void create_screen() {
        screen = lv_obj_create(nullptr);
        
        // Create your normal UI elements here
        lv_obj_t* title = lv_label_create(screen);
        lv_label_set_text(title, "Example Screen with Debug Overlay");
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 50);
        
        lv_obj_t* normal_button = lv_btn_create(screen);
        lv_obj_set_size(normal_button, 200, 60);
        lv_obj_align(normal_button, LV_ALIGN_CENTER, 0, 0);
        
        lv_obj_t* btn_label = lv_label_create(normal_button);
        lv_label_set_text(btn_label, "Normal UI Button");
        lv_obj_center(btn_label);
        
        // Initialize debug overlay AFTER creating your normal UI
        DebugAlignmentEnhanced::init(screen);
        
        lv_screen_load(screen);
    }
    
    void cleanup_screen() {
        // Clean up debug overlay BEFORE deleting the screen
        DebugAlignmentEnhanced::cleanup();
        
        if (screen) {
            lv_obj_delete(screen);
            screen = nullptr;
        }
    }
    
    void toggle_debug() {
        DebugAlignmentEnhanced::toggle();
    }
};

/*
Example of integrating into your existing screen class:

In your lvgl_screen_root.hpp:
#include "debug_alignment_enhanced.hpp"

In your create_screen() method:
void LVGLScreenRoot::create_screen(lv_obj_t* parent) {
    screen = lv_obj_create(parent);
    
    // ... create your normal UI elements ...
    
    // Add debug overlay (only in debug builds)
    #if ENABLE_DEBUG_ALIGNMENT
        DebugAlignmentEnhanced::init(screen);
    #endif
}

In your destructor or cleanup method:
LVGLScreenRoot::~LVGLScreenRoot() {
    #if ENABLE_DEBUG_ALIGNMENT
        DebugAlignmentEnhanced::cleanup();
    #endif
    
    // ... rest of cleanup ...
}

Optional: Add a keyboard shortcut to toggle debug overlay:
void LVGLScreenRoot::handle_key_event(lv_event_t* e) {
    if (lv_event_get_code(e) == LV_EVENT_KEY) {
        uint32_t key = lv_indev_get_key(lv_indev_get_active());
        if (key == LV_KEY_F12) {  // F12 to toggle debug
            DebugAlignmentEnhanced::toggle();
        }
    }
}
*/

// Simple test function you can call from main
void test_debug_alignment() {
    ExampleScreenWithDebug test_screen;
    test_screen.create_screen();
    
    printf("Debug alignment test screen created.\n");
    printf("Look for the 'DBG' button in the top-right corner.\n");
    printf("Click it to open the debug overlay and spawn widgets.\n");
    
    // The screen will remain active until you clean it up
    // test_screen.cleanup_screen();
}
