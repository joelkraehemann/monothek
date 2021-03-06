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

#include <monothek/ui/controller/monothek_jukebox_track_controller.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <monothek/session/monothek_session_manager.h>
#include <monothek/session/monothek_session.h>

#include <monothek/ui/monothek_window.h>

#include <monothek/ui/model/monothek_jukebox_track_model.h>

#include <monothek/ui/view/monothek_jukebox_track_view.h>
#include <monothek/ui/view/monothek_jukebox_mode_view.h>
#include <monothek/ui/view/monothek_jukebox_no_test_view.h>
#include <monothek/ui/view/monothek_jukebox_end_view.h>

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <stdlib.h>

#include <monothek/i18n.h>

void monothek_jukebox_track_controller_class_init(MonothekJukeboxTrackControllerClass *jukebox_track_controller);
void monothek_jukebox_track_controller_connectable_interface_init(AgsConnectableInterface *connectable);
void monothek_jukebox_track_controller_init(MonothekJukeboxTrackController *jukebox_track_controller);
void monothek_jukebox_track_controller_finalize(GObject *gobject);

void monothek_jukebox_track_controller_connect(AgsConnectable *connectable);
void monothek_jukebox_track_controller_disconnect(AgsConnectable *connectable);

void monothek_jukebox_track_controller_real_progress(MonothekJukeboxTrackController *jukebox_track_controller,
						     gdouble value);
void monothek_jukebox_track_controller_real_test_time_expired(MonothekJukeboxTrackController *jukebox_track_controller);
void monothek_jukebox_track_controller_real_run(MonothekJukeboxTrackController *jukebox_track_controller,
						gboolean do_run);
void monothek_jukebox_track_controller_real_completed(MonothekJukeboxTrackController *jukebox_track_controller);

/**
 * SECTION:monothek_jukebox_track_controller
 * @short_description: The jukebox track controller object.
 * @title: MonothekJukeboxTrackController
 * @section_id:
 * @include: monothek/ui/controller/monothek_jukebox_track_controller.h
 *
 * #MonothekJukeboxTrackController is the MVC's jukebox track controller.
 */

enum{
  PROGRESS,
  TEST_TIME_EXPIRED,
  RUN,
  COMPLETED,
  LAST_SIGNAL,
};

static gpointer monothek_jukebox_track_controller_parent_class = NULL;
static AgsConnectableInterface* monothek_jukebox_track_controller_parent_connectable_interface;

static guint jukebox_track_controller_signals[LAST_SIGNAL];

GHashTable *monothek_jukebox_track_controller_progress_increase = NULL;

GType
monothek_jukebox_track_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType monothek_type_jukebox_track_controller = 0;

    static const GTypeInfo monothek_jukebox_track_controller_info = {
      sizeof (MonothekJukeboxTrackControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) monothek_jukebox_track_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (MonothekJukeboxTrackController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) monothek_jukebox_track_controller_init,
    };

    static const GInterfaceInfo monothek_connectable_interface_info = {
      (GInterfaceInitFunc) monothek_jukebox_track_controller_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    monothek_type_jukebox_track_controller = g_type_register_static(MONOTHEK_TYPE_CONTROLLER,
								    "MonothekJukeboxTrackController", &monothek_jukebox_track_controller_info,
								    0);

    g_type_add_interface_static(monothek_type_jukebox_track_controller,
				AGS_TYPE_CONNECTABLE,
				&monothek_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, monothek_type_jukebox_track_controller);
  }

  return g_define_type_id__volatile;
}

