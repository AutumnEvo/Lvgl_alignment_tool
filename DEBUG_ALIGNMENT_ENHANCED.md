# Enhanced Debug Alignment Overlay - Quick Start Guide

## Overview
The Enhanced Debug Alignment system provides a comprehensive tool for spawning, positioning, and configuring LVGL widgets directly in your running application. It's designed to help you quickly prototype UI layouts and extract exact positioning values for production code.

## Features

### Widget Spawning
- **14 Widget Types**: Arc, Bar, Button, Canvas, Chart, Checkbox, LED, Message Box, Slider, Spin Box, Spinner, Switch, Table, Pie Chart
- **Instant Creation**: Click a button to spawn a widget centered on screen
- **Multiple Objects**: Manage multiple spawned widgets simultaneously

### Object Management
- **Object List**: Visual list showing all spawned objects with selection
- **Active Object**: One selected object receives all control inputs
- **Clean Deletion**: Remove objects safely with automatic list updates

### Transform Controls
- **Precise Movement**: 1px steps with Left/Right/Up/Down buttons
- **Fast Movement**: 15px steps with Left+15/Right+15/Up+15/Down+15 buttons
- **Rotation**: Clockwise/Counter-clockwise rotation (where supported)
- **Resizing**: Width/Height adjustment with +/- buttons
- **Z-Order**: Bring to front / Send to back

### Widget-Specific Properties
- **Arc**: Start/End angles, arc width, value, range, rounded caps
- **Bar/Slider**: Min/Max values, current value, orientation toggle
- **Button/Switch/Checkbox/LED**: State toggles, label text
- **Others**: Basic transform controls (more can be added as needed)

### Export System
- **Copy-Ready Constants**: Generate #define statements for all values
- **Production Ready**: Export exact positioning for use in your final code
- **Console Output**: Values printed to stdout for easy copying

## Quick Integration

### 1. Include the Header
```cpp
#include "debug_alignment_enhanced.hpp"
```

### 2. Initialize on Your Screen
```cpp
void YourScreen::create_screen() {
    screen = lv_obj_create(parent);
    
    // Create your normal UI elements first
    // ...
    
    // Add debug overlay last
    #if ENABLE_DEBUG_ALIGNMENT
        DebugAlignmentEnhanced::init(screen);
    #endif
}
```

### 3. Clean Up When Done
```cpp
void YourScreen::cleanup() {
    #if ENABLE_DEBUG_ALIGNMENT
        DebugAlignmentEnhanced::cleanup();
    #endif
    
    // Rest of cleanup...
}
```

## Usage Workflow

### 1. Toggle Debug Panel
- Look for the "DBG" button in the top-right corner
- Click to show/hide the debug overlay

### 2. Spawn Widgets
- In the spawn menu, click any widget button (Arc, Bar, Button, etc.)
- Widget appears centered on screen and becomes the active object
- Control panel automatically switches to show controls for that widget

### 3. Adjust Position and Size
- Use Left/Right/Up/Down for precise 1px movement
- Use Left+15/Right+15/Up+15/Down+15 for faster movement
- Use W+/W-/H+/H- to resize width and height
- Use CW/CCW to rotate (where supported)

### 4. Adjust Widget Properties
- Arc: Modify start/end angles, width, value with S+/S-/E+/E-/W+/W-/V+/V-
- Bar/Slider: Adjust min/max/value and toggle orientation
- Buttons/Switches: Toggle states and modify labels

### 5. Manage Multiple Objects
- Object list at top shows all spawned widgets
- Click any object button to make it active
- Blue highlight shows currently active object
- "New" button returns to spawn menu

### 6. Export Values
- Click "Export" to generate #define constants
- Values printed to console/stdout
- Copy and paste into your production code

### 7. Clean Up
- "Delete" button removes active object
- "Back" button returns to spawn menu
- Toggle DBG button to hide overlay

## Example Output
When you click Export, you'll get output like:
```c
[DEBUG EXPORT] ===== Arc #1 VALUES =====
// Transform
#define Arc #1_X_OFFSET     150
#define Arc #1_Y_OFFSET     200
#define Arc #1_WIDTH        120
#define Arc #1_HEIGHT       120
#define Arc #1_ROTATION     0
#define Arc #1_PAD          0
// Arc Properties
#define Arc #1_START_ANGLE  0
#define Arc #1_END_ANGLE    270
#define Arc #1_ARC_WIDTH    22
#define Arc #1_VALUE        75
#define Arc #1_RANGE_MIN    0
#define Arc #1_RANGE_MAX    100
==============================
```

## Tips
- Start with the spawn menu to create widgets
- Use the object list to switch between multiple spawned objects
- Export frequently to save positioning values
- The overlay is non-intrusive - it won't interfere with your main UI
- All spawned objects are automatically cleaned up when you call cleanup()

## Build Requirements
- LVGL 8.x or later
- C++11 or later (for std::vector and std::function)
- CMake will automatically include the new files

## Customization
You can easily extend the system by:
- Adding new widget types to the WidgetType enum
- Implementing create_widget_instance() for new types
- Adding widget-specific property controls
- Modifying button layouts and sizes
- Adding new transform operations

The system is designed to be modular and extensible while maintaining a clean, easy-to-use interface.
