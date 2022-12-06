/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>
#include <memory>

static constexpr float calculate(float start, float decrement, int count)
{
    for (int i = 0; i < count; ++i)
        start -= decrement;
    return start;
}

static constexpr double calculate(double start, double decrement, int count)
{
    for (int i = 0; i < count; ++i)
        start -= decrement;
    return start;
}

TEST(Math, CompareFloat)
{
    const auto total = 10000;
    auto count = 0;
    for (auto i = 0; i < total; ++i)
    {
        auto expected = (i / 10.0f);
        auto actual = calculate(9.0f + expected, 0.2f, 45);
        if (egt::detail::float_equal(actual, expected))
            ++count;
    }
    EXPECT_EQ(count, total);
}

TEST(Math, CompareDouble)
{
    const auto total = 10000;
    auto count = 0;
    for (auto i = 0; i < total; ++i)
    {
        auto expected = (i / 10.0);
        auto actual = calculate(9.0 + expected, 0.2, 45);
        if (egt::detail::float_equal(actual, expected))
            ++count;
    }
    EXPECT_EQ(count, total);
}

TEST(Color, Basic)
{
    egt::Color c1(0x11223344);
    EXPECT_EQ(c1.red(), 0x11U);
    EXPECT_EQ(c1.green(), 0x22U);
    EXPECT_EQ(c1.blue(), 0x33U);
    EXPECT_EQ(c1.alpha(), 0x44U);

    egt::Color c2(c1, 0xff);
    EXPECT_EQ(c2.red(), 0x11U);
    EXPECT_EQ(c2.green(), 0x22U);
    EXPECT_EQ(c2.blue(), 0x33U);
    EXPECT_EQ(c2.alpha(), 0xffU);

    egt::Color c3(11, 22, 33, 44);
    EXPECT_EQ(c3.red(), 11U);
    EXPECT_EQ(c3.green(), 22U);
    EXPECT_EQ(c3.blue(), 33U);
    EXPECT_EQ(c3.alpha(), 44U);

    egt::Color c4 = egt::Color::rgb(0x112233);
    EXPECT_EQ(c4.red(), 0x11U);
    EXPECT_EQ(c4.green(), 0x22U);
    EXPECT_EQ(c4.blue(), 0x33U);
    EXPECT_EQ(c4.alpha(), 0xffU);

    egt::Color c5 = egt::Color::rgbaf(0.0, 1.0, 0.5, 0.75);
    EXPECT_EQ(c5.red(), 0U);
    EXPECT_EQ(c5.green(), 255U);
    EXPECT_FLOAT_EQ(c5.blue(), 127);
    EXPECT_FLOAT_EQ(c5.alpha(), 191);

    egt::Color c6 = egt::Color::css("#112233");
    EXPECT_EQ(c6.red(), 0x11U);
    EXPECT_EQ(c6.green(), 0x22U);
    EXPECT_EQ(c6.blue(), 0x33U);
    EXPECT_EQ(c6.alpha(), 0xffU);

    egt::Color c7 = egt::Color::css("#aa22ff");
    EXPECT_EQ(c7.red(), 0xaaU);
    EXPECT_EQ(c7.green(), 0x22U);
    EXPECT_EQ(c7.blue(), 0xffU);
    EXPECT_EQ(c7.alpha(), 0xffU);

    egt::Color c8 = egt::Color::css("aa22ff");
    EXPECT_EQ(c8.red(), 0xaaU);
    EXPECT_EQ(c8.green(), 0x22U);
    EXPECT_EQ(c8.blue(), 0xffU);
    EXPECT_EQ(c8.alpha(), 0xffU);
}

TEST(TextBox, Basic)
{
    egt::Application app;

    std::string str1 = "hello world\nthis is EGT";
    std::string str2 = "howdy";

    egt::TextBox text1(str1);
    ASSERT_EQ(str1, text1.text());
    text1.text(str2);
    ASSERT_EQ(str2, text1.text());
    ASSERT_EQ(str2.size(), text1.len());
    text1.append(str1);
    ASSERT_EQ(str2 + str1, text1.text());
    text1.cursor_begin();
    text1.insert(str1);
    ASSERT_EQ(str1 + str2 + str1, text1.text());
    text1.cursor_end();
    text1.insert(str1);
    ASSERT_EQ(str1 + str2 + str1 + str1, text1.text());
    text1.selection_all();
    ASSERT_EQ(str1 + str2 + str1 + str1, text1.selected_text());
    text1.selection(str1.size(), 5);
    ASSERT_EQ(str2, text1.selected_text());
    text1.selection_clear();
    ASSERT_EQ("", text1.selected_text());
    text1.selection_all();
    text1.selection_delete();
    ASSERT_EQ("", text1.text());
}

