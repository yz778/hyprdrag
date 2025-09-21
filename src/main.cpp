#define WLR_USE_UNSTABLE

#include <unistd.h>
#include <vector>

#include <hyprland/src/includes.hpp>
#include <any>
#include <thread>
#include <chrono>
#include <format>

#define private public
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/managers/input/InputManager.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprland/src/managers/EventManager.hpp>
#undef private

#include "globals.hpp"

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

// Callback storage
static SP<HOOK_CALLBACK_FN> mouseMoveCallback = nullptr;
static SP<HOOK_CALLBACK_FN> mouseButtonCallback = nullptr;

// State tracking variables
static bool wasCurrentlyDragging = false;
static bool wasMoving = false;
static PHLWINDOW lastDraggedWindow = nullptr;

// Cached string constants
static constexpr const char* EVENT_DRAG_MOVE_START      = "dragstart-move";
static constexpr const char* EVENT_DRAG_MOVE_END        = "dragend-move";
static constexpr const char* EVENT_DRAG_RESIZE_START    = "dragstart-resize";
static constexpr const char* EVENT_DRAG_RESIZE_END      = "dragend-resize";
static constexpr const char* TAG_DRAGGING_MOVE          = "dragging-move";
static constexpr const char* TAG_DRAGGING_RESIZE        = "dragging-resize";
static constexpr const char* TAG_REMOVE_DRAGGING_MOVE   = "-dragging-move";
static constexpr const char* TAG_REMOVE_DRAGGING_RESIZE = "-dragging-resize";

static void handleDragEvent(PHLWINDOW window, bool isMoving, bool isStart) {
    if (!window) {
        return;
    }

    std::string address = std::format("{:x}", reinterpret_cast<uintptr_t>(window.get()));

    // [isStart][isMoving] indexing
    static constexpr const char* events[2][2] = {
        {EVENT_DRAG_RESIZE_END, EVENT_DRAG_MOVE_END},     // end events
        {EVENT_DRAG_RESIZE_START, EVENT_DRAG_MOVE_START}  // start events
    };

    static constexpr const char* tags[2][2] = {
        {TAG_REMOVE_DRAGGING_RESIZE, TAG_REMOVE_DRAGGING_MOVE}, // end tags
        {TAG_DRAGGING_RESIZE, TAG_DRAGGING_MOVE}                // start tags
    };

    // Send IPC event
    g_pEventManager->postEvent(SHyprIPCEvent{events[isStart][isMoving], address});

    // Store the current focused window to restore after tagWindow
    auto currentWindow = g_pCompositor->m_lastWindow.lock();

    // Focus and tag the window
    g_pCompositor->focusWindow(window);
    g_pKeybindManager->tagWindow(std::string(tags[isStart][isMoving]));

    // Restore the previously focused window if it was different
    if (currentWindow && currentWindow != window) {
        g_pCompositor->focusWindow(currentWindow);
    }
}

static void handleDragEnd() {
    if (!lastDraggedWindow) {
        return;
    }

    handleDragEvent(lastDraggedWindow, wasMoving, false);
    lastDraggedWindow = nullptr;
}

void onMouseMove(void* self, SCallbackInfo& info, std::any param) {
    const auto dragMode = g_pInputManager->m_dragMode;
    const bool isMoving = dragMode == MBIND_MOVE;
    bool isCurrentlyDragging = !g_pInputManager->m_currentlyDraggedWindow.expired();

    auto currentDraggedWindow = g_pInputManager->m_currentlyDraggedWindow.lock();

    if (isCurrentlyDragging && !wasCurrentlyDragging) { // Drag started
        wasMoving = isMoving;

        if (currentDraggedWindow) {
            lastDraggedWindow = currentDraggedWindow;
            handleDragEvent(currentDraggedWindow, isMoving, true);
        }
    } else if (!isCurrentlyDragging && wasCurrentlyDragging) { // Drag ended
        handleDragEnd();
    }

    wasCurrentlyDragging = isCurrentlyDragging;
}

void onMouseButton(void* self, SCallbackInfo& info, std::any param) {
    auto event = std::any_cast<IPointer::SButtonEvent>(param);

    // Only care about button releases
    if (event.state != WL_POINTER_BUTTON_STATE_RELEASED) {
        return;
    }

    // Check if we were dragging and now we're not
    if (wasCurrentlyDragging && g_pInputManager->m_currentlyDraggedWindow.expired()) {
        handleDragEnd();
        wasCurrentlyDragging = false;
    }
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    const std::string HASH = __hyprland_api_get_hash();

    if (HASH != GIT_COMMIT_HASH) {
        throw std::runtime_error("[hyprevent] Version mismatch");
    }

    // Register mouse move callback instead of drag callbacks
    mouseMoveCallback = HyprlandAPI::registerCallbackDynamic(PHANDLE, "mouseMove", onMouseMove);

    return {"hyprevent", "Window interaction event plugin", "Hyprland Community", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    mouseMoveCallback = nullptr;
    wasCurrentlyDragging = false;
    lastDraggedWindow = nullptr;
}
