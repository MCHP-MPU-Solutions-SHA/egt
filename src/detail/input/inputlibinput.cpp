/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "detail/asioallocator.h"
#include "detail/dump.h"
#include "detail/input/inputkeyboard.h"
#include "egt/app.h"
#include "egt/detail/input/inputlibinput.h"
#include "egt/detail/meta.h"
#include "egt/detail/string.h"
#include "egt/eventloop.h"
#include "egt/keycode.h"
#include "egt/screen.h"
#include <cstdarg>
#include <libinput.h>
#include <libudev.h>
#include <linux/input.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

static inline bool libinput_verbose()
{
    static int value = 0;
    if (value == 0)
    {
        if (std::getenv("EGT_LIBINPUT_VERBOSE"))
            value += 1;
        else
            value -= 1;
    }
    return value == 1;
}

namespace egt
{
inline namespace v1
{
namespace detail
{

struct InputLibInput::LibInputImpl
{
    detail::HandlerAllocator allocator;
    /// Keyboard mapping instance.
    InputKeyboard keyboard;
};

static int
open_restricted(const char* path, int flags, void* user_data)
{
    ignoreparam(user_data);

    int fd = open(path, flags);
    return fd < 0 ? -errno : fd;
}

static void
close_restricted(int fd, void* user_data)
{
    ignoreparam(user_data);
    close(fd);
}

static constexpr struct libinput_interface interface =
{
    open_restricted,
    close_restricted,
};

static void log_handler(struct libinput*,
                        enum libinput_log_priority,
                        const char* fmt, va_list args)
{
    char buffer[1024];
    vsprintf(buffer, fmt, args);
    detail::info(buffer);
}

static struct libinput* tools_open_udev(const char* seat, bool grab)
{
    struct libinput* libinput_handle;
    struct udev* udev = udev_new();

    if (!udev)
    {
        detail::warn("Failed to initialize udev");
        return nullptr;
    }

    libinput_handle = libinput_udev_create_context(&interface, &grab, udev);
    if (!libinput_handle)
    {
        detail::warn("Failed to initialize context from udev");
        udev_unref(udev);
        return nullptr;
    }

    if (libinput_verbose())
    {
        libinput_log_set_handler(libinput_handle, log_handler);
        libinput_log_set_priority(libinput_handle, LIBINPUT_LOG_PRIORITY_DEBUG);
    }

    if (libinput_udev_assign_seat(libinput_handle, seat))
    {
        detail::warn("failed to set seat");
        libinput_unref(libinput_handle);
        udev_unref(udev);
        return nullptr;
    }

    udev_unref(udev);
    return libinput_handle;
}

InputLibInput::InputLibInput(Application& app)
    : m_app(app),
      m_input(app.event().io()),
      m_impl(std::make_unique<LibInputImpl>())
{
    const char* seat_or_device = "seat0";
    m_libinput_handle = tools_open_udev(seat_or_device, false);
    if (!m_libinput_handle)
    {
        detail::warn("could not open libinput device: {}", seat_or_device);
        return;
    }

    m_input.assign(libinput_get_fd(m_libinput_handle));

    // go ahead and enumerate devices and start the first async_read
#ifdef USE_PRIORITY_QUEUE
    asio::async_read(m_input, asio::null_buffers(),
                     m_app.event().queue().wrap(detail::priorities::moderate,
                             [this](const asio::error_code & error, std::size_t)
    {
        handle_read(error);
    }));
#else
    asio::async_read(m_input, asio::null_buffers(),
                     detail::make_custom_alloc_handler(m_impl->allocator,
                             [this](const asio::error_code & error, std::size_t)
    {
        handle_read(error);
    }));
#endif
}

void InputLibInput::handle_event_device_notify(struct libinput_event* ev)
{
    struct libinput_device* dev = libinput_event_get_device(ev);
    const char* type;

    if (libinput_event_get_type(ev) == LIBINPUT_EVENT_DEVICE_ADDED)
        type = "added";
    else
        type = "removed";

    m_libinput_handle = libinput_event_get_context(ev);

    // if the device is handled by another backend, disable libinput events
    for (auto& device : m_app.get_input_devices())
    {
        std::vector<std::string> tokens;
        detail::tokenize(device.second, '/', tokens);

        if (!tokens.empty())
        {
            if (tokens.back() == libinput_device_get_sysname(dev))
            {
                if (device.first != "libinput")
                {
                    libinput_device_config_send_events_set_mode(dev,
                            LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
                    return;
                }
            }
        }
    }

    detail::info("{} {} {}", type, libinput_device_get_sysname(dev),
                 libinput_device_get_name(dev));
}

void InputLibInput::handle_event_touch(struct libinput_event* ev)
{
    struct libinput_event_touch* t = libinput_event_get_touch_event(ev);
    auto slot = libinput_event_touch_get_seat_slot(t);

    if (slot < 0 || slot >= static_cast<decltype(slot)>(m_last_point.size()))
        return;

    switch (libinput_event_get_type(ev))
    {
    case LIBINPUT_EVENT_TOUCH_UP:
    {
        Event event(EventId::raw_pointer_up, Pointer(m_last_point[slot], slot));
        dispatch(event);
        break;
    }
    case LIBINPUT_EVENT_TOUCH_DOWN:
    {
        const auto& screen_size = Application::instance().screen()->size();
        const auto x = libinput_event_touch_get_x_transformed(t, screen_size.width());
        const auto y = libinput_event_touch_get_y_transformed(t, screen_size.height());

        m_last_point[slot] = DisplayPoint(x, y);

        Event event(EventId::raw_pointer_down, Pointer(m_last_point[slot], slot));
        dispatch(event);
        break;
    }
    case LIBINPUT_EVENT_TOUCH_MOTION:
    {
        const auto& screen_size = Application::instance().screen()->size();
        const auto x = libinput_event_touch_get_x_transformed(t, screen_size.width());
        const auto y = libinput_event_touch_get_y_transformed(t, screen_size.height());

        m_last_point[slot] = DisplayPoint(x, y);
        Event event(EventId::raw_pointer_move, Pointer(m_last_point[slot], slot));
        dispatch(event);
        break;
    }
    default:
        break;
    }
}

void InputLibInput::handle_event_pointer_motion(struct libinput_event* ev)
{
    struct libinput_event_pointer* t = libinput_event_get_pointer_event(ev);

    const auto x = libinput_event_pointer_get_dx(t);
    const auto y = libinput_event_pointer_get_dy(t);

    m_last_point[0] += DisplayPoint(x, y);
    Event event(EventId::raw_pointer_move, Pointer(m_last_point[0], 0));
    dispatch(event);
}

void InputLibInput::handle_event_pointer_motion_absolute(struct libinput_event* ev)
{
    struct libinput_event_pointer* t = libinput_event_get_pointer_event(ev);

    const auto& screen_size = Application::instance().screen()->size();
    const auto x = libinput_event_pointer_get_absolute_x_transformed(t, screen_size.width());
    const auto y = libinput_event_pointer_get_absolute_y_transformed(t, screen_size.height());

    m_last_point[0] = DisplayPoint(x, y);
    Event event(EventId::raw_pointer_move, Pointer(m_last_point[0], 0));
    dispatch(event);
}

void InputLibInput::handle_event_keyboard(struct libinput_event* ev)
{
    struct libinput_event_keyboard* k = libinput_event_get_keyboard_event(ev);
    const auto key = libinput_event_keyboard_get_key(k);

    EGTLOG_TRACE("key:{} state:{}", key, libinput_event_keyboard_get_key_state(k));

    static const auto EVDEV_OFFSET = 8;

    switch (libinput_event_keyboard_get_key_state(k))
    {
    case LIBINPUT_KEY_STATE_PRESSED:
    {
        const auto unicode = m_impl->keyboard.on_key(key + EVDEV_OFFSET, EventId::keyboard_down);
        Event event(EventId::keyboard_down, Key(linux_to_ekey(key), unicode));
        dispatch(event);
        break;
    }
    case LIBINPUT_KEY_STATE_RELEASED:
    {
        const auto unicode = m_impl->keyboard.on_key(key + EVDEV_OFFSET, EventId::keyboard_up);
        Event event(EventId::keyboard_up, Key(linux_to_ekey(key), unicode));
        dispatch(event);
        break;
    }
    }
}

void InputLibInput::handle_event_button(struct libinput_event* ev)
{
    struct libinput_event_pointer* p = libinput_event_get_pointer_event(ev);
    const auto button = libinput_event_pointer_get_button(p);

    EGTLOG_TRACE("button:{}", button);

    Pointer::Button b = Pointer::Button::none;
    switch (button)
    {
    case BTN_LEFT:
        b = Pointer::Button::left;
        break;
    case BTN_MIDDLE:
        b = Pointer::Button::middle;
        break;
    case BTN_RIGHT:
        b = Pointer::Button::right;
        break;
    default:
        /* Other buttons not handled. */
        /* For compatibility reasons, all additional buttons go after the old 4-7 scroll ones */
        return;
    }

    if (b != Pointer::Button::none)
    {
        const bool is_press = libinput_event_pointer_get_button_state(p) == LIBINPUT_BUTTON_STATE_PRESSED;
        Event event(is_press ? EventId::raw_pointer_down : EventId::raw_pointer_up,
                    Pointer(m_last_point[0], b));
        dispatch(event);
    }
}

static inline bool time_input_enabled()
{
    static int value = 0;
    if (value == 0)
    {
        if (std::getenv("EGT_TIME_INPUT"))
            value += 1;
        else
            value -= 1;
    }
    return value == 1;
}

void InputLibInput::handle_read(const asio::error_code& error)
{
    if (error)
    {
        detail::error("{}", error);
        return;
    }

    detail::code_timer(time_input_enabled(), "libinput: ", [this]()
    {
        struct libinput_event* ev;

        libinput_dispatch(m_libinput_handle);

        while ((ev = libinput_get_event(m_libinput_handle)))
        {
            switch (libinput_event_get_type(ev))
            {
            case LIBINPUT_EVENT_NONE:
                break;
            case LIBINPUT_EVENT_DEVICE_ADDED:
            case LIBINPUT_EVENT_DEVICE_REMOVED:
                handle_event_device_notify(ev);
                break;
            case LIBINPUT_EVENT_KEYBOARD_KEY:
                handle_event_keyboard(ev);
                break;
            case LIBINPUT_EVENT_POINTER_BUTTON:
                handle_event_button(ev);
                break;
            case LIBINPUT_EVENT_TOUCH_DOWN:
            case LIBINPUT_EVENT_TOUCH_MOTION:
            case LIBINPUT_EVENT_TOUCH_UP:
                handle_event_touch(ev);
                break;
            case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE:
                handle_event_pointer_motion_absolute(ev);
                break;
            case LIBINPUT_EVENT_POINTER_MOTION:
                handle_event_pointer_motion(ev);
                break;
            /* These events are not handled. */
            case LIBINPUT_EVENT_POINTER_AXIS:
            case LIBINPUT_EVENT_TOUCH_CANCEL:
            case LIBINPUT_EVENT_TOUCH_FRAME:
            case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
            case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
            case LIBINPUT_EVENT_GESTURE_SWIPE_END:
            case LIBINPUT_EVENT_GESTURE_PINCH_BEGIN:
            case LIBINPUT_EVENT_GESTURE_PINCH_UPDATE:
            case LIBINPUT_EVENT_GESTURE_PINCH_END:
            case LIBINPUT_EVENT_TABLET_TOOL_AXIS:
            case LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY:
            case LIBINPUT_EVENT_TABLET_TOOL_TIP:
            case LIBINPUT_EVENT_TABLET_TOOL_BUTTON:
            case LIBINPUT_EVENT_TABLET_PAD_BUTTON:
            case LIBINPUT_EVENT_TABLET_PAD_RING:
            case LIBINPUT_EVENT_TABLET_PAD_STRIP:
            default:
                break;
            }

            libinput_event_destroy(ev);
        }

#ifdef USE_PRIORITY_QUEUE
        asio::async_read(m_input, asio::null_buffers(),
                         m_app.event().queue().wrap(detail::priorities::moderate,
                                 [this](const asio::error_code & error, std::size_t)
        {
            handle_read(error);
        }));
#else
        asio::async_read(m_input, asio::null_buffers(),
                         detail::make_custom_alloc_handler(m_impl->allocator,
                                 [this](const asio::error_code & error, std::size_t)
        {
            handle_read(error);
        }));
#endif
    });
}

InputLibInput::~InputLibInput() noexcept
{
    libinput_unref(m_libinput_handle);
}

}
}
}
