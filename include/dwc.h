#ifndef DWC_H
#define DWC_H

#include "types.h"
#include <stdbool.h>
#include <xkbcommon/xkbcommon.h>

bool server_init(struct dwc_server *server);
void server_run(struct dwc_server *server, const char *startup_cmd);
void server_cleanup(struct dwc_server *server);

void server_new_output(struct wl_listener *listener, void *data);
void output_frame(struct wl_listener *listener, void *data);
void output_request_state(struct wl_listener *listener, void *data);
void output_destroy(struct wl_listener *listener, void *data);

void server_new_input(struct wl_listener *listener, void *data);
void server_new_keyboard(struct dwc_server *server, struct wlr_input_device *device);
void server_new_pointer(struct dwc_server *server, struct wlr_input_device *device);
void keyboard_handle_modifiers(struct wl_listener *listener, void *data);
void keyboard_handle_key(struct wl_listener *listener, void *data);
void keyboard_handle_destroy(struct wl_listener *listener, void *data);
bool handle_keybinding(struct dwc_server *server, xkb_keysym_t sym);

void server_cursor_motion(struct wl_listener *listener, void *data);
void server_cursor_motion_absolute(struct wl_listener *listener, void *data);
void server_cursor_button(struct wl_listener *listener, void *data);
void server_cursor_axis(struct wl_listener *listener, void *data);
void server_cursor_frame(struct wl_listener *listener, void *data);
void process_cursor_motion(struct dwc_server *server, uint32_t time);

void seat_request_cursor(struct wl_listener *listener, void *data);
void seat_pointer_focus_change(struct wl_listener *listener, void *data);
void seat_request_set_selection(struct wl_listener *listener, void *data);

void server_new_xdg_toplevel(struct wl_listener *listener, void *data);
void server_new_xdg_popup(struct wl_listener *listener, void *data);
void xdg_toplevel_map(struct wl_listener *listener, void *data);
void xdg_toplevel_unmap(struct wl_listener *listener, void *data);
void xdg_toplevel_commit(struct wl_listener *listener, void *data);
void xdg_toplevel_destroy(struct wl_listener *listener, void *data);
void xdg_toplevel_request_move(struct wl_listener *listener, void *data);
void xdg_toplevel_request_resize(struct wl_listener *listener, void *data);
void xdg_toplevel_request_maximize(struct wl_listener *listener, void *data);
void xdg_toplevel_request_fullscreen(struct wl_listener *listener, void *data);
void xdg_popup_commit(struct wl_listener *listener, void *data);
void xdg_popup_destroy(struct wl_listener *listener, void *data);

void focus_toplevel(struct dwc_toplevel *toplevel);
struct dwc_toplevel *desktop_toplevel_at(struct dwc_server *server,
	double layout_x, double layout_y, struct wlr_surface **surface,
	double *surface_x, double *surface_y);

void begin_interactive(struct dwc_toplevel *toplevel, enum dwc_cursor_mode mode, uint32_t edges);
void reset_cursor_mode(struct dwc_server *server);
void process_cursor_move(struct dwc_server *server);
void process_cursor_resize(struct dwc_server *server);

#endif