TEST(TextBoxFixed, Basic)
{
    egt::Application app;

    std::string str1 = "hello world\nthis is EGT";
    std::string str2 = "howdy";

    egt::TextBox text1(str1, egt::Size(1000, 1000), {}, egt::TextBox::TextFlag::fit_to_width);
    ASSERT_EQ(str1, text1.text());
    text1.text(str2);
    ASSERT_EQ(str2, text1.text());
    ASSERT_EQ(str2.size(), text1.len());
    text1.append(str1);
    ASSERT_EQ(str2 + str1, text1.text());
    text1.cursor_begin();
    text1.insert(str1);
    ASSERT_EQ(str1 + str2 + str1, text1.text());
    text1.cursor_end();
    text1.insert(str1);
    ASSERT_EQ(str1 + str2 + str1 + str1, text1.text());
    text1.selection_all();
    ASSERT_EQ(str1 + str2 + str1 + str1, text1.selected_text());
    text1.selection(str1.size(), 5);
    ASSERT_EQ(str2, text1.selected_text());
    text1.selection_clear();
    ASSERT_EQ("", text1.selected_text());
    text1.selection_all();
    text1.selection_delete();
    ASSERT_EQ("", text1.text());
}

TEST(Screen, DamageAlgorithm)
{
    egt::Screen::DamageArray damage;
    egt::Screen::damage_algorithm(damage, egt::Rect(0, 0, 100, 100));
    EXPECT_EQ(damage.front(), egt::Rect(0, 0, 100, 100));
    egt::Screen::damage_algorithm(damage, egt::Rect(0, 0, 200, 200));
    EXPECT_EQ(damage.size(), 1U);
    EXPECT_EQ(damage.front(), egt::Rect(0, 0, 200, 200));
}

TEST(Canvas, Basic)
{
    egt::Canvas canvas1(egt::Size(100, 100));
    EXPECT_EQ(canvas1.size(), egt::Size(100, 100));
    EXPECT_EQ(canvas1.format(), egt::PixelFormat::argb8888);
    egt::Canvas canvas2(canvas1.surface());
    EXPECT_EQ(canvas2.size(), egt::Size(100, 100));
    EXPECT_EQ(canvas2.format(), egt::PixelFormat::argb8888);

    egt::Canvas canvas3(egt::Size(100, 100), egt::PixelFormat::rgb565);
    EXPECT_EQ(canvas3.size(), egt::Size(100, 100));
    EXPECT_EQ(canvas3.format(), egt::PixelFormat::rgb565);

    egt::Canvas canvas4(canvas3.surface());
    EXPECT_EQ(canvas4.size(), egt::Size(100, 100));
    EXPECT_EQ(canvas4.format(), egt::PixelFormat::rgb565);
}

TEST(Geometry, Basic)
{
    egt::Point p1(3, 4);
    EXPECT_EQ(p1.x(), 3);
    EXPECT_EQ(p1.y(), 4);
    p1 += 1;
    EXPECT_EQ(p1.x(), 4);
    EXPECT_EQ(p1.y(), 5);

    egt::Size s1(3, 4);
    EXPECT_EQ(s1.width(), 3);
    EXPECT_EQ(s1.height(), 4);
    s1 += 1;
    EXPECT_EQ(s1.width(), 4);
    EXPECT_EQ(s1.height(), 5);

    egt::Rect r1(3, 4, 5, 6);
    EXPECT_EQ(r1.x(), 3);
    EXPECT_EQ(r1.y(), 4);
    EXPECT_EQ(r1.width(), 5);
    EXPECT_EQ(r1.height(), 6);
}

TEST(Geometry, Points)
{
    egt::Point p1;

    egt::Point p2(10, 0);
    EXPECT_EQ(p1.distance_to(p2), 10);
}

TEST(Geometry, Rects)
{
    egt::Rect r1;
    EXPECT_TRUE(r1.empty());
}

TEST(Geometry, Sizes)
{
    egt::Size s1;
    EXPECT_TRUE(s1.empty());
}

