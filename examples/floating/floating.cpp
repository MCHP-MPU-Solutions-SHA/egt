/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cmath>
#include <egt/ui>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

class MyWindow : public Window
{
public:
    MyWindow()
        : Window(Size(), widgetmask::NO_BACKGROUND),
          m_img("background.png")
    {
        add(&m_img);
        if (m_img.w() != w())
        {
            double scale = (double)w() / (double)m_img.w();
            m_img.scale(scale, scale);
        }
    }

    Image m_img;
};

class FloatingBox
{
public:
    FloatingBox(Widget* widget, int mx, int my)
        : m_widget(widget),
          m_mx(mx),
          m_my(my)
    {}

    virtual void next_frame()
    {
        int x = m_widget->x() + m_mx;
        int y = m_widget->y() + m_my;

        if (x + m_widget->w() >= main_window()->size().w)
            m_mx *= -1;

        if (x < 0)
            m_mx *= -1;

        if (y + m_widget->h() >= main_window()->size().h)
            m_my *= -1;

        if (y < 0)
            m_my *= -1;

        m_widget->move(Point(x, y));
    }

protected:
    Widget* m_widget;
    int m_mx;
    int m_my;
};

static vector<FloatingBox*> boxes;

int main()
{
    Application app;

    set_image_path("../share/egt/examples/floating/");

    MyWindow win;
    win.show();

    int f = 2;

    vector<std::pair<int, int>> moveparms =
    {
        std::make_pair(1 * f, 2 * f),
        std::make_pair(3 * f, -2 * f),
        std::make_pair(-3 * f, 2 * f),
        std::make_pair(-3 * f, 3 * f),
        std::make_pair(2 * f, 3 * f),
        std::make_pair(2 * f, 2 * f),
        std::make_pair(4 * f, 2 * f),
        std::make_pair(-4 * f, 2 * f),
    };

    uint32_t SOFT_COUNT = 2;

    // software
    for (uint32_t x = 0; x < SOFT_COUNT; x++)
    {
        stringstream os;
        os << "_image" << x << ".png";
        Image* image = new Image(os.str(), Point(100, 100));
        boxes.push_back(new FloatingBox(image, moveparms[x].first, moveparms[x].second));
        win.add(image);
    }

#ifdef HAVE_LIBPLANES
    int total = KMSScreen::instance()->count_planes(DRM_PLANE_TYPE_OVERLAY);
#endif

#ifdef HAVE_LIBPLANES
    // hardware (or emulated)
    for (uint32_t x = SOFT_COUNT; x < SOFT_COUNT + total; x++)
    {
        stringstream os;
        os << "_image" << x << ".png";
        Image* image = new Image(os.str());
        PlaneWindow* plane = new PlaneWindow(Size(image->w(), image->h()));
        plane->palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        plane->flag_set(widgetmask::NO_BACKGROUND);
        plane->add(image);
        plane->show();
        plane->move(Point(100, 100));
        boxes.push_back(new FloatingBox(plane, moveparms[x].first, moveparms[x].second));
    }
#endif

    PeriodicTimer movetimer(std::chrono::milliseconds(30));
    movetimer.on_timeout([&]()
    {
        for (auto i : boxes)
            i->next_frame();
    });
    movetimer.start();

    Label label1("CPU: -",
                 Rect(Point(10, win.size().h - 40),
                      Size(100, 40)),
                 alignmask::LEFT | alignmask::CENTER);
    label1.palette()
    .set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE)
    .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);

    win.add(&label1);

    CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&tools, &label1]()
    {
        tools.update();

        ostringstream ss;
        ss << "CPU: " << (int)tools.usage(0) << "%";
        label1.set_text(ss.str());
    });
    cputimer.start();

    return app.run();
}
