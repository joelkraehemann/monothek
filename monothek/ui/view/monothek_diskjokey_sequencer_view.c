/* Monothek - monoidea's monothek
 * Copyright (C) 2018 Joël Krähemann
 *
 * This file is part of Monothek.
 *
 * Monothek is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Monothek is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Monothek.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <monothek/ui/view/monothek_diskjokey_sequencer_view.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <stdlib.h>

#include <monothek/i18n.h>

void monothek_diskjokey_sequencer_view_class_init(MonothekDiskjokeySequencerViewClass *diskjokey_sequencer_view);
void monothek_diskjokey_sequencer_view_connectable_interface_init(AgsConnectableInterface *connectable);
void monothek_diskjokey_sequencer_view_init(MonothekDiskjokeySequencerView *diskjokey_sequencer_view);
void monothek_diskjokey_sequencer_view_set_property(GObject *gobject,
						    guint prop_id,
						    const GValue *value,
						    GParamSpec *param_spec);
void monothek_diskjokey_sequencer_view_get_property(GObject *gobject,
						    guint prop_id,
						    GValue *value,
						    GParamSpec *param_spec);
void monothek_diskjokey_sequencer_view_finalize(GObject *gobject);

void monothek_diskjokey_sequencer_view_connect(AgsConnectable *connectable);
void monothek_diskjokey_sequencer_view_disconnect(AgsConnectable *connectable);

void monothek_diskjokey_sequencer_view_draw(MonothekView *view);

/**
 * SECTION:monothek_diskjokey_sequencer_view
 * @short_description: The diskjokey sequencer view object.
 * @title: MonothekDiskjokeySequencerView
 * @section_id:
 * @include: monothek/ui/view/monothek_diskjokey_sequencer_view.h
 *
 * #MonothekDiskjokeySequencerView is the MVC's diskjokey sequencer view widget.
 */

enum{
  PROP_0,
};

static gpointer monothek_diskjokey_sequencer_view_parent_class = NULL;
static AgsConnectableInterface *monothek_diskjokey_sequencer_view_parent_connectable_interface;

GType
monothek_diskjokey_sequencer_view_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType monothek_type_diskjokey_sequencer_view = 0;

    static const GTypeInfo monothek_diskjokey_sequencer_view_info = {
      sizeof (MonothekDiskjokeySequencerViewClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) monothek_diskjokey_sequencer_view_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (MonothekDiskjokeySequencerView),
      0,    /* n_preallocs */
      (GInstanceInitFunc) monothek_diskjokey_sequencer_view_init,
    };

    static const GInterfaceInfo monothek_connectable_interface_info = {
      (GInterfaceInitFunc) monothek_diskjokey_sequencer_view_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    monothek_type_diskjokey_sequencer_view = g_type_register_static(MONOTHEK_TYPE_VIEW,
								    "MonothekDiskjokeySequencerView", &monothek_diskjokey_sequencer_view_info,
								    0);
    
    g_type_add_interface_static(monothek_type_diskjokey_sequencer_view,
				AGS_TYPE_CONNECTABLE,
				&monothek_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, monothek_type_diskjokey_sequencer_view);
  }

  return g_define_type_id__volatile;
}

void
monothek_diskjokey_sequencer_view_class_init(MonothekDiskjokeySequencerViewClass *diskjokey_sequencer_view)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  MonothekViewClass *view;
  
  GParamSpec *param_spec;

  monothek_diskjokey_sequencer_view_parent_class = g_type_class_peek_parent(diskjokey_sequencer_view);

  /* GObjectClass */
  gobject = (GObjectClass *) diskjokey_sequencer_view;

  gobject->set_property = monothek_diskjokey_sequencer_view_set_property;
  gobject->get_property = monothek_diskjokey_sequencer_view_get_property;

  gobject->finalize = monothek_diskjokey_sequencer_view_finalize;

  /* properties */

  /* MonothekView */
  view = (MonothekViewClass *) diskjokey_sequencer_view;

  view->draw = monothek_diskjokey_sequencer_view_draw;
}