TEST(Geometry, Arcs)
{
    egt::Arc a1;
    EXPECT_TRUE(a1.empty());
}

TEST(Geometry, Circless)
{
    egt::Circle c1;
    EXPECT_TRUE(c1.empty());
}

template <class T>
class Widgets : public testing::Test
{
protected:
    Widgets() {}
    ~Widgets() override {}
    egt::Application app;
};

using WidgetTypes = ::testing::Types<egt::AnalogMeter,
      egt::Button,
      egt::ImageButton,
      egt::CheckBox,
      egt::CircleWidget,
      egt::Label,
      egt::ImageLabel,
      egt::ListBox,
      egt::RadioBox,
      egt::TextBox,
      egt::ComboBox,
      egt::Slider,
      egt::BoxSizer,
      egt::VerticalBoxSizer,
      egt::HorizontalBoxSizer,
      egt::Notebook,
      egt::StaticGrid,
      egt::Scrollwheel,
      egt::ProgressBar,
      egt::SpinProgress,
      egt::LevelMeter,
      egt::AnalogMeter,
      egt::CircleWidget,
      egt::LineWidget,
      egt::RectangleWidget>;
TYPED_TEST_SUITE(Widgets, WidgetTypes);
TYPED_TEST(Widgets, Properties)
{
    auto widget = std::make_unique<TypeParam>();

    widget->resize(egt::Size(100, 100));
    EXPECT_EQ(widget->size(), egt::Size(100, 100));
    widget->move(egt::Point(100, 100));
    EXPECT_EQ(widget->point(), egt::Point(100, 100));
    widget->width(10);
    widget->height(10);
    EXPECT_EQ(widget->size(), egt::Size(10, 10));
    widget->x(10);
    widget->y(10);
    EXPECT_EQ(widget->point(), egt::Point(10, 10));
    widget->move_to_center(egt::Point(10, 10));
    EXPECT_EQ(widget->box(), egt::Rect(5, 5, 10, 10));
    EXPECT_EQ(widget->center(), egt::Point(10, 10));
    widget->box(egt::Rect(50, 51, 15, 16));
    EXPECT_EQ(widget->box(), egt::Rect(50, 51, 15, 16));
    EXPECT_EQ(widget->width(), 15);
    EXPECT_EQ(widget->height(), 16);
    EXPECT_EQ(widget->x(), 50);
    EXPECT_EQ(widget->y(), 51);
    EXPECT_EQ(widget->visible(), true);
    widget->hide();
    EXPECT_EQ(widget->visible(), false);
    widget->show();
    EXPECT_EQ(widget->visible(), true);
    widget->visible_toggle();
    EXPECT_EQ(widget->visible(), false);
    widget->visible_toggle();
    EXPECT_EQ(widget->visible(), true);
    EXPECT_EQ(widget->readonly(), false);
    widget->readonly(true);
    EXPECT_EQ(widget->readonly(), true);
    widget->readonly(false);
    EXPECT_EQ(widget->readonly(), false);
    widget->align(egt::AlignFlag::right | egt::AlignFlag::center_vertical);
    EXPECT_EQ(widget->align(), egt::AlignFlag::right | egt::AlignFlag::center_vertical);
    widget->padding(10);
    widget->border(11);
    widget->margin(12);
    EXPECT_EQ(widget->moat(), 33U);
}

TEST(WidgetFlags, Basic)
{
    egt::Widget::Flags flags1("window|readonly");
    EXPECT_TRUE(flags1.is_set(egt::Widget::Flag::window));
    EXPECT_TRUE(flags1.is_set(egt::Widget::Flag::readonly));
    EXPECT_FALSE(flags1.is_set(egt::Widget::Flag::plane_window));

    flags1.clear(egt::Widget::Flag::readonly);
    EXPECT_TRUE(flags1.is_set(egt::Widget::Flag::window));
    EXPECT_FALSE(flags1.is_set(egt::Widget::Flag::readonly));
    EXPECT_FALSE(flags1.is_set(egt::Widget::Flag::plane_window));

    flags1.set(egt::Widget::Flag::plane_window);
    EXPECT_TRUE(flags1.is_set(egt::Widget::Flag::window));
    EXPECT_FALSE(flags1.is_set(egt::Widget::Flag::readonly));
    EXPECT_TRUE(flags1.is_set(egt::Widget::Flag::plane_window));

    flags1.clear();
    EXPECT_FALSE(flags1.is_set(egt::Widget::Flag::window));
    EXPECT_FALSE(flags1.is_set(egt::Widget::Flag::readonly));
    EXPECT_FALSE(flags1.is_set(egt::Widget::Flag::plane_window));

    flags1.set({egt::Widget::Flag::window, egt::Widget::Flag::readonly});
    EXPECT_TRUE(flags1.is_set(egt::Widget::Flag::window));
    EXPECT_TRUE(flags1.is_set(egt::Widget::Flag::readonly));
    EXPECT_FALSE(flags1.is_set(egt::Widget::Flag::plane_window));
    EXPECT_EQ(flags1.to_string(), "window|readonly");
}

