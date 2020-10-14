/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/egtlog.h"
#include "detail/video/gstappsinkimpl.h"
#include "detail/video/gstkmssinkimpl.h"
#include "egt/app.h"
#include "egt/detail/filesystem.h"
#include "egt/respath.h"
#include "egt/video.h"
#include <fstream>
#include <string>
#include <thread>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Check if target device is sama5d4 for using hardware
 * decoder.
 */

bool is_target_sama5d4()
{
    std::ifstream infile("/proc/device-tree/model");
    if (infile.is_open())
    {
        std::string line;
        while (getline(infile, line))
        {
            EGTLOG_DEBUG("CPU: {}", line);
            if (line.find("SAMA5D4") != std::string::npos)
            {
                infile.close();
                return true;
            }
        }
        infile.close();
    }
    return false;
}

/*
 * Check if audio playback device is present
 */
bool audio_device()
{
    std::ifstream infile("/proc/asound/devices");
    if (infile.is_open())
    {
        std::string line;
        while (getline(infile, line))
        {
            EGTLOG_DEBUG("Sound : {}", line);
            if (line.find("audio playback") != std::string::npos)
            {
                infile.close();
                return true;
            }
        }
        infile.close();
    }
    return false;
}

WindowHint check_windowhint(WindowHint& hint)
{
    if (hint == WindowHint::cursor_overlay)
    {
        throw std::runtime_error("Cannot create Videowindow with cursor_overlay hint");
    }
    return hint;
}

} // End of detail.

VideoWindow::VideoWindow(const Rect& rect, PixelFormat format, WindowHint hint)
    : Window(rect, format, detail::check_windowhint(hint))
{
    fill_flags().clear();

    create_impl(rect.size());
}

VideoWindow::VideoWindow(const Rect& rect, const std::string& uri,
                         PixelFormat format, WindowHint hint)
    : VideoWindow(rect, format, hint)
{

    if (!media(uri))
    {
        throw std::runtime_error("failed to initialize gstreamer pipeline");
    }
}

void VideoWindow::create_impl(const Size& size)
{
    if (plane_window() && detail::is_target_sama5d4())
    {
        EGTLOG_DEBUG("VideoWindow: Using KMS sink");
#ifdef HAVE_LIBPLANES
        m_video_impl = std::make_unique<detail::GstKmsSinkImpl>(*this, size, detail::is_target_sama5d4());
#endif
    }
    else
    {
        EGTLOG_DEBUG("VideoWindow: Using APP sink");
        m_video_impl = std::make_unique<detail::GstAppSinkImpl>(*this, size);
    }
}

void VideoWindow::draw(Painter& painter, const Rect& rect)
{
    m_video_impl->draw(painter, rect);
}

int64_t VideoWindow::position() const
{
    return m_video_impl->position();
}

int64_t VideoWindow::duration() const
{
    return m_video_impl->duration();
}

bool VideoWindow::media(const std::string& uri)
{
    std::string path;
    auto type = detail::resolve_path(uri, path);

    switch (type)
    {
    case detail::SchemeType::network:
        return m_video_impl->media(uri);
    case detail::SchemeType::filesystem:
    {
        auto gstpath = "file://" + path;
        return m_video_impl->media(gstpath);
    }
    default:
    {
        throw std::runtime_error("unsupported uri: " + uri);
    }
    }

    return false;
}

bool VideoWindow::pause()
{
    return m_video_impl->pause();
}

bool VideoWindow::playing() const
{
    return m_video_impl->playing();
}

bool VideoWindow::play()
{
    return m_video_impl->play();
}

bool VideoWindow::volume(int volume)
{
    m_video_impl->volume(volume);
    return true;
}

int VideoWindow::volume() const
{
    return m_video_impl->volume();
}

bool VideoWindow::seek(int64_t pos)
{
    return m_video_impl->seek(pos);
}

void VideoWindow::scale(float hscale, float vscale)
{
    auto xs = detail::change_if_diff<float>(m_hscale, hscale);
    auto ys = detail::change_if_diff<float>(m_vscale, vscale);

    if (xs || ys)
    {
        if (!plane_window())
        {
            m_video_impl->scale(m_hscale, m_vscale);
        }
        else
        {
            Window::scale(m_hscale, m_vscale);
        }
    }
}

bool VideoWindow::has_audio() const
{
    return m_video_impl->has_audio();
}

VideoWindow::VideoWindow(VideoWindow&&) noexcept = default;
VideoWindow& VideoWindow::operator=(VideoWindow&&) noexcept = default;

VideoWindow::~VideoWindow() noexcept
{
    m_video_impl->destroyPipeline();
}

} //namespace v1

} //namespace egt
