/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <chrono>
#include <cxxopts.hpp>
#include <egt/detail/string.h>
#include <egt/ui>
#include <sstream>
#include <string>

// warning: not UTF-8 safe
static std::string line_break(const std::string& in, size_t width = 50)
{
    std::string out;
    std::string tmp;
    char last = '\0';
    size_t i = 0;

    for (auto& cur : in)
    {
        if (++i == width)
        {
            tmp = egt::detail::ltrim(tmp);
            out += "\n" + tmp;
            i = tmp.length();
            tmp.clear();
        }
        else if (std::isspace(cur) && !std::isspace(last))
        {
            out += tmp;
            tmp.clear();
        }
        tmp += cur;
        last = cur;
    }
    return out + tmp;
}

static void scale_camera_window(egt::ImageButton& fullscreen, egt::CameraWindow& player, const egt::Size& ssize)
{
    static bool scaled = true;
    if (scaled)
    {
        const auto wscale = static_cast<float>(ssize.width()) / player.width();
        const auto hscale = static_cast<float>(ssize.height()) / player.height();
        player.move(egt::Point(0, 0));
        player.scale(wscale, hscale);
        fullscreen.image(egt::Image("res:fullscreen_exit_png"));
        scaled = false;
    }
    else
    {
        player.scale(1.0, 1.0);
        player.move_to_center();
        fullscreen.image(egt::Image("res:fullscreen_png"));
        scaled = true;
    }
}

int main(int argc, char** argv)
{
    cxxopts::Options options(argv[0], "display camera video stream");
    options.add_options()
    ("h,help", "Show help")
    ("d,device", "V4L2 device", cxxopts::value<std::string>()->default_value("/dev/video0"))
    ("width", "Width of the stream", cxxopts::value<int>()->default_value("320"))
    ("height", "Height of the stream", cxxopts::value<int>()->default_value("240"))
    ("f,format", "Pixel format", cxxopts::value<std::string>()->default_value("yuyv"), "[egt::PixelFormat]");
    auto args = options.parse(argc, argv);

    if (args.count("help"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    egt::Application app(argc, argv);
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    egt::Size size(args["width"].as<int>(), args["height"].as<int>());
    auto format = egt::detail::enum_from_string<egt::PixelFormat>(args["format"].as<std::string>());
    auto dev(args["device"].as<std::string>());

    egt::TopWindow win;
    win.background(egt::Image("file:background.jpg"));

    egt::CameraWindow player(size, dev, format, egt::WindowHint::overlay);
    player.move_to_center(win.center());
    win.add(player);

    egt::Label errlabel;
    errlabel.align(egt::AlignFlag::expand_horizontal);
    errlabel.text_align(egt::AlignFlag::center | egt::AlignFlag::top);
    win.add(errlabel);

    // wait to start playing the video until the window is shown
    win.on_show([&player]()
    {
        player.start();
    });

    player.on_error([&errlabel](const std::string & err)
    {
        errlabel.text(line_break(err));
    });

    player.on_connect([&player, &errlabel, dev](const std::string&)
    {
        if (!errlabel.text().empty())
        {
            errlabel.text("");
            player.start();
        }
    });

    player.on_disconnect([&player, &errlabel, dev](const std::string & devnode)
    {
        errlabel.text(line_break("Device removed: " + devnode));
        player.stop();
    });

    egt::Window ctrlwindow(egt::Size(win.width(), 72), egt::PixelFormat::argb8888);
    ctrlwindow.align(egt::AlignFlag::bottom | egt::AlignFlag::center);
    ctrlwindow.color(egt::Palette::ColorId::bg, egt::Palette::transparent);
    if (!ctrlwindow.plane_window())
        ctrlwindow.fill_flags(egt::Theme::FillFlag::blend);
    win.add(ctrlwindow);

    egt::HorizontalBoxSizer hpos;
    hpos.align(egt::AlignFlag::center);
    ctrlwindow.add(hpos);

    auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_icon.png;32"));
    logo->margin(10);
    hpos.add(logo);

    egt::ImageButton fullscreen(egt::Image("res:fullscreen_png"));
    fullscreen.fill_flags().clear();
    hpos.add(fullscreen);

    const auto ssize = egt::Application::instance().screen()->size();

    fullscreen.on_click([&fullscreen, &player, &ssize](egt::Event&)
    {
        scale_camera_window(fullscreen, player, ssize);
    });

    player.on_click([&fullscreen, &player, &ssize]()
    {
        scale_camera_window(fullscreen, player, ssize);
    });

    egt::Label cpulabel("CPU: 0%", egt::Size(100, 40));
    cpulabel.margin(5);
    hpos.add(cpulabel);

    egt::experimental::CPUMonitorUsage tools;
    egt::PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&cpulabel, &tools]()
    {
        tools.update();
        std::ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage()) << "%";
        cpulabel.text(ss.str());
    });
    cputimer.start();

    ctrlwindow.show();
    player.show();
    win.show();

    return app.run();
}