void
monothek_jukebox_track_controller_class_init(MonothekJukeboxTrackControllerClass *jukebox_track_controller)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  monothek_jukebox_track_controller_parent_class = g_type_class_peek_parent(jukebox_track_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) jukebox_track_controller;

  gobject->finalize = monothek_jukebox_track_controller_finalize;

  /* MonothekJukeboxTrackController */
  jukebox_track_controller->progress = monothek_jukebox_track_controller_real_progress;
  jukebox_track_controller->test_time_expired = monothek_jukebox_track_controller_real_test_time_expired;
  jukebox_track_controller->run = monothek_jukebox_track_controller_real_run;
  jukebox_track_controller->completed = monothek_jukebox_track_controller_real_completed;

  /* signals */
  /**
   * MonothekJukeboxTrackController::progress:
   * @jukebox_track_controller: the #MonothekJukeboxTrackController
   * @value: a gdouble ranging from 0.0 to 1.0
   *
   * The ::progress signal notifies about progress.
   *
   * Since: 1.0.0
   */
  jukebox_track_controller_signals[PROGRESS] =
    g_signal_new("progress",
		 G_TYPE_FROM_CLASS(jukebox_track_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(MonothekJukeboxTrackControllerClass, progress),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__DOUBLE,
		 G_TYPE_NONE, 1,
		 G_TYPE_DOUBLE);

  /**
   * MonothekJukeboxTrackController::test-time-expired:
   * @jukebox_track_controller: the #MonothekJukeboxTrackController
   *
   * The ::test-time-expired signal notifies about test time expired.
   *
   * Since: 1.0.0
   */
  jukebox_track_controller_signals[TEST_TIME_EXPIRED] =
    g_signal_new("test-time-expired",
		 G_TYPE_FROM_CLASS(jukebox_track_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(MonothekJukeboxTrackControllerClass, test_time_expired),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * MonothekJukeboxTrackController::run:
   * @jukebox_track_controller: the #MonothekJukeboxTrackController
   * @do_run: if %TRUE start playback, else if %FALSE stop playback
   *
   * The ::run signal notifies about start/stop playback.
   *
   * Since: 1.0.0
   */
  jukebox_track_controller_signals[RUN] =
    g_signal_new("run",
		 G_TYPE_FROM_CLASS(jukebox_track_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(MonothekJukeboxTrackControllerClass, run),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__BOOLEAN,
		 G_TYPE_NONE, 1,
		 G_TYPE_BOOLEAN);

  /**
   * MonothekJukeboxTrackController::completed:
   * @jukebox_track_controller: the #MonothekJukeboxTrackController
   *
   * The ::completed signal notifies about terminating playback.
   *
   * Since: 1.0.0
   */
  jukebox_track_controller_signals[COMPLETED] =
    g_signal_new("completed",
		 G_TYPE_FROM_CLASS(jukebox_track_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(MonothekJukeboxTrackControllerClass, completed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
monothek_jukebox_track_controller_connectable_interface_init(AgsConnectableInterface *connectable)
{  
  monothek_jukebox_track_controller_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = monothek_jukebox_track_controller_connect;
  connectable->disconnect = monothek_jukebox_track_controller_disconnect;
}

void
monothek_jukebox_track_controller_init(MonothekJukeboxTrackController *jukebox_track_controller)
{
  if(monothek_jukebox_track_controller_progress_increase == NULL){
    monothek_jukebox_track_controller_progress_increase = g_hash_table_new_full(g_direct_hash, g_direct_equal,
										NULL,
										NULL);
  }

  jukebox_track_controller->start_time = (struct timespec *) malloc(sizeof(struct timespec));
  jukebox_track_controller->start_time->tv_sec = 0;
  jukebox_track_controller->start_time->tv_nsec = 0;
  
  jukebox_track_controller->timer = (struct timespec *) malloc(sizeof(struct timespec));
  jukebox_track_controller->timer->tv_sec = 0;
  jukebox_track_controller->timer->tv_nsec = 0;
}

void
monothek_jukebox_track_controller_finalize(GObject *gobject)
{
  MonothekJukeboxTrackController *jukebox_track_controller;

  jukebox_track_controller = (MonothekJukeboxTrackController *) gobject;
  
  /* call parent */
  G_OBJECT_CLASS(monothek_jukebox_track_controller_parent_class)->finalize(gobject);
}

void
monothek_jukebox_track_controller_connect(AgsConnectable *connectable)
{
  MonothekJukeboxTrackController *jukebox_track_controller;

  jukebox_track_controller = MONOTHEK_JUKEBOX_TRACK_CONTROLLER(connectable);

  if(monothek_controller_test_flags(jukebox_track_controller, MONOTHEK_CONTROLLER_CONNECTED)){
    return;
  }

  monothek_jukebox_track_controller_parent_connectable_interface->connect(connectable);

  /* progress timeout - add */
  g_hash_table_insert(monothek_jukebox_track_controller_progress_increase,
		      jukebox_track_controller, monothek_jukebox_track_controller_progress_increase_timeout);
  
  g_timeout_add(1000 / 30,
		(GSourceFunc) monothek_jukebox_track_controller_progress_increase_timeout,
		(gpointer) jukebox_track_controller);

  /* start playback */
  monothek_jukebox_track_controller_run(jukebox_track_controller,
					TRUE);
}

void
monothek_jukebox_track_controller_disconnect(AgsConnectable *connectable)
{
  MonothekJukeboxTrackController *jukebox_track_controller;

  jukebox_track_controller = MONOTHEK_JUKEBOX_TRACK_CONTROLLER(connectable);

  if(!monothek_controller_test_flags(jukebox_track_controller, MONOTHEK_CONTROLLER_CONNECTED)){
    return;
  }

  monothek_jukebox_track_controller_parent_connectable_interface->disconnect(connectable);

  /* progress timeout - remove */
  g_hash_table_remove(monothek_jukebox_track_controller_progress_increase,
		      jukebox_track_controller);
}

void
monothek_jukebox_track_controller_real_progress(MonothekJukeboxTrackController *jukebox_track_controller,
						gdouble value)
{
  MonothekJukeboxTrackView *jukebox_track_view;
  
  MonothekSessionManager *session_manager;
  MonothekSession *session;

  GValue *jukebox_mode;

  g_object_get(jukebox_track_controller,
	       "view", &jukebox_track_view,
	       NULL);

  gtk_adjustment_set_value(jukebox_track_view->progress,
			   value);

  /* find session */
  session_manager = monothek_session_manager_get_instance();
  session = monothek_session_manager_find_session(session_manager,
						  MONOTHEK_SESSION_DEFAULT_SESSION);

  /* set jukebox mode - test */
  jukebox_mode = g_hash_table_lookup(session->value,
				     "jukebox-mode");

  if(!g_strcmp0("play",
		g_value_get_string(jukebox_mode))){
    if(value == 1.0){
      monothek_jukebox_track_controller_run(jukebox_track_controller,
					    FALSE);
    }
  }else{
    if(jukebox_track_controller->timer->tv_sec > MONOTHEK_JUKEBOX_TRACK_CONTROLLER_TEST_TIME_SEC){
      monothek_jukebox_track_controller_run(jukebox_track_controller,
					    FALSE);
    }
  }
}

/**
 * monothek_jukebox_track_controller_progress:
 * @jukebox_track_controller: the #MonothekJukeboxTrackController
 * @value: a gdouble ranging from 0.0 to 1.0
 * 
 * Notify about progress.
 * 
 * Since: 1.0.0
 */
void
monothek_jukebox_track_controller_progress(MonothekJukeboxTrackController *jukebox_track_controller,
					   gdouble value)
{
  g_return_if_fail(MONOTHEK_IS_JUKEBOX_TRACK_CONTROLLER(jukebox_track_controller));
  
  g_object_ref((GObject *) jukebox_track_controller);
  g_signal_emit(G_OBJECT(jukebox_track_controller),
		jukebox_track_controller_signals[PROGRESS], 0,
		value);
  g_object_unref((GObject *) jukebox_track_controller);
}

void
monothek_jukebox_track_controller_real_test_time_expired(MonothekJukeboxTrackController *jukebox_track_controller)
{
  monothek_jukebox_track_controller_run(jukebox_track_controller,
					FALSE);
}

/**
 * monothek_jukebox_track_controller_test_time_expired:
 * @jukebox_track_controller: the #MonothekJukeboxTrackController
 * 
 * Test time of track expired.
 * 
 * Since: 1.0.0
 */
void
monothek_jukebox_track_controller_test_time_expired(MonothekJukeboxTrackController *jukebox_track_controller)
{
  g_return_if_fail(MONOTHEK_IS_JUKEBOX_TRACK_CONTROLLER(jukebox_track_controller));
  
  g_object_ref((GObject *) jukebox_track_controller);
  g_signal_emit(G_OBJECT(jukebox_track_controller),
		jukebox_track_controller_signals[TEST_TIME_EXPIRED], 0);
  g_object_unref((GObject *) jukebox_track_controller);
}

void
monothek_jukebox_track_controller_real_run(MonothekJukeboxTrackController *jukebox_track_controller,
					   gboolean do_run)
{
  if(do_run){
#ifdef __APPLE__
    clock_serv_t cclock;
    mach_timespec_t mts;
#endif

#ifdef __APPLE__
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    
    jukebox_track_controller->start_time->tv_sec = mts.tv_sec;
    jukebox_track_controller->start_time->tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_MONOTONIC, jukebox_track_controller->start_time);
#endif

    jukebox_track_controller->timer->tv_sec = 0;
    jukebox_track_controller->timer->tv_nsec = 0;

    //TODO:JK: implement me
  }else{
    //TODO:JK: implement me

    monothek_jukebox_track_controller_completed(jukebox_track_controller);
  }
}

/**
 * monothek_jukebox_track_controller_run:
 * @jukebox_track_controller: the #MonothekJukeboxTrackController
 * @do_run: if %TRUE start playback, otherwise %FALSE stop playback
 * 
 * Start/stop playback.
 * 
 * Since: 1.0.0
 */
void
monothek_jukebox_track_controller_run(MonothekJukeboxTrackController *jukebox_track_controller,
				      gboolean do_run)
{
  g_return_if_fail(MONOTHEK_IS_JUKEBOX_TRACK_CONTROLLER(jukebox_track_controller));
  
  g_object_ref((GObject *) jukebox_track_controller);
  g_signal_emit(G_OBJECT(jukebox_track_controller),
		jukebox_track_controller_signals[RUN], 0,
		do_run);
  g_object_unref((GObject *) jukebox_track_controller);
}

void
monothek_jukebox_track_controller_real_completed(MonothekJukeboxTrackController *jukebox_track_controller)
{
  MonothekWindow *window;
  MonothekJukeboxModeView *view;

  MonothekSessionManager *session_manager;
  MonothekSession *session;

  GValue *jukebox_mode;

  /* change view */
  g_object_get(jukebox_track_controller,
	       "view", &view,
	       NULL);

  window = gtk_widget_get_ancestor(view,
				   MONOTHEK_TYPE_WINDOW);

  /* find session */
  session_manager = monothek_session_manager_get_instance();
  session = monothek_session_manager_find_session(session_manager,
						  MONOTHEK_SESSION_DEFAULT_SESSION);

  /* set jukebox mode - test */
  jukebox_mode = g_hash_table_lookup(session->value,
				     "jukebox-mode");

  if(!g_strcmp0("play",
		g_value_get_string(jukebox_mode))){
    monothek_window_change_view(window,
				MONOTHEK_TYPE_JUKEBOX_END_VIEW, G_TYPE_NONE);
  }else{
    guint test_count;
    
    GValue *jukebox_test_count;

    jukebox_test_count = g_hash_table_lookup(session->value,
					     "jukebox-test-count");

    test_count = 0;
    
    if(jukebox_test_count == NULL){
      jukebox_test_count = g_new0(GValue,
				  1);
      g_value_init(jukebox_test_count,
		   G_TYPE_UINT);

      g_hash_table_insert(session->value,
			  "jukebox-test-count", jukebox_test_count);
    }else{
      test_count = g_value_get_uint(jukebox_test_count);
    }
    
    test_count++;
    g_value_set_uint(jukebox_test_count,
		     test_count);
    
    if(test_count < 3){
      monothek_window_change_view(window,
 				  MONOTHEK_TYPE_JUKEBOX_MODE_VIEW, G_TYPE_NONE);
    }else{
      monothek_window_change_view(window,
				  MONOTHEK_TYPE_JUKEBOX_NO_TEST_VIEW, G_TYPE_NONE);
    }
  }
}

/**
 * monothek_jukebox_track_controller_completed:
 * @jukebox_track_controller: the #MonothekJukeboxTrackController
 * 
 * Termintating playback.
 * 
 * Since: 1.0.0
 */
void
monothek_jukebox_track_controller_completed(MonothekJukeboxTrackController *jukebox_track_controller)
{
  g_return_if_fail(MONOTHEK_IS_JUKEBOX_TRACK_CONTROLLER(jukebox_track_controller));
  
  g_object_ref((GObject *) jukebox_track_controller);
  g_signal_emit(G_OBJECT(jukebox_track_controller),
		jukebox_track_controller_signals[COMPLETED], 0);
  g_object_unref((GObject *) jukebox_track_controller);
}

/**
 * ags_line_indicator_increase_timeout:
 * @gobject: the gobject
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 1.0.0
 */
gboolean
monothek_jukebox_track_controller_progress_increase_timeout(GObject *gobject)
{
  MonothekJukeboxTrackController *jukebox_track_controller;

  jukebox_track_controller = gobject;
  
  if(g_hash_table_lookup(monothek_jukebox_track_controller_progress_increase,
			 gobject) != NULL){
    MonothekJukeboxTrackModel *jukebox_track_model;

#ifdef __APPLE__
    clock_serv_t cclock;
    mach_timespec_t mts;
#endif

    struct timespec *duration;
    struct timespec time_now;
    
    gdouble value;
    
    g_object_get(jukebox_track_controller,
		 "model", &jukebox_track_model,
		 NULL);

    g_object_get(jukebox_track_model,
		 "duration", &duration,
		 NULL);
    
#ifdef __APPLE__
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    
    time_now.tv_sec = mts.tv_sec;
    time_now.tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_MONOTONIC, &time_now);
#endif

    /* calculate timer */
    if(time_now.tv_nsec >= jukebox_track_controller->start_time->tv_nsec){
      jukebox_track_controller->timer->tv_sec = time_now.tv_sec - jukebox_track_controller->start_time->tv_sec;
      jukebox_track_controller->timer->tv_nsec = time_now.tv_nsec - jukebox_track_controller->start_time->tv_nsec;
    }else{
      jukebox_track_controller->timer->tv_sec = time_now.tv_sec - jukebox_track_controller->start_time->tv_sec - 1;
      jukebox_track_controller->timer->tv_nsec = NSEC_PER_SEC - jukebox_track_controller->start_time->tv_nsec + time_now.tv_sec;
    }

    /* calculate progress */
    if(duration->tv_sec > 0){
      if(duration->tv_sec < jukebox_track_controller->timer->tv_sec){
	value = 1.0;
      }else{
	value = 1.0 / duration->tv_sec * jukebox_track_controller->timer->tv_sec;
      }
    }else{
      value = 0.0;
    }
    
    monothek_jukebox_track_controller_progress(jukebox_track_controller,
					       value);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * monothek_jukebox_track_controller_new:
 *
 * Creates an #MonothekJukeboxTrackController
 *
 * Returns: a new #MonothekJukeboxTrackController
 *
 * Since: 1.0.0
 */
MonothekJukeboxTrackController*
monothek_jukebox_track_controller_new()
{
  MonothekJukeboxTrackController *jukebox_track_controller;

  jukebox_track_controller = (MonothekJukeboxTrackController *) g_object_new(MONOTHEK_TYPE_JUKEBOX_TRACK_CONTROLLER,
									     NULL);
  
  return(jukebox_track_controller);
}
