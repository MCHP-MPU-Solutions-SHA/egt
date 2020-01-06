/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_VIRTUALKEYBOARD_H
#define EGT_VIRTUALKEYBOARD_H

/**
 * @file
 * @brief On-screen VirtualKeyboard.
 */

#include <egt/button.h>
#include <egt/detail/meta.h>
#include <egt/input.h>
#include <egt/keycode.h>
#include <egt/notebook.h>
#include <egt/popup.h>
#include <egt/sizer.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * VirtualKeyboard class.
 *
 * The VirtualKeyboard class provides a way to simulate a physical keyboard.
 * By default, a Qwerty keyboard is provided but you can easily create your own
 * keyboard by giving a list of keys divided in panels and rows. A base size of
 * the keys is automatically computed.
 */
class EGT_API VirtualKeyboard : public Frame
{
protected:
    struct Panel;

public:
    class Key;
    using panel_keys = std::vector<std::vector<std::shared_ptr<Key>>>;
    /**
     * @brief Key widget for the Keyboard widget.
     *
     * This widget allows to describe a key that will be used by the Keyboard
     * widget. There are different kinds of keys:
     * - The basic key corresponds to a simple printable character defined by
     * its unicode value.
     * - For non printable characters, the keycode has to be set. The label of
     * the key can be a string or an image.
     * - The link key allows to switch from one panel to another. The label of
     * the key can be a string or an image.
     * - The multichoice key is a basic key which allows to display a
     * multichoice panel with several keys when a long press event happens.
     *
     * Even if the VirtualKeyboard class computes automatically the base size
     * of the keys, it's still possible to customize the size of a key by
     * setting its length which is a factor for the base size width.
     */
    class Key
    {
    public:

        /**
         * @param[in] unicode UTF32 code point for the key. It's also the label
         * of the key.
         * @param[in] length Length of the key.
         */
        explicit Key(uint32_t unicode, double length = 1.0) noexcept;

        /**
         * @param[in] label Label of the key.
         * @param[in] keycode Code of the key.
         * @param[in] length Length of the key.
         */
        explicit Key(const std::string& label, KeyboardCode keycode,
                     double length = 1.0) noexcept;

        /**
         * @param[in] img Image of the key.
         * @param[in] keycode Code of the key.
         * @param[in] length Length of the key.
         */
        explicit Key(const Image& img, KeyboardCode keycode,
                     double length = 1.0) noexcept;

        /**
         * @param[in] label Label of the key.
         * @param[in] link Id of the main panel to display when clicking on this
         * key.
         * The id depends on the order of registration of the main panels. It
         * starts from 0.
         * @param[in] length Length of the key.
         */
        explicit Key(const std::string& label, int link,
                     double length = 1.0) noexcept;

        /**
         * @param[in] img Image of the key.
         * @param[in] link Id of the main panel to display when clicking on this
         * key.
         * The id depends on the order of registration of the main panels. It
         * starts from 0.
         * @param[in] length Length of the key.
         */
        explicit Key(const Image& img, int link, double length = 1.0) noexcept;

        /**
         * @param[in] unicode UTF32 code point for the key.
         * @param[in] keys_multichoice Multichoice panel that has to be displayed
         * after a long touch event.
         * @param[in] length Length of the key.
         */
        explicit Key(uint32_t unicode,
                     panel_keys keys_multichoice,
                     double length = 1.0) noexcept;

        /**
         * Set the key color.
         */
        void set_color(Palette::ColorId id, const Palette::pattern_type& color,
                       Palette::GroupId group = Palette::GroupId::normal);

        /*
         * Set the key font.
         */
        void set_font(const Font& font);

    protected:
        /**
         * Button associated with the key.
         */
        std::shared_ptr<Button> m_button {nullptr};

        /**
         * Code of the key.
         */
        KeyboardCode m_keycode {EKEY_UNKNOWN};

        /**
         * UTF32 code point for the key.
         */
        uint32_t m_unicode {0};

        /**
         * Length multiplicator of the panel default key size.
         */
        double m_length;

        /**
         * Id of the MainPanel to display when clicking on this button.
         */
        int m_link{-1};

        /**
         * Keys for the multichoice panel.
         */
        panel_keys m_keys_multichoice;

        /**
         * Multichoice panel to display on long press event
         */
        std::shared_ptr<Panel> m_multichoice_panel {nullptr};

        friend class VirtualKeyboard;
    };

    /**
     * A default keyboard with a Qwerty layout.
     *
     * @param[in] rect Rect of the Keyboard.
     */
    explicit VirtualKeyboard(const Rect& rect = {}) noexcept;

    /**
     * A keyboard with a custom layout.
     *
     * @param[in] keys Main panels of the keyboard.
     * @param[in] rect Rect of the Keyboard.
     */
    explicit VirtualKeyboard(std::vector<panel_keys> keys,
                             const Rect& rect = {});

    virtual void resize(const Size& s) override;

    /**
     * Set the space between keys.
     */
    void set_key_space(unsigned key_space);

protected:
    /**
     * Internal representation of a panel i.e. a set of keys organized by rows.
     */
    struct Panel : public VerticalBoxSizer
    {
        // cppcheck-suppress noExplicitConstructor
        Panel(panel_keys keys);
        void update_key_space(unsigned key_space);
        void update_key_size(const Size& s);