TEST(AlignFlags, Basic)
{
    bool state = false;
    egt::AlignFlags flags1("left|right");
    flags1.on_change([&state]() { state = true; });
    EXPECT_TRUE(flags1.is_set(egt::AlignFlag::left));
    EXPECT_TRUE(flags1.is_set(egt::AlignFlag::right));
    EXPECT_FALSE(flags1.is_set(egt::AlignFlag::bottom));

    flags1.clear(egt::AlignFlag::right);
    EXPECT_TRUE(state); state = false;
    EXPECT_TRUE(flags1.is_set(egt::AlignFlag::left));
    EXPECT_FALSE(flags1.is_set(egt::AlignFlag::right));
    EXPECT_FALSE(flags1.is_set(egt::AlignFlag::bottom));

    flags1.set(egt::AlignFlag::bottom);
    EXPECT_TRUE(state); state = false;
    flags1.set(egt::AlignFlag::bottom);
    EXPECT_FALSE(state);
    EXPECT_TRUE(flags1.is_set(egt::AlignFlag::left));
    EXPECT_FALSE(flags1.is_set(egt::AlignFlag::right));
    EXPECT_TRUE(flags1.is_set(egt::AlignFlag::bottom));

    flags1.clear();
    EXPECT_TRUE(state); state = false;
    EXPECT_FALSE(flags1.is_set(egt::AlignFlag::left));
    EXPECT_FALSE(flags1.is_set(egt::AlignFlag::right));
    EXPECT_FALSE(flags1.is_set(egt::AlignFlag::bottom));

    flags1.set({egt::AlignFlag::left, egt::AlignFlag::right});
    EXPECT_TRUE(state); state = false;
    flags1.set({egt::AlignFlag::left, egt::AlignFlag::right});
    EXPECT_FALSE(state);
    EXPECT_TRUE(flags1.is_set(egt::AlignFlag::left));
    EXPECT_TRUE(flags1.is_set(egt::AlignFlag::right));
    EXPECT_FALSE(flags1.is_set(egt::AlignFlag::bottom));
    EXPECT_EQ(flags1.to_string(), "left|right");

    egt::Button b;
    b.align().on_change([&state]() { state = true; });
    b.align(flags1);
    EXPECT_TRUE(state); state = false;
    EXPECT_TRUE(b.align().is_set(egt::AlignFlag::left));
    EXPECT_TRUE(b.align().is_set(egt::AlignFlag::right));
    EXPECT_FALSE(b.align().is_set(egt::AlignFlag::bottom));

    b.align(b.align() | egt::AlignFlag::bottom);
    EXPECT_TRUE(state); state = false;
    EXPECT_TRUE(b.align().is_set(egt::AlignFlag::left));
    EXPECT_TRUE(b.align().is_set(egt::AlignFlag::right));
    EXPECT_TRUE(b.align().is_set(egt::AlignFlag::bottom));
}