void
monothek_diskjokey_sequencer_view_connectable_interface_init(AgsConnectableInterface *connectable)
{
  monothek_diskjokey_sequencer_view_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = monothek_diskjokey_sequencer_view_connect;
  connectable->disconnect = monothek_diskjokey_sequencer_view_disconnect;
}

void
monothek_diskjokey_sequencer_view_init(MonothekDiskjokeySequencerView *diskjokey_sequencer_view)
{
  /* pattern */
  diskjokey_sequencer_view->pattern_line_width = 5.0;

  diskjokey_sequencer_view->pattern_x0 = 160.0;
  diskjokey_sequencer_view->pattern_y0 = 80.0;

  diskjokey_sequencer_view->pattern_column_spacing = 20.0;
  diskjokey_sequencer_view->pattern_row_spacing = 20.0;

  diskjokey_sequencer_view->pad_width = 60.0;
  diskjokey_sequencer_view->pad_height = 60.0;

  /* tab - 0 */
  diskjokey_sequencer_view->tab_box_line_width[0] = 5.0;

  diskjokey_sequencer_view->tab_box_x0[0] = 160.0;
  diskjokey_sequencer_view->tab_box_y0[0] = 800.0;

  diskjokey_sequencer_view->tab_box_width[0] = 300.0;
  diskjokey_sequencer_view->tab_box_height[0] = 60.0;

  /* tab - 1 */
  diskjokey_sequencer_view->tab_box_line_width[1] = 5.0;

  diskjokey_sequencer_view->tab_box_x0[1] = 480.0;
  diskjokey_sequencer_view->tab_box_y0[1] = 800.0;

  diskjokey_sequencer_view->tab_box_width[1] = 300.0;
  diskjokey_sequencer_view->tab_box_height[1] = 60.0;

  /* tab - 2 */
  diskjokey_sequencer_view->tab_box_line_width[2] = 5.0;

  diskjokey_sequencer_view->tab_box_x0[2] = 800.0;
  diskjokey_sequencer_view->tab_box_y0[2] = 800.0;

  diskjokey_sequencer_view->tab_box_width[2] = 300.0;
  diskjokey_sequencer_view->tab_box_height[2] = 60.0;

  /* tab - 3 */
  diskjokey_sequencer_view->tab_box_line_width[3] = 5.0;

  diskjokey_sequencer_view->tab_box_x0[3] = 1120.0;
  diskjokey_sequencer_view->tab_box_y0[3] = 800.0;

  diskjokey_sequencer_view->tab_box_width[3] = 300.0;
  diskjokey_sequencer_view->tab_box_height[3] = 60.0;
  
  /* bpm */
  diskjokey_sequencer_view->bpm_box_line_width = 5.0;

  diskjokey_sequencer_view->bpm_box_x0 = 160.0;
  diskjokey_sequencer_view->bpm_box_y0 = 880.0;

  diskjokey_sequencer_view->bpm_box_width = 140.0;
  diskjokey_sequencer_view->bpm_box_height = 60.0;

  diskjokey_sequencer_view->bpm_control_line_width = 5.0;

  diskjokey_sequencer_view->bpm_control_x0 = 160.0;
  diskjokey_sequencer_view->bpm_control_y0 = 990.0;

  diskjokey_sequencer_view->bpm_control_width = 620.0;
  diskjokey_sequencer_view->bpm_control_height = 20.0;

  /* swing */
  diskjokey_sequencer_view->swing_box_line_width = 5.0;

  diskjokey_sequencer_view->swing_box_x0 = 800.0;
  diskjokey_sequencer_view->swing_box_y0 = 880.0;

  diskjokey_sequencer_view->swing_box_width = 60.0;
  diskjokey_sequencer_view->swing_box_height = 60.0;

  diskjokey_sequencer_view->swing_control_line_width = 5.0;

  diskjokey_sequencer_view->swing_control_x0 = 800.0;
  diskjokey_sequencer_view->swing_control_y0 = 990.0;

  diskjokey_sequencer_view->swing_control_width = 620.0;
  diskjokey_sequencer_view->swing_control_height = 20.0;
  
  /* techno */
  diskjokey_sequencer_view->techno_box_line_width = 5.0;

  diskjokey_sequencer_view->techno_box_x0 = 1460.0;
  diskjokey_sequencer_view->techno_box_y0 = 80.0;

  diskjokey_sequencer_view->techno_box_width = 420.0;
  diskjokey_sequencer_view->techno_box_height = 140.0;

  /* house */
  diskjokey_sequencer_view->house_box_line_width = 5.0;

  diskjokey_sequencer_view->house_box_x0 = 1460.0;
  diskjokey_sequencer_view->house_box_y0 = 240.0;

  diskjokey_sequencer_view->house_box_width = 420.0;
  diskjokey_sequencer_view->house_box_height = 140.0;

  /* hiphop */
  diskjokey_sequencer_view->hiphop_box_line_width = 5.0;

  diskjokey_sequencer_view->hiphop_box_x0 = 1460.0;
  diskjokey_sequencer_view->hiphop_box_y0 = 400.0;

  diskjokey_sequencer_view->hiphop_box_width = 420.0;
  diskjokey_sequencer_view->hiphop_box_height = 140.0;

  /* random */
  diskjokey_sequencer_view->random_box_line_width = 5.0;

  diskjokey_sequencer_view->random_box_x0 = 1460.0;
  diskjokey_sequencer_view->random_box_y0 = 560.0;

  diskjokey_sequencer_view->random_box_width = 200.0;
  diskjokey_sequencer_view->random_box_height = 60.0;

  /* clear */
  diskjokey_sequencer_view->clear_box_line_width = 5.0;

  diskjokey_sequencer_view->clear_box_x0 = 1680.0;
  diskjokey_sequencer_view->clear_box_y0 = 560.0;

  diskjokey_sequencer_view->clear_box_width = 200.0;
  diskjokey_sequencer_view->clear_box_height = 60.0;

  /* timer */
  diskjokey_sequencer_view->timer_box_line_width = 5.0;

  diskjokey_sequencer_view->timer_box_x0 = 1460.0;
  diskjokey_sequencer_view->timer_box_y0 = 720.0;

  diskjokey_sequencer_view->timer_box_width = 420.0;
  diskjokey_sequencer_view->timer_box_height = 140.0;

  /* stop */
  diskjokey_sequencer_view->stop_box_line_width = 5.0;

  diskjokey_sequencer_view->stop_box_x0 = 1460.0;
  diskjokey_sequencer_view->stop_box_y0 = 880.0;

  diskjokey_sequencer_view->stop_box_width = 420.0;
  diskjokey_sequencer_view->stop_box_height = 140.0;
}

