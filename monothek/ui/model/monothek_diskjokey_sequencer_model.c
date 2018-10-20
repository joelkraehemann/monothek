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

#include <monothek/ui/model/monothek_diskjokey_sequencer_model.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <stdlib.h>

#include <monothek/i18n.h>

void monothek_diskjokey_sequencer_model_class_init(MonothekDiskjokeySequencerModelClass *diskjokey_sequencer_model);
void monothek_diskjokey_sequencer_model_init(MonothekDiskjokeySequencerModel *diskjokey_sequencer_model);
void monothek_diskjokey_sequencer_model_set_property(GObject *gobject,
						     guint prop_id,
						     const GValue *value,
						     GParamSpec *param_spec);
void monothek_diskjokey_sequencer_model_get_property(GObject *gobject,
						     guint prop_id,
						     GValue *value,
						     GParamSpec *param_spec);
void monothek_diskjokey_sequencer_model_finalize(GObject *gobject);

/**
 * SECTION:monothek_diskjokey_sequencer_model
 * @short_description: The start model object.
 * @title: MonothekDiskjokeySequencerModel
 * @section_id:
 * @include: monothek/ui/model/monothek_diskjokey_sequencer_model.h
 *
 * #MonothekDiskjokeySequencerModel is the MVC's sequencer model.
 */

enum{
  PROP_0,
};

static gpointer monothek_diskjokey_sequencer_model_parent_class = NULL;

GType
monothek_diskjokey_sequencer_model_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType monothek_type_diskjokey_sequencer_model = 0;

    static const GTypeInfo monothek_diskjokey_sequencer_model_info = {
      sizeof (MonothekDiskjokeySequencerModelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) monothek_diskjokey_sequencer_model_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (MonothekDiskjokeySequencerModel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) monothek_diskjokey_sequencer_model_init,
    };

    monothek_type_diskjokey_sequencer_model = g_type_register_static(G_TYPE_OBJECT,
								     "MonothekDiskjokeySequencerModel", &monothek_diskjokey_sequencer_model_info,
								     0);

    g_once_init_leave(&g_define_type_id__volatile, monothek_type_diskjokey_sequencer_model);
  }

  return g_define_type_id__volatile;
}

void
monothek_diskjokey_sequencer_model_class_init(MonothekDiskjokeySequencerModelClass *diskjokey_sequencer_model)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  monothek_diskjokey_sequencer_model_parent_class = g_type_class_peek_parent(diskjokey_sequencer_model);

  /* GObjectClass */
  gobject = (GObjectClass *) diskjokey_sequencer_model;

  gobject->set_property = monothek_diskjokey_sequencer_model_set_property;
  gobject->get_property = monothek_diskjokey_sequencer_model_get_property;

  gobject->finalize = monothek_diskjokey_sequencer_model_finalize;

  /* properties */

  /* MonothekModel */
}

void
monothek_diskjokey_sequencer_model_init(MonothekDiskjokeySequencerModel *diskjokey_sequencer_model)
{
  //TODO:JK: implement me
}

void
monothek_diskjokey_sequencer_model_set_property(GObject *gobject,
						guint prop_id,
						const GValue *value,
						GParamSpec *param_spec)
{
  MonothekDiskjokeySequencerModel *diskjokey_sequencer_model;

  diskjokey_sequencer_model = MONOTHEK_DISKJOKEY_SEQUENCER_MODEL(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
monothek_diskjokey_sequencer_model_get_property(GObject *gobject,
						guint prop_id,
						GValue *value,
						GParamSpec *param_spec)
{
  MonothekDiskjokeySequencerModel *diskjokey_sequencer_model;

  diskjokey_sequencer_model = MONOTHEK_DISKJOKEY_SEQUENCER_MODEL(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
monothek_diskjokey_sequencer_model_finalize(GObject *gobject)
{
  MonothekDiskjokeySequencerModel *diskjokey_sequencer_model;

  diskjokey_sequencer_model = (MonothekDiskjokeySequencerModel *) gobject;
  
  /* call parent */
  G_OBJECT_CLASS(monothek_diskjokey_sequencer_model_parent_class)->finalize(gobject);
}

/**
 * monothek_diskjokey_sequencer_model_new:
 *
 * Creates an #MonothekDiskjokeySequencerModel
 *
 * Returns: a new #MonothekDiskjokeySequencerModel
 *
 * Since: 1.0.0
 */
MonothekDiskjokeySequencerModel*
monothek_diskjokey_sequencer_model_new()
{
  MonothekDiskjokeySequencerModel *diskjokey_sequencer_model;

  diskjokey_sequencer_model = (MonothekDiskjokeySequencerModel *) g_object_new(MONOTHEK_TYPE_DISKJOKEY_SEQUENCER_MODEL,
									       NULL);
  
  return(diskjokey_sequencer_model);
}
