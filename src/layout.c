#include "dwc.h"
#include <wlr/types/wlr_xdg_shell.h>

void reset_cursor_mode(struct dwc_server *server) {
	server->cursor_mode = DWC_CURSOR_PASSTHROUGH;
	server->grabbed_toplevel = NULL;
}

void process_cursor_move(struct dwc_server *server) {
	struct dwc_toplevel *toplevel = server->grabbed_toplevel;
	wlr_scene_node_set_position(
        &toplevel->scene_tree->node,
        server->cursor->x - server->grab_x,
        server->cursor->y - server->grab_y
    );
}

void process_cursor_resize(struct dwc_server *server) {
	struct dwc_toplevel *toplevel = server->grabbed_toplevel;
	double border_x = server->cursor->x - server->grab_x;
	double border_y = server->cursor->y - server->grab_y;
	int new_left = server->grab_geobox.x;
	int new_right = server->grab_geobox.x + server->grab_geobox.width;
	int new_top = server->grab_geobox.y;
	int new_bottom = server->grab_geobox.y + server->grab_geobox.height;

	if (server->resize_edges & WLR_EDGE_TOP) {
		new_top = border_y;
		if (new_top >= new_bottom) {
			new_top = new_bottom - 1;
		}
	} else if (server->resize_edges & WLR_EDGE_BOTTOM) {
		new_bottom = border_y;
		if (new_bottom <= new_top) {
			new_bottom = new_top + 1;
		}
	}
	if (server->resize_edges & WLR_EDGE_LEFT) {
		new_left = border_x;
		if (new_left >= new_right) {
			new_left = new_right - 1;
		}
	} else if (server->resize_edges & WLR_EDGE_RIGHT) {
		new_right = border_x;
		if (new_right <= new_left) {
			new_right = new_left + 1;
		}
	}

	struct wlr_box *geometry_box = &toplevel->xdg_toplevel->base->geometry;
	wlr_scene_node_set_position(
        &toplevel->scene_tree->node,
        new_left - geometry_box->x,
        new_top - geometry_box->y
    );

	int new_width = new_right - new_left;
	int new_height = new_bottom - new_top;
	wlr_xdg_toplevel_set_size(toplevel->xdg_toplevel, new_width, new_height);
}

void begin_interactive(struct dwc_toplevel *toplevel, enum dwc_cursor_mode mode, uint32_t edges) {
	struct dwc_server *server = toplevel->server;

	server->grabbed_toplevel = toplevel;
	server->cursor_mode = mode;

	if (mode == DWC_CURSOR_MOVE) {
		server->grab_x = server->cursor->x - toplevel->scene_tree->node.x;
		server->grab_y = server->cursor->y - toplevel->scene_tree->node.y;
	} else {
		struct wlr_box *geometry_box = &toplevel->xdg_toplevel->base->geometry;

		double border_x = (toplevel->scene_tree->node.x + geometry_box->x) +
			((edges & WLR_EDGE_RIGHT) ? geometry_box->width : 0);
		double border_y = (toplevel->scene_tree->node.y + geometry_box->y) +
			((edges & WLR_EDGE_BOTTOM) ? geometry_box->height : 0);
		server->grab_x = server->cursor->x - border_x;
		server->grab_y = server->cursor->y - border_y;

		server->grab_geobox = *geometry_box;
		server->grab_geobox.x += toplevel->scene_tree->node.x;
		server->grab_geobox.y += toplevel->scene_tree->node.y;

		server->resize_edges = edges;
	}
}