void
monothek_diskjokey_sequencer_view_set_property(GObject *gobject,
					       guint prop_id,
					       const GValue *value,
					       GParamSpec *param_spec)
{
  MonothekDiskjokeySequencerView *diskjokey_sequencer_view;

  diskjokey_sequencer_view = MONOTHEK_DISKJOKEY_SEQUENCER_VIEW(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
monothek_diskjokey_sequencer_view_get_property(GObject *gobject,
					       guint prop_id,
					       GValue *value,
					       GParamSpec *param_spec)
{
  MonothekDiskjokeySequencerView *diskjokey_sequencer_view;

  diskjokey_sequencer_view = MONOTHEK_DISKJOKEY_SEQUENCER_VIEW(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
monothek_diskjokey_sequencer_view_finalize(GObject *gobject)
{
  MonothekDiskjokeySequencerView *diskjokey_sequencer_view;

  diskjokey_sequencer_view = (MonothekDiskjokeySequencerView *) gobject;
  
  /* call parent */
  G_OBJECT_CLASS(monothek_diskjokey_sequencer_view_parent_class)->finalize(gobject);
}

void
monothek_diskjokey_sequencer_view_connect(AgsConnectable *connectable)
{
  MonothekDiskjokeySequencerView *diskjokey_sequencer_view;

  GList *list, *list_diskjokey_sequencer;

  diskjokey_sequencer_view = MONOTHEK_DISKJOKEY_SEQUENCER_VIEW(connectable);

  if((MONOTHEK_VIEW_CONNECTED & (MONOTHEK_VIEW(diskjokey_sequencer_view)->flags)) != 0){
    return;
  }

  monothek_diskjokey_sequencer_view_parent_connectable_interface->connect(connectable);

  //TODO:JK: implement me
}

void
monothek_diskjokey_sequencer_view_disconnect(AgsConnectable *connectable)
{
  MonothekDiskjokeySequencerView *diskjokey_sequencer_view;

  GList *list, *list_diskjokey_sequencer;

  diskjokey_sequencer_view = MONOTHEK_DISKJOKEY_SEQUENCER_VIEW(connectable);

  if((MONOTHEK_VIEW_CONNECTED & (MONOTHEK_VIEW(diskjokey_sequencer_view)->flags)) == 0){
    return;
  }

  monothek_diskjokey_sequencer_view_parent_connectable_interface->disconnect(connectable);

  //TODO:JK: implement me
}

void
monothek_diskjokey_sequencer_view_draw(MonothekView *view)
{
  MonothekDiskjokeySequencerView *diskjokey_sequencer_view;
  
  cairo_t *cr;

  guint width, height;
  guint x_diskjokey_sequencer, y_diskjokey_sequencer;
  guint i, j;
  
  static const gdouble white_gc = 65535.0;

  diskjokey_sequencer_view = MONOTHEK_DISKJOKEY_SEQUENCER_VIEW(view);
  
  /* call parent */
  MONOTHEK_VIEW_CLASS(monothek_diskjokey_sequencer_view_parent_class)->draw(view);

  /* create cr */
  cr = gdk_cairo_create(GTK_WIDGET(view)->window);
  
  if(cr == NULL){
    return;
  }

  cairo_surface_flush(cairo_get_target(cr));
  cairo_push_group(cr);

  x_diskjokey_sequencer = 0;
  y_diskjokey_sequencer = 0;

  width = GTK_WIDGET(view)->allocation.width;
  height = GTK_WIDGET(view)->allocation.height;

  cairo_set_source_rgb(cr,
		       1.0 / 255.0 * ((0xff0000 & view->diskjokey_gc) >> 16),
		       1.0 / 255.0 * ((0xff00 & view->diskjokey_gc) >> 8),
		       1.0 / 255.0 * ((0xff & view->diskjokey_gc)));

  /* columns numeration */
  for(j = 0; j < MONOTHEK_DISKJOKEY_SEQUENCER_VIEW_PATTERN_COLUMN_COUNT; j++){
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect;
    PangoRectangle logical_rect;
    
    gchar *diskjokey_font;
    gchar *label;
    
    static const guint font_size = 100;

    diskjokey_font = g_strdup_printf("%s Bold", view->font);

    label = g_strdup_printf("%d", j + 1);
    
    /* columns */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, label, -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    19 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) diskjokey_sequencer_view->pattern_x0 + (j * (diskjokey_sequencer_view->pad_width + diskjokey_sequencer_view->pattern_column_spacing)) + ((diskjokey_sequencer_view->pad_width / 2.0) - (logical_rect.width / 2.0)),
		  (double) 40.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    g_free(diskjokey_font);
    g_free(label);
  }

  /* row sound */
  for(i = 0; i < MONOTHEK_DISKJOKEY_SEQUENCER_VIEW_PATTERN_ROW_COUNT; i++){
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect;
    PangoRectangle logical_rect;
    
    gchar *diskjokey_font;
    gchar *label;
    
    static const guint font_size = 100;

    diskjokey_font = g_strdup_printf("%s Bold", view->font);

    label = g_strdup_printf("(null)");
    
    /* rows */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, label, -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    19 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) 140.0 - (logical_rect.width),
		  (double) diskjokey_sequencer_view->pattern_y0 + 20.0 + (i * (diskjokey_sequencer_view->pad_height + diskjokey_sequencer_view->pattern_row_spacing)));

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    g_free(diskjokey_font);
    g_free(label);
  }
  
  /* pattern */
  cairo_set_line_width(cr,
		       diskjokey_sequencer_view->pattern_line_width);

  for(i = 0; i < MONOTHEK_DISKJOKEY_SEQUENCER_VIEW_PATTERN_ROW_COUNT; i++){
    for(j = 0; j < MONOTHEK_DISKJOKEY_SEQUENCER_VIEW_PATTERN_COLUMN_COUNT; j++){
      cairo_rectangle(cr,
		      (double) diskjokey_sequencer_view->pattern_x0 + (j * (diskjokey_sequencer_view->pad_width + diskjokey_sequencer_view->pattern_column_spacing)), (double) diskjokey_sequencer_view->pattern_y0 + (i * (diskjokey_sequencer_view->pad_height + diskjokey_sequencer_view->pattern_row_spacing)),
		      (double) diskjokey_sequencer_view->pad_width, (double) diskjokey_sequencer_view->pad_height);
      cairo_stroke(cr);
    }
  }

  /* tabs */
  {
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect;
    PangoRectangle logical_rect;
    
    gchar *diskjokey_font;
    
    static const guint font_size = 100;

    diskjokey_font = g_strdup_printf("%s Bold", view->font);

    /* tab 1 */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, "TAB 1", -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    19 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) 270.0,
		  (double) 820.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* tab 2 */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, "TAB 2", -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    19 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) 590.0,
		  (double) 820.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* tab 3 */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, "TAB 3", -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    19 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) 910.0,
		  (double) 820.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* tab 4 */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, "TAB 4", -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    19 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) 1230.0,
		  (double) 820.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* free font string */
    g_free(diskjokey_font);
  }

  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->tab_box_x0[0], (double) diskjokey_sequencer_view->tab_box_y0[0],
		  (double) diskjokey_sequencer_view->tab_box_width[0], (double) diskjokey_sequencer_view->tab_box_height[0]);
  cairo_stroke(cr);

  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->tab_box_x0[1], (double) diskjokey_sequencer_view->tab_box_y0[1],
		  (double) diskjokey_sequencer_view->tab_box_width[1], (double) diskjokey_sequencer_view->tab_box_height[1]);
  cairo_stroke(cr);

  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->tab_box_x0[2], (double) diskjokey_sequencer_view->tab_box_y0[2],
		  (double) diskjokey_sequencer_view->tab_box_width[2], (double) diskjokey_sequencer_view->tab_box_height[2]);
  cairo_stroke(cr);
  
  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->tab_box_x0[3], (double) diskjokey_sequencer_view->tab_box_y0[3],
		  (double) diskjokey_sequencer_view->tab_box_width[3], (double) diskjokey_sequencer_view->tab_box_height[3]);
  cairo_stroke(cr);

  /* bpm */
  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->bpm_box_x0, (double) diskjokey_sequencer_view->bpm_box_y0,
		  (double) diskjokey_sequencer_view->bpm_box_width, (double) diskjokey_sequencer_view->bpm_box_height);
  cairo_stroke(cr);
  
  {
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect;
    PangoRectangle logical_rect;
    
    gchar *diskjokey_font;
    gchar *bpm;
    
    static const guint font_size = 100;

    diskjokey_font = g_strdup_printf("%s Bold", view->font);

    bpm = g_strdup_printf("120");
    
    /* bpm */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, bpm, -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    19 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) 200.0,
		  (double) 900.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* label */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, "BPM", -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    19 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) 320.0,
		  (double) 900.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* free font string */
    g_free(diskjokey_font);
  }

  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->bpm_control_x0, (double) diskjokey_sequencer_view->bpm_control_y0,
		  (double) diskjokey_sequencer_view->bpm_control_width, (double) diskjokey_sequencer_view->bpm_control_height);
  cairo_fill(cr);

  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->bpm_control_x0, (double) diskjokey_sequencer_view->bpm_control_y0 - 10.0,
		  (double) 40.0, (double) 40.0);
  cairo_fill(cr);

  /* swing */
  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->swing_box_x0, (double) diskjokey_sequencer_view->swing_box_y0,
		  (double) diskjokey_sequencer_view->swing_box_width, (double) diskjokey_sequencer_view->swing_box_height);
  cairo_stroke(cr);
  
  {
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect;
    PangoRectangle logical_rect;
    
    gchar *diskjokey_font;
    gchar *swing;
    
    static const guint font_size = 100;

    diskjokey_font = g_strdup_printf("%s Bold", view->font);

    swing = g_strdup_printf("6");
    
    /* swing */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, swing, -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    19 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) 820.0,
		  (double) 900.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* label */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, "SWING", -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    19 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) 880.0,
		  (double) 900.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* free font string */
    g_free(diskjokey_font);
  }

  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->swing_control_x0, (double) diskjokey_sequencer_view->swing_control_y0,
		  (double) diskjokey_sequencer_view->swing_control_width, (double) diskjokey_sequencer_view->swing_control_height);
  cairo_fill(cr);

  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->swing_control_x0, (double) diskjokey_sequencer_view->swing_control_y0 - 10.0,
		  (double) 40.0, (double) 40.0);
  cairo_fill(cr);

  /* techno */
  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->techno_box_x0, (double) diskjokey_sequencer_view->techno_box_y0,
		  (double) diskjokey_sequencer_view->techno_box_width, (double) diskjokey_sequencer_view->techno_box_height);
  cairo_stroke(cr);

  {
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect;
    PangoRectangle logical_rect;
    
    gchar *diskjokey_font;
    
    static const guint font_size = 100;

    diskjokey_font = g_strdup_printf("%s Bold", view->font);
    
    /* techno */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, "TECHNO", -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    40 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) diskjokey_sequencer_view->techno_box_x0 + ((diskjokey_sequencer_view->techno_box_width / 2.0) - (logical_rect.width / 2.0)),
		  (double) 132.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* free font string */
    g_free(diskjokey_font);
  }
  
  /* house */
  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->house_box_x0, (double) diskjokey_sequencer_view->house_box_y0,
		  (double) diskjokey_sequencer_view->house_box_width, (double) diskjokey_sequencer_view->house_box_height);
  cairo_stroke(cr);

  {
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect;
    PangoRectangle logical_rect;
    
    gchar *diskjokey_font;
    
    static const guint font_size = 100;

    diskjokey_font = g_strdup_printf("%s Bold", view->font);
    
    /* house */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, "HOUSE", -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    40 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) diskjokey_sequencer_view->house_box_x0 + ((diskjokey_sequencer_view->house_box_width / 2.0) - (logical_rect.width / 2.0)),
		  (double) 292.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* free font string */
    g_free(diskjokey_font);
  }

  /* hiphop */
  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->hiphop_box_x0, (double) diskjokey_sequencer_view->hiphop_box_y0,
		  (double) diskjokey_sequencer_view->hiphop_box_width, (double) diskjokey_sequencer_view->hiphop_box_height);
  cairo_stroke(cr);

  {
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect;
    PangoRectangle logical_rect;
    
    gchar *diskjokey_font;
    
    static const guint font_size = 100;

    diskjokey_font = g_strdup_printf("%s Bold", view->font);
    
    /* hiphop */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, "HIPHOP", -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    40 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) diskjokey_sequencer_view->hiphop_box_x0 + ((diskjokey_sequencer_view->hiphop_box_width / 2.0) - (logical_rect.width / 2.0)),
		  (double) 452.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* free font string */
    g_free(diskjokey_font);
  }

  /* random */
  {
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect;
    PangoRectangle logical_rect;
    
    gchar *diskjokey_font;
    
    static const guint font_size = 100;

    diskjokey_font = g_strdup_printf("%s Bold", view->font);
    
    /* random */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, "RANDOM", -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    19 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) diskjokey_sequencer_view->random_box_x0 + ((diskjokey_sequencer_view->random_box_width / 2.0) - (logical_rect.width / 2.0)),
		  (double) 580.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* free font string */
    g_free(diskjokey_font);
  }

  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->random_box_x0, (double) diskjokey_sequencer_view->random_box_y0,
		  (double) diskjokey_sequencer_view->random_box_width, (double) diskjokey_sequencer_view->random_box_height);
  cairo_stroke(cr);

  /* clear */
  {
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect;
    PangoRectangle logical_rect;
    
    gchar *diskjokey_font;
    
    static const guint font_size = 100;

    diskjokey_font = g_strdup_printf("%s Bold", view->font);
    
    /* clear */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, "CLEAR", -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    19 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) diskjokey_sequencer_view->clear_box_x0 + ((diskjokey_sequencer_view->clear_box_width / 2.0) - (logical_rect.width / 2.0)),
		  (double) 580.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* free font string */
    g_free(diskjokey_font);
  }

  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->clear_box_x0, (double) diskjokey_sequencer_view->clear_box_y0,
		  (double) diskjokey_sequencer_view->clear_box_width, (double) diskjokey_sequencer_view->clear_box_height);
  cairo_stroke(cr);

  /* timer */
  {
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect;
    PangoRectangle logical_rect;
    
    gchar *diskjokey_font;
    gchar *timer;
    
    static const guint font_size = 100;

    diskjokey_font = g_strdup_printf("%s Bold", view->font);

    /* label */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, "TIMER", -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    19 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) 1460.0,
		  (double) 680.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);
    
    timer = g_strdup_printf("10:00");
    
    /* timer */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, timer, -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    40 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) diskjokey_sequencer_view->timer_box_x0 + ((diskjokey_sequencer_view->timer_box_width / 2.0) - (logical_rect.width / 2.0)),
		  (double) 772.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* free font string */
    g_free(diskjokey_font);
  }

  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->timer_box_x0, (double) diskjokey_sequencer_view->timer_box_y0,
		  (double) diskjokey_sequencer_view->timer_box_width, (double) diskjokey_sequencer_view->timer_box_height);
  cairo_stroke(cr);

  /* stop */
  {
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect;
    PangoRectangle logical_rect;
    
    gchar *diskjokey_font;
    
    static const guint font_size = 100;

    diskjokey_font = g_strdup_printf("%s Bold", view->font);
    
    /* stop */
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, "STOP", -1);
    desc = pango_font_description_from_string(diskjokey_font);
    pango_font_description_set_size(desc,
				    40 * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout,
			       PANGO_ALIGN_CENTER);
    pango_font_description_free(desc);

    pango_layout_get_pixel_extents(layout,
				   &ink_rect,
				   &logical_rect);
    cairo_move_to(cr,
		  (double) diskjokey_sequencer_view->stop_box_x0 + ((diskjokey_sequencer_view->stop_box_width / 2.0) - (logical_rect.width / 2.0)),
		  (double) 932.0);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);

    /* free font string */
    g_free(diskjokey_font);
  }

  cairo_rectangle(cr,
		  (double) diskjokey_sequencer_view->stop_box_x0, (double) diskjokey_sequencer_view->stop_box_y0,
		  (double) diskjokey_sequencer_view->stop_box_width, (double) diskjokey_sequencer_view->stop_box_height);
  cairo_stroke(cr);
  
  /* paint */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);

#ifndef __APPLE__
  pango_fc_font_map_cache_clear(pango_cairo_font_map_get_default());
#endif
}

/**
 * monothek_diskjokey_sequencer_view_new:
 *
 * Creates an #MonothekDiskjokeySequencerView
 *
 * Returns: a new #MonothekDiskjokeySequencerView
 *
 * Since: 1.0.0
 */
MonothekDiskjokeySequencerView*
monothek_diskjokey_sequencer_view_new()
{
  MonothekDiskjokeySequencerView *diskjokey_sequencer_view;

  diskjokey_sequencer_view = (MonothekDiskjokeySequencerView *) g_object_new(MONOTHEK_TYPE_DISKJOKEY_SEQUENCER_VIEW,
									     NULL);
  
  return(diskjokey_sequencer_view);
}
