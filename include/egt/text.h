/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_TEXT_H
#define EGT_TEXT_H

/**
 * @file
 * @brief Working with text input.
 */

#include <egt/detail/meta.h>
#include <egt/flags.h>
#include <egt/font.h>
#include <egt/image.h>
#include <egt/textwidget.h>
#include <egt/timer.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * Input text box.
 *
 * Supported Features:
 * - UTF-8 encoding
 * - Cursor movement
 * - Selection Copy/Delete
 * - Multi-line
 *
 * @b Example
 * @code{.cpp}
 * TextBox textbox;
 * textbox.text_flags().set({TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap});
 * @endcode
 *
 * @image html widget_textbox.png
 * @image latex widget_textbox.png "Widget Textbox" height=5cm
 *
 * @ingroup controls
 */
class EGT_API TextBox : public TextWidget
{
public:

    /// Text flags.
    enum class TextFlag : uint32_t
    {
        /// When not multiline, only allow max length to be what can fit.
        fit_to_width = detail::bit(0),

        /// Enable multi-line text.
        multiline = detail::bit(1),

        /// Wrap at word boundaries instead of character boundaries.  Must be flag::multiline.
        word_wrap = detail::bit(2),

        /// Do not display a virtual keyboard when focus is gained.
        no_virt_keyboard = detail::bit(3),
    };

    /// Text flags.
    using TextFlags = egt::Flags<TextFlag>;

    /**
     * Validator callback type.
     * @see add_validator_function()
     */
    using ValidatorCallback = std::function<bool(const std::string&)>;

    /// Change text align.
    static AlignFlags default_text_align();
    /// Change default text align.
    static void default_text_align(const AlignFlags& align);

    /**
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     * @param[in] flags TextBox flags.
     */
    explicit TextBox(const std::string& text = {},
                     const AlignFlags& text_align = default_text_align(),
                     const TextFlags& flags = {}) noexcept;

