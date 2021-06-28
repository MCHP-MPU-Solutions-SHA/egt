@page fonts Fonts

[TOC]

This chapter discusses how to to use fonts.

@section fonts_font Font Class

The interface for working with fonts in EGT is the egt::Font class. This class
handles properties related to working with fonts, but there are still some other
libraries in play.  Underneath, the egt::Font class uses Fontconfig and Freetype
for finding and rendering fonts.

Creating and assigning an egt::Font to a specific widget involves using the
egt::v1::Widget::font() member function.

@code{.cpp}
egt::Button btn("Press Me");
auto customfont = egt::Font("Serif", 25, egt::Font::Weight::bold, egt::Font::Slant::italic);
btn.font(customfont);
@endcode

@section fonts_fontconfig Fontconfig

[Fontconfig](https://www.freedesktop.org/wiki/Software/fontconfig/) is a library
for configuring and customizing font access.  This is the library EGT uses to
discover fonts installed on the system automatically.

Selecting fonts from within EGT ultimately uses Fontconfig.  The
[Fontconfig](https://www.freedesktop.org/software/fontconfig/fontconfig-user.html)
documentation</a> should be the ultimate source of how to make the best use of
and how to configure Fontconfig. However, to help introduce how to debug using
Fontconfig, several things are worth noting.

To see what fonts will be returned given a specific name, you can use the
`fc-match` utility, not to be confused with the `fc-query` utility.

@code{.sh}
# fc-match -s 'Serif'
LiberationSerif-Regular.ttf: "Liberation Serif" "Regular"
LiberationSerif-Italic.ttf: "Liberation Serif" "Italic"
DejaVuSerif.ttf: "DejaVu Serif" "Book"
DejaVuSerif-Bold.ttf: "DejaVu Serif" "Bold"
DejaVuSerif-Italic.ttf: "DejaVu Serif" "Italic"
DejaVuSerif-BoldItalic.ttf: "DejaVu Serif" "Bold Italic"
DejaVuSans.ttf: "DejaVu Sans" "Book"
DejaVuSansMono.ttf: "DejaVu Sans Mono" "Book"
Roboto-Black.ttf: "Roboto" "Black"
DejaVuSans-Bold.ttf: "DejaVu Sans" "Bold"
DejaVuSansMono-Bold.ttf: "DejaVu Sans Mono" "Bold"
DejaVuSans-Oblique.ttf: "DejaVu Sans" "Oblique"
DejaVuSans-BoldOblique.ttf: "DejaVu Sans" "Bold Oblique"
@endcode

@code{.sh}
# fc-match -s 'DejaVu Sans'
DejaVuSans.ttf: "DejaVu Sans" "Book"
DejaVuSans-Bold.ttf: "DejaVu Sans" "Bold"
DejaVuSans-Oblique.ttf: "DejaVu Sans" "Oblique"
DejaVuSans-BoldOblique.ttf: "DejaVu Sans" "Bold Oblique"
LiberationSerif-Regular.ttf: "Liberation Serif" "Regular"
LiberationSerif-Italic.ttf: "Liberation Serif" "Italic"
DejaVuSansMono.ttf: "DejaVu Sans Mono" "Book"
DejaVuSerif.ttf: "DejaVu Serif" "Book"
Roboto-Black.ttf: "Roboto" "Black"
DejaVuSansMono-Bold.ttf: "DejaVu Sans Mono" "Bold"
DejaVuSerif-Bold.ttf: "DejaVu Serif" "Bold"
DejaVuSerif-Italic.ttf: "DejaVu Serif" "Italic"
DejaVuSerif-BoldItalic.ttf: "DejaVu Serif" "Bold Italic"
@endcode

To see what installed fonts contain a certain code point, you can use the
`fc-list` utility and specify one or more code points in hex.

@code{.sh}
fc-list ':charset=B6'
@endcode

When running an EGT application, or any application that uses Fontconfig, you
can define the environment variable `FC_DEBUG` that will cause Fontconfig to
issue some useful information.

@code{.sh}
FC_DEBUG=1 ./basic
@endcode

The value of the `FC_DEBUG` variable, depending on what you want printed, is
defined in the
[Fontconfig documentation](https://www.freedesktop.org/software/fontconfig/fontconfig-user.html#DEBUG).

@section fonts_installing Installing Fonts

Installing fonts is a system level operation outside of EGT itself.  In most
cases the font can be copied to /usr/share/fonts, `fc-cache` can be run, and it
will be automatically found by EGT.

A useful tool to load a font and inspect it can be found
[here](http://mathew-kurian.github.io/CharacterMap/).

@note This may not work with some fonts, such as as the Google Noto
Color Emoji font.

While there are hundreds if not thousands of fonts available from all different
sources under various licenses, both free and non-free, Google provides some
relatively [complete and free fonts called Google Noto Fonts](https://www.google.com/get/noto/).

> When text is rendered by a computer, sometimes characters are displayed as
> “tofu”. They are little boxes to indicate your device doesn’t have a font to
> display the text.
>
> Google has been developing a font family called Noto, which aims to support
> all languages with a harmonious look and feel. Noto is Google’s answer to
> tofu. The name noto is to convey the idea that Google’s goal is to see
> “no more tofu”. Noto has multiple styles and weights, and is freely available
> to all. The comprehensive set of fonts and tools used in our development is
> available in our GitHub repositories.

These Google Noto Fonts also include specific support for many languages
requiring Unicode support.

> Noto Sans CJK and Noto Serif CJK comprehensively cover Simplified Chinese,
> Traditional Chinese, Japanese, and Korean in a unified font family. This
> includes the full coverage of CJK Ideographs with variation support for 4
> regions, Kangxi radicals, Japanese Kana, Korean Hangul, and other CJK symbols
> and letters in the Basic Multilingual Plane of Unicode. It also provides
> limited coverage of CJK Ideographs in Plane 2 of Unicode as necessary to
> support standards from China and Japan.

@section fonts_freetype FreeType

FreeType is used by EGT to render any font that is available and installed in
the system.

[FreeType](https://www.freetype.org/) is a software font engine that
is designed to be small, efficient, highly customizable, and portable while
capable of producing high-quality output (glyph images). It can be used in
graphics libraries, display servers, font conversion tools, text image
generation tools, and many other products as well.

@section fonts_inter Internationalization

Both Fontconfig and FreeType support international fonts in many various
languages and EGT takes advantage of this and in turn provides complete
internationalization support for rendered text.