TEST(Network, URI)
{
    egt::Uri uri1("http://www.example.com/path/?key=value#frag");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "http");
    EXPECT_EQ(uri1.host(), "www.example.com");
    EXPECT_EQ(uri1.path(), "/path/");
    EXPECT_EQ(uri1.query(), "key=value");
    EXPECT_EQ(uri1.fragment(), "frag");

    uri1.set("res:name");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "res");
    EXPECT_EQ(uri1.host(), "");
    EXPECT_EQ(uri1.path(), "name");
    EXPECT_EQ(uri1.query(), "");
    EXPECT_EQ(uri1.fragment(), "");

    uri1.set("file:///absolute/path");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "file");
    EXPECT_EQ(uri1.host(), "");
    EXPECT_EQ(uri1.path(), "/absolute/path");
    EXPECT_EQ(uri1.query(), "");
    EXPECT_EQ(uri1.fragment(), "");

    uri1.set("file:relative/path");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "file");
    EXPECT_EQ(uri1.host(), "");
    EXPECT_EQ(uri1.path(), "relative/path");
    EXPECT_EQ(uri1.query(), "");
    EXPECT_EQ(uri1.fragment(), "");

    uri1.set("file://relative/path");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "file");
    EXPECT_EQ(uri1.host(), "");
    EXPECT_EQ(uri1.path(), "relative/path");
    EXPECT_EQ(uri1.query(), "");
    EXPECT_EQ(uri1.fragment(), "");

    uri1.set("file://relative");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "file");
    EXPECT_EQ(uri1.host(), "");
    EXPECT_EQ(uri1.path(), "relative");
    EXPECT_EQ(uri1.query(), "");
    EXPECT_EQ(uri1.fragment(), "");

    uri1.set("icon:relative/path");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "icon");
    EXPECT_EQ(uri1.host(), "");
    EXPECT_EQ(uri1.path(), "relative/path");
    EXPECT_EQ(uri1.query(), "");
    EXPECT_EQ(uri1.fragment(), "");

    uri1.set("icon:/absolute/path");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "icon");
    EXPECT_EQ(uri1.host(), "");
    EXPECT_EQ(uri1.path(), "/absolute/path");
    EXPECT_EQ(uri1.query(), "");
    EXPECT_EQ(uri1.fragment(), "");

    uri1.set("icon:/absolute/path;32");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "icon");
    EXPECT_EQ(uri1.host(), "");
    EXPECT_EQ(uri1.path(), "/absolute/path");
    EXPECT_EQ(uri1.query(), "");
    EXPECT_EQ(uri1.fragment(), "");
    EXPECT_EQ(uri1.icon_size(), "32");

    uri1.set("icon:/absolute/path;SMALL");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "icon");
    EXPECT_EQ(uri1.host(), "");
    EXPECT_EQ(uri1.path(), "/absolute/path");
    EXPECT_EQ(uri1.query(), "");
    EXPECT_EQ(uri1.fragment(), "");
    EXPECT_EQ(uri1.icon_size(), "16");
}

class Align : public ::testing::Test
{
protected:
    using WidgetType = egt::Frame;

    void SetUp() override
    {
        top.show();
        window.show();
    }

    // void TearDown() override {}

    egt::Application app;
    egt::TopWindow top{};
    egt::Window window{top, egt::Size(400, 400)};
};

TEST_F(Align, TopLeft)
{
    WidgetType widget(window, egt::Size(100, 100));
    widget.align(egt::AlignFlag::top | egt::AlignFlag::left);
    EXPECT_EQ(widget.box().top_left(), egt::Point(0, 0));
    EXPECT_EQ(widget.box().size(), egt::Size(100, 100));
}

TEST_F(Align, TopCenter)
{
    WidgetType widget(window, egt::Size(100, 100));
    widget.align(egt::AlignFlag::top | egt::AlignFlag::center_horizontal);
    EXPECT_EQ(widget.box().top_left(), egt::Point(400 / 2 - 100 / 2, 0));
    EXPECT_EQ(widget.box().size(), egt::Size(100, 100));
}

TEST_F(Align, TopRight)
{
    WidgetType widget(window, egt::Size(100, 100));
    widget.align(egt::AlignFlag::top | egt::AlignFlag::right);
    EXPECT_EQ(widget.box().top_left(), egt::Point(400 - 100, 0));
    EXPECT_EQ(widget.box().size(), egt::Size(100, 100));
}

TEST_F(Align, RightCenter)
{
    WidgetType widget(window, egt::Size(100, 100));
    widget.align(egt::AlignFlag::right | egt::AlignFlag::center_vertical);
    EXPECT_EQ(widget.box().top_left(), egt::Point(400 - 100, 400 / 2 - 100 / 2));
    EXPECT_EQ(widget.box().size(), egt::Size(100, 100));
}

TEST_F(Align, BottomRight)
{
    WidgetType widget(window, egt::Size(100, 100));
    widget.align(egt::AlignFlag::bottom | egt::AlignFlag::right);
    EXPECT_EQ(widget.box().top_left(), egt::Point(400 - 100, 400 - 100));
    EXPECT_EQ(widget.box().size(), egt::Size(100, 100));
}