    /**
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     * @param[in] flags TextBox flags.
     */
    explicit TextBox(const std::string& text,
                     const TextFlags& flags,
                     const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     * @param[in] flags TextBox flags.
     */
    TextBox(const std::string& text,
            const Rect& rect,
            const AlignFlags& text_align = default_text_align(),
            const TextFlags& flags = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     * @param[in] flags TextBox flags.
     */
    explicit TextBox(Frame& parent,
                     const std::string& text = {},
                     const AlignFlags& text_align = default_text_align(),
                     const TextFlags& flags = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     * @param[in] flags TextBox flags.
     */
    TextBox(Frame& parent,
            const std::string& text,
            const Rect& rect,
            const AlignFlags& text_align = default_text_align(),
            const TextFlags& flags = {}) noexcept;

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit TextBox(Serializer::Properties& props) noexcept;

    TextBox(const TextBox&) = delete;
    TextBox& operator=(const TextBox&) = delete;
    TextBox(TextBox&&) = default;
    TextBox& operator=(TextBox&&) = default;

    void handle(Event& event) override;

    void draw(Painter& painter, const Rect& rect) override;

    using TextWidget::text;

    void text(const std::string& str) override;

    void clear() override;

    using TextWidget::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override;

    /**
     * Set the maximum allowed length of the text.
     *
     * @param len The maximum length of the text.
     */
    void max_length(size_t len);

    /**
     * Get the max length of allowed text.
     */
    EGT_NODISCARD size_t max_length() const { return m_max_len;}

    /**
     * Get a const ref of the flags.
     */
    EGT_NODISCARD const TextFlags& text_flags() const { return m_text_flags; }

    /**
     * Move the cursor to the end and insert.
     *
     * The is the same as doing:
     * @code{.cpp}
     * cursor_end();
     * insert(str);
     * @endcode
     *
     * @param str The string to append.
     */
    size_t append(const std::string& str);

    /**
     * Insert text at the cursor.
     *
     * @param str The string to insert.
     * @return The number of characters inserted.
     */
    size_t insert(const std::string& str);

    /**
     * Get the cursor position.
     */
    EGT_NODISCARD size_t cursor() const;

    /**
     * Move the cursor to the beginning.
     */
    void cursor_begin();

    /**
     * Move the cursor to the end.
     */
    void cursor_end();

    /**
     * Move the cursor forward by the specified count from the current
     * position.
     *
     * @param count The number of characters to move.
     */
    void cursor_forward(size_t count = 1);

    /**
     * Move the cursor backward by the specified count from the current
     * position.
     *
     * @param count The number of characters to move.
     */
    void cursor_backward(size_t count = 1);

    /**
     * Set the cursor to the specific position.
     *
     * @param pos The new cursor position.
     */
    void cursor_set(size_t pos);

    /**
     * Select all of the text.
     */
    void selection_all();

    /**
     * Set the selection of text.
     *
     * @param pos The starting position.
     * @param length The length of the selection.
     */
    void selection(size_t pos, size_t length);

    /**
     * Get the start position of the selection.
     */
    EGT_NODISCARD size_t selection_start() const
    {
        return m_select_start;
    }

    /**
     * Get the length of the selection.
     */
    EGT_NODISCARD size_t selection_length() const
    {
        return m_select_len;
    }

    /**
     * Clear any selection.
     *
     * @note This does not delete text, it just clears the selection. To delete
     * the selected text call delete_selection().
     */
    void selection_clear();

    /**
     * Get, or copy, the selected text.
     *
     * @return The selected text, or an empty string if there is no selection.
     */
    EGT_NODISCARD std::string selected_text() const;

    /**
     * Delete the selected text.
     */
    void selection_delete();

    /**
     * Enable or disable input validation.
     *
     * Invoke the validator callbacks. If one of them returns false, the input
     * is rejected.
     */
    void input_validation_enabled(bool enabled);

    /**
     * Add a callback to be invoked to validate the input.
     *
     * Anytime the text is changed, the callback will be invoked with the new
     * text for validation.
     *
     * To enable this, you must set input_validation_enabled() to true.
     */
    void add_validator_function(ValidatorCallback callback);

    void serialize(Serializer& serializer) const override;

    ~TextBox() noexcept override;

protected:

    /// Type array used for validator callbacks.
    using ValidatorCallbackArray = std::vector<ValidatorCallback>;

    /// Show/enable the visibility of the cursor.
    void show_cursor();

    /// If the cursor is shown, keep is shown some more.
    void continue_show_cursor();

    /// Hide/disable the visibility of the cursor.
    void hide_cursor();

    /// Process key events.
    void handle_key(const Key& key);

    /// Validate the input against the validator pattern.
    bool validate_input(const std::string& str);

    /// Timer for blinking the cursor.
    PeriodicTimer m_timer;

    /**
     * The current position of the cursor.
     * This is a UTF-8 offset.
     */
    size_t m_cursor_pos{0};

    /**
     * Selection start position.
     * This is a UTF-8 offset.
     */
    size_t m_select_start{0};

    /// Selection length.
    size_t m_select_len{0};

    /// Validation of the input.
    bool m_validate_input{false};

    /// Callbacks invoked to validate the input.
    ValidatorCallbackArray m_validator_callbacks;

    /**
     * Given text, return the number of UTF8 characters that will fit on a
     * single line inside of the widget.
     */
    EGT_NODISCARD size_t width_to_len(const std::string& str) const;

private:

    /// Callback for the cursor timeout.
    void cursor_timeout();

    /// The current visible state of the cursor.
    bool m_cursor_state{false};

    /// TextBox flags.
    TextFlags m_text_flags{};

    /// Maximum text length, or zero.
    size_t m_max_len{0};

    /// Gain focus registration.
    Signal<>::RegisterHandle m_gain_focus_reg{};

    /// Lost focus registration.
    Signal<>::RegisterHandle m_lost_focus_reg{};

    void initialize();

    void deserialize(Serializer::Properties& props) override;
};

namespace detail
{
/// Internal draw text function.
EGT_API void draw_text(Painter& painter,
                       const Rect& b,
                       const std::string& text,
                       const Font& font,
                       const TextBox::TextFlags& flags,
                       const AlignFlags& text_align,
                       Justification justify,
                       const Pattern& text_color,
                       const std::function<void(const Point& offset, size_t height)>& draw_cursor = nullptr,
                       size_t cursor_pos = 0,
                       const Pattern& highlight_color = {},
                       size_t select_start = 0,
                       size_t select_len = 0);

/// Internal draw text function with associated image.
EGT_API void draw_text(Painter& painter,
                       const Rect& b,
                       const std::string& text,
                       const Font& font,
                       const TextBox::TextFlags& flags,
                       const AlignFlags& text_align,
                       Justification justify,
                       const Pattern& text_color,
                       const AlignFlags& image_align,
                       const Image& image,
                       const std::function<void(const Point& offset, size_t height)>& draw_cursor = nullptr,
                       size_t cursor_pos = 0,
                       const Pattern& highlight_color = {},
                       size_t select_start = 0,
                       size_t select_len = 0);
}

}
}

#endif
