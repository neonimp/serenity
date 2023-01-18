/*
 * Copyright (c) 2022, Sam Atkins <atkinssj@serenityos.org>
 * Copyright (c) 2023, MacDue <macdue@dueutil.tech>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Variant.h>
#include <AK/Vector.h>
#include <LibGfx/AffineTransform.h>
#include <LibGfx/Color.h>
#include <LibGfx/PaintStyle.h>
#include <LibWeb/HTML/CanvasGradient.h>

namespace Web::HTML {

// https://html.spec.whatwg.org/multipage/canvas.html#canvasstate
class CanvasState {
public:
    virtual ~CanvasState() = default;

    void save();
    void restore();
    void reset();
    bool is_context_lost();

    using FillOrStrokeVariant = Variant<Gfx::Color, JS::Handle<CanvasGradient>>;

    struct FillOrStrokeStyle {
        FillOrStrokeStyle(Gfx::Color color)
            : m_fill_or_stoke_style(color)
        {
        }

        FillOrStrokeStyle(JS::Handle<CanvasGradient> gradient)
            : m_fill_or_stoke_style(gradient)
        {
        }

        NonnullRefPtr<Gfx::PaintStyle> to_gfx_paint_style();

        Optional<Gfx::Color> as_color() const;
        Gfx::Color to_color_but_fixme_should_accept_any_paint_style() const;

        Variant<DeprecatedString, JS::Handle<CanvasGradient>> to_js_fill_or_stoke_style() const
        {
            if (auto* handle = m_fill_or_stoke_style.get_pointer<JS::Handle<CanvasGradient>>())
                return *handle;
            return m_fill_or_stoke_style.get<Gfx::Color>().to_deprecated_string();
        }

    private:
        FillOrStrokeVariant m_fill_or_stoke_style;
        RefPtr<Gfx::PaintStyle> m_color_fill_style { nullptr };
    };

    // https://html.spec.whatwg.org/multipage/canvas.html#drawing-state
    struct DrawingState {
        Gfx::AffineTransform transform;
        FillOrStrokeStyle fill_style { Gfx::Color::Black };
        FillOrStrokeStyle stroke_style { Gfx::Color::Black };
        float line_width { 1 };
    };
    DrawingState& drawing_state() { return m_drawing_state; }
    DrawingState const& drawing_state() const { return m_drawing_state; }

    void clear_drawing_state_stack() { m_drawing_state_stack.clear(); }
    void reset_drawing_state() { m_drawing_state = {}; }

    virtual void reset_to_default_state() = 0;

protected:
    CanvasState() = default;

private:
    DrawingState m_drawing_state;
    Vector<DrawingState> m_drawing_state_stack;

    // https://html.spec.whatwg.org/multipage/canvas.html#concept-canvas-context-lost
    bool m_context_lost { false };
};

}
