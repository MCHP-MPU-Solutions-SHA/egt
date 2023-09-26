/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "egt/detail/layout.h"
#include "egt/frame.h"
#include "egt/sizer.h"

#define LAY_IMPLEMENTATION
#include "layout.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

static void run_and_apply(lay_context& ctx, lay_id parent,
                          std::vector<LayoutRect>& children)
{
    for (const auto& child : children)
    {
        lay_id c = lay_item(&ctx);
        lay_set_size_xy(&ctx, c, child.rect.width(), child.rect.height());
        lay_set_margins_ltrb(&ctx, c, child.lmargin, child.tmargin, child.rmargin, child.bmargin);
        lay_set_behave(&ctx, c, child.behave);
        lay_insert(&ctx, parent, c);
    }

    lay_run_context(&ctx);

    auto re = children.begin();
    auto child = lay_first_child(&ctx, parent);
    while (child != LAY_INVALID_ID)
    {
        lay_vec4 r = lay_get_rect(&ctx, child);
        if (r[0] < 0 || r[1] < 0 || r[2] < 0 || r[3] < 0)
            detail::debug("The layout library returned a negative value for coordinates: boundaries probably exceeded (-32 768 to 32 767)");
        re->rect = Rect(r[0], r[1], r[2], r[3]);

        lay_item_t* pchild = lay_get_item(&ctx, child);
        child = pchild->next_sibling;
        re++;
    }
}

uint32_t justify_to_contains(Justification justify, Orientation orient)
{
    uint32_t contains = 0;

    switch (justify)
    {
    case Justification::start:
        contains |= LAY_START;
        break;
    case Justification::middle:
        contains |= LAY_MIDDLE;
        break;
    case Justification::ending:
        contains |= LAY_END;
        break;
    case Justification::justify:
        contains |= LAY_JUSTIFY;
        break;
    case Justification::none:
        break;
    }

    if (orient == Orientation::horizontal)
        contains |= LAY_ROW;
    else if (orient == Orientation::vertical)
        contains |= LAY_COLUMN;
    else if (orient == Orientation::none)
        contains |= (LAY_LAYOUT);
    else
        contains |= (LAY_FLEX | LAY_WRAP);

    return contains;
}

uint32_t align_to_behave(const AlignFlags& align)
{
    uint32_t behave = 0;
    if (align.is_set(AlignFlag::expand_horizontal))
        behave |= LAY_HFILL;
    else if (align.is_set(AlignFlag::left))
        behave |= LAY_LEFT;
    else if (align.is_set(AlignFlag::right))
        behave |= LAY_RIGHT;

    if (align.is_set(AlignFlag::expand_vertical))
        behave |= LAY_VFILL;
    else if (align.is_set(AlignFlag::top))
        behave |= LAY_TOP;
    else if (align.is_set(AlignFlag::bottom))
        behave |= LAY_BOTTOM;
    return behave;
}

void flex_layout(const Rect& parent,
                 std::vector<LayoutRect>& children,
                 Justification justify,
                 Orientation orient)
{
    lay_context ctx;
    lay_init_context(&ctx);

    auto cleanup = on_scope_exit([&ctx]()
    {
        lay_destroy_context(&ctx);
    });

    lay_reserve_items_capacity(&ctx, children.size() + 1);

    lay_id outer_parent = lay_item(&ctx);
    lay_set_size_xy(&ctx, outer_parent, parent.width(), parent.height());
    uint32_t contains = justify_to_contains(justify, orient);
    lay_set_contain(&ctx, outer_parent, contains);

    run_and_apply(ctx, outer_parent, children);
}

void flex_layout(const Rect& parent,
                 std::vector<LayoutRect>& children,
                 Justification justify,
                 Orientation orient,
                 const AlignFlags& align)
{
    lay_context ctx;
    lay_init_context(&ctx);

    auto cleanup = on_scope_exit([&ctx]()
    {
        lay_destroy_context(&ctx);
    });

    lay_reserve_items_capacity(&ctx, children.size() + 2);

    lay_id outer_parent = lay_item(&ctx);
    lay_id inner_parent = lay_item(&ctx);

    lay_set_size_xy(&ctx, outer_parent, parent.width(), parent.height());

    uint32_t contains = justify_to_contains(justify, orient);
    lay_set_contain(&ctx, inner_parent, contains);

    uint32_t dbehave = align_to_behave(align);
    lay_set_behave(&ctx, inner_parent, dbehave);
    lay_insert(&ctx, outer_parent, inner_parent);

    run_and_apply(ctx, inner_parent, children);
}

}
}
}