TEST_F(Align, BottomCenter)
{
    WidgetType widget(window, egt::Size(100, 100));
    widget.align(egt::AlignFlag::bottom | egt::AlignFlag::center_horizontal);
    EXPECT_EQ(widget.box().top_left(), egt::Point(400 / 2 - 100 / 2, 400 - 100));
    EXPECT_EQ(widget.box().size(), egt::Size(100, 100));
}

TEST_F(Align, BottomLeft)
{
    WidgetType widget(window, egt::Size(100, 100));
    widget.align(egt::AlignFlag::bottom | egt::AlignFlag::left);
    EXPECT_EQ(widget.box().top_left(), egt::Point(0, 400 - 100));
    EXPECT_EQ(widget.box().size(), egt::Size(100, 100));
}

TEST_F(Align, LeftCenter)
{
    WidgetType widget(window, egt::Size(100, 100));
    widget.align(egt::AlignFlag::center_vertical | egt::AlignFlag::left);
    EXPECT_EQ(widget.box().top_left(), egt::Point(0, 400 / 2 - 100 / 2));
    EXPECT_EQ(widget.box().size(), egt::Size(100, 100));
}

TEST_F(Align, Center)
{
    WidgetType widget(window, egt::Size(100, 100));
    widget.align(egt::AlignFlag::center);
    EXPECT_EQ(widget.box().top_left(), egt::Point(400 / 2 - 100 / 2, 400 / 2 - 100 / 2));
    EXPECT_EQ(widget.box().size(), egt::Size(100, 100));
}

TEST_F(Align, ExpandVertical)
{
    WidgetType widget(window, egt::Size(100, 100));
    widget.align(egt::AlignFlag::center | egt::AlignFlag::expand_vertical);
    EXPECT_EQ(widget.box().top_left(), egt::Point(400 / 2 - 100 / 2, 0));
    EXPECT_EQ(widget.box().size(), egt::Size(100, 400));
}

TEST_F(Align, ExpandHorizontal)
{
    WidgetType widget(window, egt::Size(100, 100));
    widget.align(egt::AlignFlag::center | egt::AlignFlag::expand_horizontal);
    EXPECT_EQ(widget.box().top_left(), egt::Point(0, 400 / 2 - 100 / 2));
    EXPECT_EQ(widget.box().size(), egt::Size(400, 100));
}

TEST_F(Align, Expand)
{
    WidgetType widget(window, egt::Size(100, 100));
    widget.align(egt::AlignFlag::expand);
    EXPECT_EQ(widget.box().top_left(), egt::Point(0, 0));
    EXPECT_EQ(widget.box().size(), egt::Size(400, 400));
}

class SizerLayout : public ::testing::Test
{
protected:

    void SetUp() override
    {
        top.show();
        window.show();
    }

    // void TearDown() override {}

    egt::Application app;
    egt::TopWindow top{};
    egt::Window window{top, egt::Size(400, 400)};
};

TEST_F(SizerLayout, Default)
{
    egt::BoxSizer vsizer(window, egt::Orientation::vertical);

    egt::Frame widget(vsizer, egt::Size(100, 100));
    EXPECT_EQ(widget.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(widget.box().size(), egt::Size(100, 100));
}

TEST_F(SizerLayout, SizerCenter)
{
    egt::BoxSizer vsizer(window, egt::Orientation::vertical);
    vsizer.align(egt::AlignFlag::center);

    egt::Frame widget(vsizer, egt::Size(100, 100));
    EXPECT_EQ(widget.display_origin(), egt::DisplayPoint(400 / 2 - 100 / 2, 400 / 2 - 100 / 2));
    EXPECT_EQ(widget.box().size(), egt::Size(100, 100));
}

TEST_F(SizerLayout, ChildCenter)
{
    egt::BoxSizer vsizer(window, egt::Orientation::vertical);
    vsizer.align(egt::AlignFlag::expand);

    egt::Frame widget(vsizer, egt::Size(100, 100));
    EXPECT_EQ(widget.display_origin(), egt::DisplayPoint(400 / 2 - 100 / 2, 400 / 2 - 100 / 2));
    EXPECT_EQ(widget.box().size(), egt::Size(100, 100));
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