        panel_keys m_keys;
    };

    /**
     * VirtualKeyboard input dispatcher.
     */
    struct KeyboardInput : public Input
    {
        using Input::Input;
        friend class VirtualKeyboard;
    };

    /**
     * Configure the key to select the appropriate panel on a click event.
     */
    void set_key_link(const std::shared_ptr<Key>& k);

    /**
     * Configure the Event data to be dispatched on a click event.
     */
    void set_key_input_value(const std::shared_ptr<Key>& k);

    /**
     * Create the multichoice panel and configure the keys.
     */
    void set_key_multichoice(const std::shared_ptr<Key>& k);

    /**
     * The popup that displays the multichoice panel.
     */
    std::shared_ptr<Popup> m_multichoice_popup {nullptr};

    /**
     * The main panels of the keyboard i.e. not the multichoice ones.
     */
    std::vector<std::shared_ptr<Panel>> m_main_panels;

    /**
     * A Notebook containing the main panels of the keyboard.
     */
    Notebook m_main_panel;

    /**
     * The input dispatcher.
     */
    KeyboardInput m_in;

    /**
     * The maximum number of rows among the main panels.
     */
    unsigned m_max_rows {0};

    /**
     * The maximum number of columns among the main panels.
     */
    unsigned m_max_cols {0};

    /**
     * The key space.
     */
    unsigned m_key_space {2};

    /**
     * The size factor for keys in the multichoice panel.
     */
    double m_key_size_multichoice_factor {1.2};
};

/**
 * PopupVirtualKeyboard class.
 *
 * A virtual keyboard displayed automatically when a click happens in a textbox.
 * Two buttons are added to the virtual keyboard in order to close it or to
 * move it from the bottom to the top of the screen or the other way around.
 */
class EGT_API PopupVirtualKeyboard : public Popup
{
public:
    /**
     * @param[in] keyboard The virtual keyboard to display.
     */
    explicit PopupVirtualKeyboard(std::shared_ptr<VirtualKeyboard> keyboard) noexcept;

protected:
    /**
     * A vertical sizer for the keyboard and the two additionnal buttons.
     */
    VerticalBoxSizer m_vsizer;

    /**
     * A horizontal sizer for the two additionnal buttons.
     */
    HorizontalBoxSizer m_hsizer;

    /**
     * A button to close the virtual keyboard.
     */
    ImageButton m_close_button {Image("@cancel.png")};

    /**
     * A button to move the virtual keyboard to the bottom or the top of the
     * screen.
     */
    ImageButton m_top_bottom_button {Image("@arrow_up.png")};

    /**
     * Store the position of the virtual keyboard on the screen.
     */
    bool m_bottom_positionned {true};
};

/**
 * Get a pointer to the popup virtual keyboard.
 */
PopupVirtualKeyboard*& popup_virtual_keyboard();

VirtualKeyboard::panel_keys& multichoice_e();
VirtualKeyboard::panel_keys& multichoice_E();
VirtualKeyboard::panel_keys& multichoice_r();
VirtualKeyboard::panel_keys& multichoice_R();
VirtualKeyboard::panel_keys& multichoice_t();
VirtualKeyboard::panel_keys& multichoice_T();
VirtualKeyboard::panel_keys& multichoice_y();
VirtualKeyboard::panel_keys& multichoice_Y();
VirtualKeyboard::panel_keys& multichoice_u();
VirtualKeyboard::panel_keys& multichoice_U();
VirtualKeyboard::panel_keys& multichoice_i();
VirtualKeyboard::panel_keys& multichoice_I();
VirtualKeyboard::panel_keys& multichoice_o();
VirtualKeyboard::panel_keys& multichoice_O();
VirtualKeyboard::panel_keys& multichoice_a();
VirtualKeyboard::panel_keys& multichoice_A();
VirtualKeyboard::panel_keys& multichoice_s();
VirtualKeyboard::panel_keys& multichoice_S();
VirtualKeyboard::panel_keys& multichoice_d();
VirtualKeyboard::panel_keys& multichoice_D();
VirtualKeyboard::panel_keys& multichoice_g();
VirtualKeyboard::panel_keys& multichoice_G();
VirtualKeyboard::panel_keys& multichoice_k();
VirtualKeyboard::panel_keys& multichoice_K();
VirtualKeyboard::panel_keys& multichoice_l();
VirtualKeyboard::panel_keys& multichoice_L();
VirtualKeyboard::panel_keys& multichoice_z();
VirtualKeyboard::panel_keys& multichoice_Z();
VirtualKeyboard::panel_keys& multichoice_c();
VirtualKeyboard::panel_keys& multichoice_C();
VirtualKeyboard::panel_keys& multichoice_n();
VirtualKeyboard::panel_keys& multichoice_N();
VirtualKeyboard::panel_keys& QwertyLettersLowerCase();
VirtualKeyboard::panel_keys& QwertyLettersUpperCase();
VirtualKeyboard::panel_keys& QwertySymbols1();
VirtualKeyboard::panel_keys& QwertySymbols2();

}
}

#endif
