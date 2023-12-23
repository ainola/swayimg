// SPDX-License-Identifier: MIT
// Keyboard bindings.
// Copyright (C) 2023 Artem Senichev <artemsen@gmail.com>

#include "keybind.h"

#include <stdlib.h>
#include <string.h>

/** Action names. */
static const char* action_names[] = {
    [kb_none] = NULL,
    [kb_first_file] = "first_file",
    [kb_last_file] = "last_file",
    [kb_prev_dir] = "prev_dir",
    [kb_next_dir] = "next_dir",
    [kb_prev_file] = "prev_file",
    [kb_next_file] = "next_file",
    [kb_prev_frame] = "prev_frame",
    [kb_next_frame] = "next_frame",
    [kb_animation] = "animation",
    [kb_slideshow] = "slideshow",
    [kb_fullscreen] = "fullscreen",
    [kb_step_left] = "step_left",
    [kb_step_right] = "step_right",
    [kb_step_up] = "step_up",
    [kb_step_down] = "step_down",
    [kb_zoom_in] = "zoom_in",
    [kb_zoom_out] = "zoom_out",
    [kb_zoom_optimal] = "oom_optimal",
    [kb_zoom_fit] = "zoom_fit",
    [kb_zoom_fill] = "zoom_fill",
    [kb_zoom_real] = "zoom_real",
    [kb_zoom_reset] = "zoom_reset",
    [kb_rotate_left] = "rotate_left",
    [kb_rotate_right] = "rotate_right",
    [kb_flip_vertical] = "flip_vertical",
    [kb_flip_horizontal] = "flip_horizontal",
    [kb_reload] = "reload",
    [kb_antialiasing] = "antialiasing",
    [kb_info] = "info",
    [kb_exec] = "exec",
    [kb_quit] = "quit",
};

// Default key bindings
static struct key_binding default_bindings[] = {
    { XKB_KEY_Home, kb_first_file, NULL },
    { XKB_KEY_g, kb_first_file, NULL },
    { XKB_KEY_End, kb_last_file, NULL },
    { XKB_KEY_G, kb_last_file, NULL },
    { XKB_KEY_P, kb_prev_dir, NULL },
    { XKB_KEY_N, kb_next_dir, NULL },
    { XKB_KEY_SunPageUp, kb_prev_file, NULL },
    { XKB_KEY_p, kb_prev_file, NULL },
    { XKB_KEY_SunPageDown, kb_next_file, NULL },
    { XKB_KEY_n, kb_next_file, NULL },
    { XKB_KEY_space, kb_next_file, NULL },
    { XKB_KEY_F2, kb_prev_frame, NULL },
    { XKB_KEY_O, kb_prev_frame, NULL },
    { XKB_KEY_F3, kb_next_frame, NULL },
    { XKB_KEY_o, kb_next_frame, NULL },
    { XKB_KEY_F4, kb_animation, NULL },
    { XKB_KEY_s, kb_animation, NULL },
    { XKB_KEY_F9, kb_slideshow, NULL },
    { XKB_KEY_F11, kb_fullscreen, NULL },
    { XKB_KEY_f, kb_fullscreen, NULL },
    { XKB_KEY_Left, kb_step_left, NULL },
    { XKB_KEY_h, kb_step_left, NULL },
    { XKB_KEY_Right, kb_step_right, NULL },
    { XKB_KEY_l, kb_step_right, NULL },
    { XKB_KEY_Up, kb_step_up, NULL },
    { XKB_KEY_k, kb_step_up, NULL },
    { XKB_KEY_Down, kb_step_down, NULL },
    { XKB_KEY_j, kb_step_down, NULL },
    { XKB_KEY_equal, kb_zoom_in, NULL },
    { XKB_KEY_plus, kb_zoom_in, NULL },
    { XKB_KEY_minus, kb_zoom_out, NULL },
    { XKB_KEY_x, kb_zoom_optimal, NULL },
    { XKB_KEY_z, kb_zoom_fit, NULL },
    { XKB_KEY_Z, kb_zoom_fill, NULL },
    { XKB_KEY_0, kb_zoom_real, NULL },
    { XKB_KEY_BackSpace, kb_zoom_reset, NULL },
    { XKB_KEY_F5, kb_rotate_left, NULL },
    { XKB_KEY_bracketleft, kb_rotate_left, NULL },
    { XKB_KEY_F6, kb_rotate_right, NULL },
    { XKB_KEY_bracketright, kb_rotate_right, NULL },
    { XKB_KEY_F7, kb_flip_vertical, NULL },
    { XKB_KEY_F8, kb_flip_horizontal, NULL },
    { XKB_KEY_a, kb_antialiasing, NULL },
    { XKB_KEY_r, kb_reload, NULL },
    { XKB_KEY_i, kb_info, NULL },
    { XKB_KEY_e, kb_exec, NULL },
    { XKB_KEY_Escape, kb_quit, NULL },
    { XKB_KEY_Return, kb_quit, NULL },
    { XKB_KEY_F10, kb_quit, NULL },
    { XKB_KEY_q, kb_quit, NULL },
};

/** Keybind context. */
struct keybind_context {
    struct key_binding* bindings;
    size_t size;
};
static struct keybind_context ctx;

void keybind_init(void)
{
    ctx.bindings = malloc(sizeof(default_bindings));
    if (ctx.bindings) {
        memcpy(ctx.bindings, default_bindings, sizeof(default_bindings));
        ctx.size = sizeof(default_bindings) / sizeof(default_bindings[0]);
    }
    keybind_set(XKB_KEY_e, kb_exec, "echo \"Current file: %\"");
}

void keybind_free(void)
{
    for (size_t i = 0; i < ctx.size; ++i) {
        free(ctx.bindings[i].params);
    }
    free(ctx.bindings);
}

void keybind_set(xkb_keysym_t key, enum kb_action action, const char* params)
{
    struct key_binding* new_binding = NULL;

    for (size_t i = 0; i < ctx.size; ++i) {
        struct key_binding* binding = &ctx.bindings[i];
        if (binding->key == key) {
            new_binding = binding; // replace existing
            break;
        } else if (binding->action == kb_none && !new_binding) {
            new_binding = binding; // reuse existing
        }
    }

    if (!new_binding) {
        // add new (reallocate)
        const size_t new_sz = (ctx.size + 1) * sizeof(struct key_binding);
        struct key_binding* bindings = realloc(ctx.bindings, new_sz);
        if (!bindings) {
            return;
        }
        new_binding = &bindings[ctx.size];
        new_binding->params = NULL;
        ctx.bindings = bindings;
        ++ctx.size;
    } else if (new_binding->params) {
        // free existing buffer
        free(new_binding->params);
        new_binding->params = NULL;
    }

    // set new parameters
    new_binding->key = key;
    new_binding->action = action;
    if (params && *params) {
        const size_t sz = strlen(params) + 1;
        new_binding->params = malloc(sz);
        if (new_binding->params) {
            memcpy(new_binding->params, params, sz);
        }
    }
}

const struct key_binding* keybind_get(xkb_keysym_t key)
{
    for (size_t i = 0; i < ctx.size; ++i) {
        struct key_binding* binding = &ctx.bindings[i];
        if (binding->key == key) {
            return binding;
        }
    }
    return NULL;
}

enum kb_action keybind_action(const char* name)
{
    size_t i;
    for (i = 1; i < sizeof(action_names) / sizeof(action_names[0]); ++i) {
        if (strcmp(name, action_names[i]) == 0) {
            return (enum kb_action)i;
        }
    }
    return kb_none;
}