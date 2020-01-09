/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_VIDEO_H
#define EGT_VIDEO_H

#include <egt/detail/meta.h>
#include <egt/detail/signal.h>
#include <egt/window.h>

namespace egt
{
inline namespace v1
{
namespace detail
{
bool is_target_sama5d4();
bool audio_device();
class GstDecoderImpl;
class GstKmsSinkImpl;
class GstAppSinkImpl;
}

/**
 * A VideoWindow is a widget to decode video and render it to a screen.
 *
 * It has a bounding rectangle, format and WindowHint. These properties can
 * be manipulated to create a video window either as a basic window or an
 * overlay plane.
 *
 * The video decoding is done through gstreamer media framework.
 *
 * @ingroup media
 */
class EGT_API VideoWindow : public Window
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when the position of the player changes.
     */
    detail::Signal<> on_position_changed;

    /**
     * Invoked when an error occurs.
     */
    detail::Signal<> on_error;

    /**
     * Invoked on end of stream.
     */
    detail::Signal<> on_eos;
    /** @} */

    /**
     * Create a video window to decode video and render it to a screen.
     *
     * @param rect Size of window with offset x & y.
     * @param format Pixel format of window or a overlay plane.
     * @param hint Used for configuring window backend's.
     *
     * @note Only WindowHint::heo_overlay can use yuyv, nv21 and yuv420 pixel
     * formats.
     */
    explicit VideoWindow(const Rect& rect = {},
                         PixelFormat format = PixelFormat::xrgb8888,
                         WindowHint hint = WindowHint::overlay);

    /**
    * Create a video window to decode video and render it to a screen.
    *
    * @param rect Size of window with offset x & y.
    * @param uri Media file
    * @param format Pixel format of window or a overlay plane.
    * @param hint Used for configuring window backend's.
    *
    * @note Only WindowHint::heo_overlay can use yuyv, nv21 and yuv420 pixel
    * formats.
    */
    VideoWindow(const Rect& rect,
                const std::string& uri,
                PixelFormat format = PixelFormat::xrgb8888,
                WindowHint hint = WindowHint::overlay);

    virtual void do_draw() override
    {
        // video windows don't draw
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Initialize gstreamer pipeline for specified media file.
     *
     * @param uri Media file
     * @return true if success
     */
    virtual bool media(const std::string& uri);

    /**
     * Play the video.
     *
     * @return true if success
     */
    virtual bool play();

    /**
     * Pause the video.
     *
     * @return true if success
     */
    virtual bool pause();

    /**
     * Check is video in play state.
     *
     * @return true if success
     */
    virtual bool playing() const;

    /**
     * Get the current position of the video being played.
     *
     * @return 64bit time value nanosec's
     */
    int64_t position() const;

    /**
     * Get the total duration of the video.
     *
     * @return 64bit time value in nanosec's
     */
    int64_t duration() const;

    /**
     * Adjust volume of the video being played.
     *
     * @param volume is in range of 0 (no sound) to 10 (normal sound)
     * @return true if success
     */
    virtual bool volume(double volume);

    /**
     * Get the volume value for the video being played.
     */
    virtual double volume() const;

    /**
     * Seek the video being played.
     *
     * @param time in nanoseconds
     * @return true if seek success
     */
    virtual bool seek(int64_t time);

    /**
     * Enable/disable continues loop-back mode of the video
     * being played. by default this is disabled.
     *
     * @param enable to enable/disable loop-back mode.
     */
    inline void loopback(bool enable)
    {
        m_loopback = enable;
    }

    /**
     * Get loop-back state
     *
     * @return true/false based on loop-back state
     */
    inline bool loopback() const
    {
        return m_loopback;
    }

    /**
     * Scale the video by scale value.
     *
     * @param[in] scalex Horizontal scale value, 0-1.
     * @param[in] scaley Vertical scale value, 0-1.
     */
    virtual void scale(float scalex, float scaley) override;

    /**
     * Get video scale value.
     */
    inline float scalex() const
    {
        return m_scalex;
    }

    inline float scaley() const
    {
        return m_scaley;
    }

    /**
     * check for audio is supported. check is done based on
     * sound device and audio track present in media file.
     *
     * @return true if supported and false if not supported.
     */
    virtual bool has_audio() const;

    /**
     * Get error message
     */
    std::string error_message() const;

    virtual ~VideoWindow();

protected:
    bool m_loopback;
    float m_scalex{1.0};
    float m_scaley{1.0};

    void createImpl(const Size& size);

    std::shared_ptr<detail::GstDecoderImpl> m_decoderImpl;

    friend class detail::GstDecoderImpl;
    friend class detail::GstKmsSinkImpl;
    friend class detail::GstAppSinkImpl;

};

} //namespace v1

} //namespace egt

#endif
