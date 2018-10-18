/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2018 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <pango/pango.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#ifndef __APPLE__
#include <pango/pangofc-fontmap.h>
#endif

#include <sys/types.h>
#include <pwd.h>

#include <sys/mman.h>

#include <stdbool.h>

#include <ags/i18n.h>

void monothek_application_context_signal_handler(int signr);
static void monothek_application_context_signal_cleanup();

void monothek_application_context_class_init(MonothekApplicationContextClass *xorg_application_context);
void monothek_application_context_connectable_interface_init(AgsConnectableInterface *connectable);
void monothek_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider);
void monothek_application_context_sound_provider_interface_init(AgsSoundProviderInterface *sound_provider);
void monothek_application_context_init(MonothekApplicationContext *xorg_application_context);
void monothek_application_context_set_property(GObject *gobject,
					       guint prop_id,
					       const GValue *value,
					       GParamSpec *param_spec);
void monothek_application_context_get_property(GObject *gobject,
					       guint prop_id,
					       GValue *value,
					       GParamSpec *param_spec);
void monothek_application_context_connect(AgsConnectable *connectable);
void monothek_application_context_disconnect(AgsConnectable *connectable);
AgsThread* monothek_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider);
AgsThread* monothek_application_context_get_task_thread(AgsConcurrencyProvider *concurrency_provider);
AgsThreadPool* monothek_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider);
GList* monothek_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider);
void monothek_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
					     GList *worker);
GList* monothek_application_context_get_soundcard(AgsSoundProvider *sound_provider);
void monothek_application_context_set_soundcard(AgsSoundProvider *sound_provider,
						GList *soundcard);
GObject* monothek_application_context_get_default_soundcard_thread(AgsSoundProvider *sound_provider);
void monothek_application_context_set_default_soundcard_thread(AgsSoundProvider *sound_provider,
							       GObject *soundcard_thread);
GList* monothek_application_context_get_sequencer(AgsSoundProvider *sound_provider);
void monothek_application_context_set_sequencer(AgsSoundProvider *sound_provider,
						GList *sequencer);
GList* monothek_application_context_get_sound_server(AgsSoundProvider *sound_provider);
GList* monothek_application_context_get_audio(AgsSoundProvider *sound_provider);
void monothek_application_context_set_audio(AgsSoundProvider *sound_provider,
					    GList *soundcard);

void monothek_application_context_dispose(GObject *gobject);
void monothek_application_context_finalize(GObject *gobject);

void monothek_application_context_load_config(AgsApplicationContext *application_context);

void monothek_application_context_prepare(AgsApplicationContext *application_context);
void monothek_application_context_setup(AgsApplicationContext *application_context);

void monothek_application_context_register_types(AgsApplicationContext *application_context);

void monothek_application_context_quit(AgsApplicationContext *application_context);

void monothek_application_context_launch(AgsFileLaunch *launch, MonothekApplicationContext *application_context);

/**
 * SECTION:monothek_application_context
 * @short_description: The xorg application context
 * @title: MonothekApplicationContext
 * @section_id:
 * @include: ags/X/monothek_application_context.h
 *
 * #MonothekApplicationContext is a application context providing
 * the main window and sets up a functional audio layer.
 */

enum{
  PROP_0,
  PROP_WINDOW,
};

static gpointer monothek_application_context_parent_class = NULL;
static AgsConnectableInterface* monothek_application_context_parent_connectable_interface;

MonothekApplicationContext *monothek_application_context;
volatile gboolean ags_show_start_animation;

extern AgsApplicationContext *ags_application_context;

//TODO:JK: implement get functions
#ifndef AGS_USE_TIMER
static sigset_t ags_wait_mask;
struct sigaction ags_sigact;
#else
static sigset_t ags_timer_mask;

struct sigaction ags_sigact_timer;

struct sigevent ags_sev_timer;
struct itimerspec its;
#endif

GType
monothek_application_context_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_xorg_application_context = 0;

    static const GTypeInfo monothek_application_context_info = {
      sizeof (MonothekApplicationContextClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) monothek_application_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (MonothekApplicationContext),
      0,    /* n_preallocs */
      (GInstanceInitFunc) monothek_application_context_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) monothek_application_context_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_concurrency_provider_interface_info = {
      (GInterfaceInitFunc) monothek_application_context_concurrency_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sound_provider_interface_info = {
      (GInterfaceInitFunc) monothek_application_context_sound_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_xorg_application_context = g_type_register_static(AGS_TYPE_APPLICATION_CONTEXT,
							       "MonothekApplicationContext",
							       &monothek_application_context_info,
							       0);

    g_type_add_interface_static(ags_type_xorg_application_context,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_xorg_application_context,
				AGS_TYPE_CONCURRENCY_PROVIDER,
				&ags_concurrency_provider_interface_info);

    g_type_add_interface_static(ags_type_xorg_application_context,
				AGS_TYPE_SOUND_PROVIDER,
				&ags_sound_provider_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_xorg_application_context);
  }

  return g_define_type_id__volatile;
}

#ifndef AGS_USE_TIMER
void
monothek_application_context_signal_handler(int signr)
{
  if(signr == SIGINT){
    //TODO:JK: do backup
    
    exit(-1);
  }else{
    sigemptyset(&(ags_sigact.sa_mask));
    
    //    if(signr == AGS_ASYNC_QUEUE_SIGNAL_HIGH){
    // pthread_yield();
    //    }
  }
}

static void
monothek_application_context_signal_cleanup()
{
  sigemptyset(&(ags_sigact.sa_mask));
}
#endif

#ifdef AGS_USE_TIMER
void
monothek_application_context_signal_handler_timer(int sig, siginfo_t *si, void *uc)
{
  pthread_mutex_lock(AGS_THREAD(ags_application_context->main_loop)->timer_mutex);

  g_atomic_int_set(&(AGS_THREAD(ags_application_context->main_loop)->timer_expired),
		   TRUE);
  
  if(AGS_THREAD(ags_application_context->main_loop)->timer_wait){
    pthread_cond_signal(AGS_THREAD(ags_application_context->main_loop)->timer_cond);
  }
    
  pthread_mutex_unlock(AGS_THREAD(ags_application_context->main_loop)->timer_mutex);
  //  signal(sig, SIG_IGN);
}
#endif

void
monothek_application_context_class_init(MonothekApplicationContextClass *xorg_application_context)
{
  GObjectClass *gobject;
  AgsApplicationContextClass *application_context;
  GParamSpec *param_spec;

  monothek_application_context_parent_class = g_type_class_peek_parent(xorg_application_context);

  /* GObjectClass */
  gobject = (GObjectClass *) xorg_application_context;

  gobject->set_property = monothek_application_context_set_property;
  gobject->get_property = monothek_application_context_get_property;

  gobject->dispose = monothek_application_context_dispose;
  gobject->finalize = monothek_application_context_finalize;

  /* properties */
  /**
   * MonothekApplicationContext:window:
   *
   * The assigned window.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("window",
				   i18n_pspec("window of xorg application context"),
				   i18n_pspec("The window which this xorg application context assigned to"),
				   AGS_TYPE_WINDOW,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WINDOW,
				  param_spec);

  /* MonothekApplicationContextClass */
  application_context = (AgsApplicationContextClass *) xorg_application_context;
  
  application_context->load_config = monothek_application_context_load_config;

  application_context->prepare = monothek_application_context_prepare;
  application_context->setup = monothek_application_context_setup;

  application_context->register_types = monothek_application_context_register_types;
  
  application_context->quit = monothek_application_context_quit;

  application_context->write = monothek_application_context_write;
  application_context->read = monothek_application_context_read;
}

void
monothek_application_context_connectable_interface_init(AgsConnectableInterface *connectable)
{
  monothek_application_context_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = monothek_application_context_connect;
  connectable->disconnect = monothek_application_context_disconnect;
}

void
monothek_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider)
{
  concurrency_provider->get_main_loop = monothek_application_context_get_main_loop;
  concurrency_provider->get_task_thread = monothek_application_context_get_task_thread;
  concurrency_provider->get_thread_pool = monothek_application_context_get_thread_pool;
  concurrency_provider->get_worker = monothek_application_context_get_worker;
  concurrency_provider->set_worker = monothek_application_context_set_worker;
}

void
monothek_application_context_sound_provider_interface_init(AgsSoundProviderInterface *sound_provider)
{
  sound_provider->get_soundcard = monothek_application_context_get_soundcard;
  sound_provider->set_soundcard = monothek_application_context_set_soundcard;

  sound_provider->get_default_soundcard_thread = monothek_application_context_get_default_soundcard_thread;
  sound_provider->set_default_soundcard_thread = monothek_application_context_set_default_soundcard_thread;

  sound_provider->get_sequencer = monothek_application_context_get_sequencer;
  sound_provider->set_sequencer = monothek_application_context_set_sequencer;

  sound_provider->get_sound_server = monothek_application_context_get_sound_server;

  sound_provider->get_audio = monothek_application_context_get_audio;
  sound_provider->set_audio = monothek_application_context_set_audio;
}

void
monothek_application_context_init(MonothekApplicationContext *xorg_application_context)
{
  AgsConfig *config;

  if(ags_application_context == NULL){
    ags_application_context = xorg_application_context;
  }
  
  /* fundamental instances */
  config = ags_config_get_instance();
  AGS_APPLICATION_CONTEXT(xorg_application_context)->config = config;
  g_object_ref(config);
  g_object_set(config,
	       "application-context", xorg_application_context,
	       NULL);

  AGS_APPLICATION_CONTEXT(xorg_application_context)->log = ags_log_get_instance();
  g_object_ref(AGS_APPLICATION_CONTEXT(xorg_application_context)->log);
  
  /* Xorg application context */
  xorg_application_context->thread_pool = NULL;

  xorg_application_context->polling_thread = NULL;

  xorg_application_context->worker = NULL;
  
  xorg_application_context->soundcard_thread = NULL;
  xorg_application_context->export_thread = NULL;

  xorg_application_context->gui_thread = NULL;

  xorg_application_context->soundcard = NULL;
  xorg_application_context->sequencer = NULL;

  xorg_application_context->sound_server = NULL;
  
  xorg_application_context->audio = NULL;

  xorg_application_context->window = NULL;
}

void
monothek_application_context_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec)
{
  MonothekApplicationContext *xorg_application_context;

  xorg_application_context = MONOTHEK_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_WINDOW:
    {
      AgsWindow *window;
      
      window = (AgsWindow *) g_value_get_object(value);

      if(window == xorg_application_context->window){
	return;
      }

      if(xorg_application_context->window != NULL){
	g_object_unref(xorg_application_context->window);
      }
      
      if(window != NULL){
	g_object_ref(G_OBJECT(window));
      }
      
      xorg_application_context->window = window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
monothek_application_context_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec)
{
  MonothekApplicationContext *xorg_application_context;

  xorg_application_context = MONOTHEK_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_WINDOW:
    {
      g_value_set_object(value, xorg_application_context->window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
monothek_application_context_connect(AgsConnectable *connectable)
{
  MonothekApplicationContext *xorg_application_context;

  GList *soundcard, *sequencer;
  
  xorg_application_context = MONOTHEK_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (AGS_APPLICATION_CONTEXT(xorg_application_context)->flags)) != 0){
    return;
  }

  monothek_application_context_parent_connectable_interface->connect(connectable);

  /* soundcard */
  soundcard = xorg_application_context->soundcard;

  while(soundcard != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(soundcard->data));

    soundcard = soundcard->next;
  }
  
  ags_connectable_connect(AGS_CONNECTABLE(xorg_application_context->window));
}

void
monothek_application_context_disconnect(AgsConnectable *connectable)
{
  MonothekApplicationContext *xorg_application_context;

  xorg_application_context = MONOTHEK_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (AGS_APPLICATION_CONTEXT(xorg_application_context)->flags)) == 0){
    return;
  }

  monothek_application_context_parent_connectable_interface->disconnect(connectable);
}

AgsThread*
monothek_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider)
{
  return((AgsThread *) AGS_APPLICATION_CONTEXT(concurrency_provider)->main_loop);
}

AgsThread*
monothek_application_context_get_task_thread(AgsConcurrencyProvider *concurrency_provider)
{
  return((AgsThread *) AGS_APPLICATION_CONTEXT(concurrency_provider)->task_thread);
}

AgsThreadPool*
monothek_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider)
{
  return(MONOTHEK_APPLICATION_CONTEXT(concurrency_provider)->thread_pool);
}

GList*
monothek_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider)
{
  return(MONOTHEK_APPLICATION_CONTEXT(concurrency_provider)->worker);
}

void
monothek_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
					GList *worker)
{
  MONOTHEK_APPLICATION_CONTEXT(concurrency_provider)->worker = worker;
}

GList*
monothek_application_context_get_soundcard(AgsSoundProvider *sound_provider)
{
  GList *soundcard;

  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get soundcard */
  pthread_mutex_lock(application_context_mutex);
  
  soundcard = g_list_copy(MONOTHEK_APPLICATION_CONTEXT(sound_provider)->soundcard);

  pthread_mutex_unlock(application_context_mutex);
  
  return(soundcard);
}

void
monothek_application_context_set_soundcard(AgsSoundProvider *sound_provider,
					   GList *soundcard)
{
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* set soundcard */
  pthread_mutex_lock(application_context_mutex);

  if(MONOTHEK_APPLICATION_CONTEXT(sound_provider)->soundcard != NULL){
    g_list_free(MONOTHEK_APPLICATION_CONTEXT(sound_provider)->soundcard);
  }

  MONOTHEK_APPLICATION_CONTEXT(sound_provider)->soundcard = soundcard;

  pthread_mutex_unlock(application_context_mutex);
}

GObject*
monothek_application_context_get_default_soundcard_thread(AgsSoundProvider *sound_provider)
{
  return((GObject *) MONOTHEK_APPLICATION_CONTEXT(sound_provider)->soundcard_thread);
}

void
monothek_application_context_set_default_soundcard_thread(AgsSoundProvider *sound_provider,
							  GObject *soundcard_thread)
{
  MONOTHEK_APPLICATION_CONTEXT(sound_provider)->soundcard_thread = (AgsThread *) soundcard_thread;
}

GList*
monothek_application_context_get_sequencer(AgsSoundProvider *sound_provider)
{
  GList *sequencer;

  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get sequencer */
  pthread_mutex_lock(application_context_mutex);
  
  sequencer = g_list_copy(MONOTHEK_APPLICATION_CONTEXT(sound_provider)->sequencer);

  pthread_mutex_unlock(application_context_mutex);
  
  return(sequencer);
}

void
monothek_application_context_set_sequencer(AgsSoundProvider *sound_provider,
					   GList *sequencer)
{
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* set sequencer */
  pthread_mutex_lock(application_context_mutex);

  if(MONOTHEK_APPLICATION_CONTEXT(sound_provider)->sequencer != NULL){
    g_list_free(MONOTHEK_APPLICATION_CONTEXT(sound_provider)->sequencer);
  }
  
  MONOTHEK_APPLICATION_CONTEXT(sound_provider)->sequencer = sequencer;

  pthread_mutex_unlock(application_context_mutex);
}

GList*
monothek_application_context_get_sound_server(AgsSoundProvider *sound_provider)
{
  GList *sound_server;

  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get sound server */
  pthread_mutex_lock(application_context_mutex);
  
  sound_server = g_list_copy(MONOTHEK_APPLICATION_CONTEXT(sound_provider)->sound_server);

  pthread_mutex_unlock(application_context_mutex);
  
  return(sound_server);
}

GList*
monothek_application_context_get_audio(AgsSoundProvider *sound_provider)
{
  GList *audio;

  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get audio */
  pthread_mutex_lock(application_context_mutex);
  
  audio = g_list_copy(MONOTHEK_APPLICATION_CONTEXT(sound_provider)->audio);

  pthread_mutex_unlock(application_context_mutex);
  
  return(audio);
}

void
monothek_application_context_set_audio(AgsSoundProvider *sound_provider,
				       GList *audio)
{
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* set audio */
  pthread_mutex_lock(application_context_mutex);

  if(MONOTHEK_APPLICATION_CONTEXT(sound_provider)->audio != NULL){
    g_list_free(MONOTHEK_APPLICATION_CONTEXT(sound_provider)->audio);
  }

  MONOTHEK_APPLICATION_CONTEXT(sound_provider)->audio = audio;

  pthread_mutex_unlock(application_context_mutex);
}

void
monothek_application_context_dispose(GObject *gobject)
{
  MonothekApplicationContext *xorg_application_context;

  GList *list;

  xorg_application_context = MONOTHEK_APPLICATION_CONTEXT(gobject);

  /* thread pool */
  if(xorg_application_context->thread_pool != NULL){
    g_object_unref(xorg_application_context->thread_pool);
    
    xorg_application_context->thread_pool = NULL;
  }

  /* polling thread */
  if(xorg_application_context->polling_thread != NULL){
    g_object_unref(xorg_application_context->polling_thread);

    xorg_application_context->polling_thread = NULL;
  }

  /* worker thread */
  if(xorg_application_context->worker != NULL){
    list = xorg_application_context->worker;

    while(list != NULL){
      g_object_run_dispose(list->data);
      
      list = list->next;
    }
    
    g_list_free_full(xorg_application_context->worker,
		     g_object_unref);

    xorg_application_context->worker = NULL;
  }
  
  /* soundcard and export thread */
  if(xorg_application_context->soundcard_thread != NULL){
    g_object_unref(xorg_application_context->soundcard_thread);

    xorg_application_context->soundcard_thread = NULL;
  }

  if(xorg_application_context->export_thread != NULL){
    g_object_unref(xorg_application_context->export_thread);

    xorg_application_context->export_thread = NULL;
  }

  /* soundcard */
  if(xorg_application_context->soundcard != NULL){
    list = xorg_application_context->soundcard;

    while(list != NULL){
      g_object_set(list->data,
		   "application-context", NULL,
		   NULL);

      list = list->next;
    }
    
    g_list_free_full(xorg_application_context->soundcard,
		     g_object_unref);

    xorg_application_context->soundcard = NULL;
  }

  /* window */
  if(xorg_application_context->window != NULL){
    g_object_set(xorg_application_context->window,
		 "application-context", NULL,
		 NULL);
    
    gtk_widget_destroy(xorg_application_context->window);

    xorg_application_context->window = NULL;
  }  
  
  /* call parent */
  G_OBJECT_CLASS(monothek_application_context_parent_class)->dispose(gobject);
}

void
monothek_application_context_finalize(GObject *gobject)
{
  MonothekApplicationContext *xorg_application_context;

  xorg_application_context = MONOTHEK_APPLICATION_CONTEXT(gobject);

  if(xorg_application_context->thread_pool != NULL){
    g_object_unref(xorg_application_context->thread_pool);
  }

  if(xorg_application_context->polling_thread != NULL){
    g_object_unref(xorg_application_context->polling_thread);
  }

  if(xorg_application_context->worker != NULL){
    g_list_free_full(xorg_application_context->worker,
		     g_object_unref);

    xorg_application_context->worker = NULL;
  }
  
  if(xorg_application_context->soundcard_thread != NULL){
    g_object_unref(xorg_application_context->soundcard_thread);
  }

  if(xorg_application_context->export_thread != NULL){
    g_object_unref(xorg_application_context->export_thread);
  }

  if(xorg_application_context->soundcard != NULL){
    g_list_free_full(xorg_application_context->soundcard,
		     g_object_unref);
  }
  
  if(xorg_application_context->window != NULL){
    gtk_widget_destroy(xorg_application_context->window);
  }
  
  /* call parent */
  G_OBJECT_CLASS(monothek_application_context_parent_class)->finalize(gobject);
}
void
monothek_application_context_load_config(AgsApplicationContext *application_context)
{
  //TODO:JK: implement me
}

void
monothek_application_context_prepare(AgsApplicationContext *application_context)
{
  MonothekApplicationContext *xorg_application_context;

  AgsThread *audio_loop, *polling_thread, *task_thread;
  AgsThreadPool *thread_pool;

  GList *start_queue;
  
  xorg_application_context = (MonothekApplicationContext *) application_context;

  /* call parent */
  //  AGS_APPLICATION_CONTEXT_CLASS(monothek_application_context_parent_class)->prepare(application_context);
  
  /* register types */
  ags_application_context_register_types(xorg_application_context);

  /*
   * fundamental thread setup
   */
  /* AgsAudioLoop */
  audio_loop =
    application_context->main_loop = ags_audio_loop_new((GObject *) NULL,
							(GObject *) xorg_application_context);
  g_object_set(xorg_application_context,
	       "main-loop", audio_loop,
	       NULL);

  g_object_ref(audio_loop);
  ags_connectable_connect(AGS_CONNECTABLE(audio_loop));

  /* AgsPollingThread */
  polling_thread = 
    xorg_application_context->polling_thread = ags_polling_thread_new();
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				(AgsThread *) polling_thread,
				TRUE, TRUE);
  
  /* AgsTaskThread */
  task_thread = 
    application_context->task_thread = (GObject *) ags_task_thread_new();
  thread_pool = AGS_TASK_THREAD(task_thread)->thread_pool;
  ags_main_loop_set_async_queue(AGS_MAIN_LOOP(audio_loop),
				task_thread);
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				(AgsThread *) task_thread,
				TRUE, TRUE);
  g_signal_connect(application_context->task_thread, "clear-cache",
		   G_CALLBACK(monothek_application_context_clear_cache), NULL);

  /* start engine */
  pthread_mutex_lock(audio_loop->start_mutex);
  
  start_queue = NULL;
  start_queue = g_list_prepend(start_queue,
			       polling_thread);
  start_queue = g_list_prepend(start_queue,
			       task_thread);

  g_atomic_pointer_set(&(audio_loop->start_queue),
		       start_queue);
  
  pthread_mutex_unlock(audio_loop->start_mutex);

  /* start audio loop and thread pool*/
  ags_thread_start(audio_loop);
  
  ags_thread_pool_start(thread_pool);

  /* wait for audio loop */
  pthread_mutex_lock(audio_loop->start_mutex);

  if(g_atomic_int_get(&(audio_loop->start_wait)) == TRUE){	
    g_atomic_int_set(&(audio_loop->start_done),
		     FALSE);
      
    while(g_atomic_int_get(&(audio_loop->start_wait)) == TRUE &&
	  g_atomic_int_get(&(audio_loop->start_done)) == FALSE){
      pthread_cond_wait(audio_loop->start_cond,
			audio_loop->start_mutex);
    }
  }
    
  pthread_mutex_unlock(audio_loop->start_mutex);

  /* start gui thread */
  gtk_main();
}

void
monothek_application_context_setup(AgsApplicationContext *application_context)
{
  MonothekApplicationContext *xorg_application_context;
  AgsWindow *window;

  AgsAudioLoop *audio_loop;
  GObject *soundcard;

  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;
  AgsMessageQueue *audio_message_queue;
  AgsThread *soundcard_thread;
  AgsThread *export_thread;
  AgsDestroyWorker *destroy_worker;
  AgsThread *main_loop;

  AgsLog *log;
  AgsConfig *config;

  GList *list;  
  
  struct passwd *pw;

#ifdef AGS_USE_TIMER
  timer_t *timer_id;
#endif

  gchar *soundcard_group;
  gchar *str;
  gchar *capability;
  
  uid_t uid;
  
  guint i;
  
  xorg_application_context = (MonothekApplicationContext *) application_context;

  audio_loop = AGS_APPLICATION_CONTEXT(xorg_application_context)->main_loop;

  config = ags_config_get_instance();

  /* call parent */
  //  AGS_APPLICATION_CONTEXT_CLASS(monothek_application_context_parent_class)->setup(application_context);

  g_object_get(application_context,
	       "main-loop", &main_loop,
	       NULL);

  log = ags_log_get_instance();

  atexit(monothek_application_context_signal_cleanup);

  /* Ignore interactive and job-control signals.  */
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
  signal(AGS_THREAD_RESUME_SIG, SIG_IGN);
  signal(AGS_THREAD_SUSPEND_SIG, SIG_IGN);

  ags_sigact.sa_handler = monothek_application_context_signal_handler;
  sigemptyset(&ags_sigact.sa_mask);
  ags_sigact.sa_flags = 0;
  sigaction(SIGINT, &ags_sigact, (struct sigaction *) NULL);
  sigaction(SA_RESTART, &ags_sigact, (struct sigaction *) NULL);
  
  /* message delivery */
  message_delivery = ags_message_delivery_get_instance();

  message_queue = ags_message_queue_new("libags");
  ags_message_delivery_add_queue(message_delivery,
				 message_queue);

  audio_message_queue = ags_message_queue_new("libags-audio");
  ags_message_delivery_add_queue(message_delivery,
				 audio_message_queue);

  /* AgsSoundcard */
  xorg_application_context->soundcard = NULL;
  soundcard = NULL;

  soundcard_group = g_strdup("soundcard");
  
  for(i = 0; ; i++){
    guint pcm_channels, buffer_size, samplerate, format;

    if(!g_key_file_has_group(config->key_file,
			     soundcard_group)){
      if(i == 0){
	g_free(soundcard_group);    
	soundcard_group = g_strdup_printf("%s-%d",
					  AGS_CONFIG_SOUNDCARD,
					  i);
    	
	continue;
      }else{
	break;
      }
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "backend");

    capability = ags_config_get_value(config,
				      soundcard_group,
				      "capability");

    is_output = TRUE;

    if(capability != NULL &&
       !g_ascii_strncasecmp(capability,
			    "capture",
			    8)){
      is_output = FALSE;
    }
    
    /* change soundcard */
    if(str != NULL){
      if(!g_ascii_strncasecmp(str,
			      "alsa",
			      5)){
	if(is_output){
	  soundcard = (GObject *) ags_devout_new((GObject *) xorg_application_context);
	  
	  AGS_DEVOUT(soundcard)->flags &= (~AGS_DEVOUT_OSS);
	  AGS_DEVOUT(soundcard)->flags |= AGS_DEVOUT_ALSA;
	}
      }else{
	g_warning(i18n("unknown soundcard backend - %s"), str);
	
	g_free(soundcard_group);    
	soundcard_group = g_strdup_printf("%s-%d",
					  AGS_CONFIG_SOUNDCARD,
					  i);
	
	continue;
      }
    }else{
      g_warning(i18n("unknown soundcard backend - NULL"));

      g_free(soundcard_group);    
      soundcard_group = g_strdup_printf("%s-%d",
					AGS_CONFIG_SOUNDCARD,
					i);
      
      continue;
    }
    
    if(xorg_application_context->soundcard == NULL){
      g_object_set(audio_loop,
		   "default-output-soundcard", G_OBJECT(soundcard),
		   NULL);
    }
    
    xorg_application_context->soundcard = g_list_append(xorg_application_context->soundcard,
							soundcard);
    g_object_ref(soundcard);

    /* device */
    str = ags_config_get_value(config,
			       soundcard_group,
			       "device");

    if(str != NULL){
      ags_soundcard_set_device(AGS_SOUNDCARD(soundcard),
			       str);
      g_free(str);
    }
    
    /* presets */
    pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
    buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    format = AGS_SOUNDCARD_DEFAULT_FORMAT;

    str = ags_config_get_value(config,
			       soundcard_group,
			       "pcm-channels");

    if(str != NULL){
      pcm_channels = g_ascii_strtoull(str,
				      NULL,
				      10);
      g_free(str);
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "buffer-size");

    if(str != NULL){
      buffer_size = g_ascii_strtoull(str,
				     NULL,
				     10);
      g_free(str);
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "samplerate");

    if(str != NULL){
      samplerate = g_ascii_strtoull(str,
				    NULL,
				    10);
      g_free(str);
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "format");

    if(str != NULL){
      format = g_ascii_strtoull(str,
				NULL,
				10);
      g_free(str);
    }

    ags_soundcard_set_presets(AGS_SOUNDCARD(soundcard),
			      pcm_channels,
			      samplerate,
			      buffer_size,
			      format);

    g_free(soundcard_group);    
    soundcard_group = g_strdup_printf("%s-%d",
				      AGS_CONFIG_SOUNDCARD,
				      i);
  }

  if(xorg_application_context->soundcard != NULL){
    soundcard = xorg_application_context->soundcard->data;
  }  

  g_free(soundcard_group);
  
  /* AgsWindow */
#ifdef AGS_WITH_QUARTZ
  g_object_new(GTKOSX_TYPE_APPLICATION,
	       NULL);
#endif
  window = g_object_new(AGS_TYPE_WINDOW,
			"soundcard", soundcard,
			"application-context", xorg_application_context,
			NULL);
  g_object_set(xorg_application_context,
	       "window", window,
	       NULL);

  gtk_window_set_default_size((GtkWindow *) window, 1920, 1080;

  ags_connectable_connect(AGS_CONNECTABLE(window));
  
  /* AgsSoundcardThread and AgsExportThread */
  xorg_application_context->soundcard_thread = NULL;
  list = xorg_application_context->soundcard;
    
  while(list != NULL){
    AgsNotifySoundcard *notify_soundcard;

    guint soundcard_capability;

    soundcard_capability = ags_soundcard_get_capability(AGS_SOUNDCARD(list->data));
    
    soundcard_thread = (AgsThread *) ags_soundcard_thread_new(list->data,
							      soundcard_capability);
    ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				  (AgsThread *) soundcard_thread,
				  TRUE, TRUE);

    /* notify soundcard and export thread */
    export_thread = NULL;
    
    //    if(soundcard_capability == AGS_SOUNDCARD_CAPABILITY_PLAYBACK){
      notify_soundcard = ags_notify_soundcard_new(soundcard_thread);
      AGS_TASK(notify_soundcard)->task_thread = application_context->task_thread;
    
      if(AGS_IS_DEVOUT(list->data)){
	AGS_DEVOUT(list->data)->notify_soundcard = notify_soundcard;
      }else if(AGS_IS_JACK_DEVOUT(list->data)){
	AGS_JACK_DEVOUT(list->data)->notify_soundcard = notify_soundcard;
      }else if(AGS_IS_PULSE_DEVOUT(list->data)){
	AGS_PULSE_DEVOUT(list->data)->notify_soundcard = notify_soundcard;
      }else if(AGS_IS_CORE_AUDIO_DEVOUT(list->data)){
	AGS_CORE_AUDIO_DEVOUT(list->data)->notify_soundcard = notify_soundcard;
      }else if(AGS_IS_DEVIN(list->data)){
	AGS_DEVIN(list->data)->notify_soundcard = notify_soundcard;
      }else if(AGS_IS_JACK_DEVIN(list->data)){
	AGS_JACK_DEVIN(list->data)->notify_soundcard = notify_soundcard;
      }else if(AGS_IS_PULSE_DEVIN(list->data)){
	AGS_PULSE_DEVIN(list->data)->notify_soundcard = notify_soundcard;
      }else if(AGS_IS_CORE_AUDIO_DEVIN(list->data)){
	AGS_CORE_AUDIO_DEVIN(list->data)->notify_soundcard = notify_soundcard;
      }

      ags_task_thread_append_cyclic_task(application_context->task_thread,
					 notify_soundcard);

      /* export thread */
      if(AGS_IS_DEVOUT(list->data) ||
	 AGS_IS_JACK_DEVOUT(list->data) ||
	 AGS_IS_PULSE_DEVOUT(list->data) ||
	 AGS_IS_CORE_AUDIO_DEVOUT(list->data)){
	export_thread = (AgsThread *) ags_export_thread_new(list->data,
							    NULL);
	ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				      (AgsThread *) export_thread,
				      TRUE, TRUE);
      }    

    /* default soundcard thread */
    if(xorg_application_context->soundcard_thread == NULL){
      xorg_application_context->soundcard_thread = soundcard_thread;
      g_object_ref(soundcard_thread);
    }

    /* default export thread */
    if(export_thread != NULL &&
       xorg_application_context->export_thread == NULL){
      xorg_application_context->export_thread = export_thread;
      g_object_ref(export_thread);
    }

    /* iterate */
    list = list->next;      
  }

  /* AgsWorkerThread */
  xorg_application_context->worker = NULL;

  /* AgsDestroyWorker */
  destroy_worker = ags_destroy_worker_new();
  g_object_ref(destroy_worker);
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				destroy_worker,
				TRUE, TRUE);
  xorg_application_context->worker = g_list_prepend(xorg_application_context->worker,
						    destroy_worker);
  ags_thread_start(destroy_worker);
  
  /* AgsThreadPool */
  xorg_application_context->thread_pool = AGS_TASK_THREAD(application_context->task_thread)->thread_pool;
}

void
monothek_application_context_register_types(AgsApplicationContext *application_context)
{
  ags_complex_get_type();

  /* */
  ags_connectable_get_type();

  /*  */
  ags_audio_loop_get_type();
  ags_soundcard_thread_get_type();
  ags_export_thread_get_type();

  /*  */
  ags_audio_file_get_type();
  ags_audio_file_link_get_type();

  /* register tasks */
  ags_cancel_audio_get_type();
  ags_cancel_channel_get_type();

  //TODO:JK: extend me
  
  /* register recalls */
  ags_generic_recall_channel_run_get_type();

  ags_play_audio_get_type();
  ags_play_channel_get_type();
  ags_play_channel_run_get_type();
  ags_play_channel_run_master_get_type();

  ags_stream_channel_get_type();
  ags_stream_channel_run_get_type();

  ags_loop_channel_get_type();
  ags_loop_channel_run_get_type();

  ags_copy_channel_get_type();
  ags_copy_channel_run_get_type();

  ags_volume_channel_get_type();
  ags_volume_channel_run_get_type();

  ags_peak_channel_get_type();
  ags_peak_channel_run_get_type();

  ags_recall_ladspa_get_type();
  ags_recall_ladspa_run_get_type();

  ags_recall_dssi_get_type();
  ags_recall_dssi_run_get_type();

  ags_recall_lv2_get_type();
  ags_recall_lv2_run_get_type();

  ags_delay_audio_get_type();
  ags_delay_audio_run_get_type();

  ags_count_beats_audio_get_type();
  ags_count_beats_audio_run_get_type();

  ags_copy_pattern_audio_get_type();
  ags_copy_pattern_audio_run_get_type();
  ags_copy_pattern_channel_get_type();
  ags_copy_pattern_channel_run_get_type();

  ags_buffer_channel_get_type();
  ags_buffer_channel_run_get_type();

  ags_play_notation_audio_get_type();
  ags_play_notation_audio_run_get_type();

  ags_route_dssi_audio_get_type();
  ags_route_dssi_audio_run_get_type();

  ags_route_lv2_audio_get_type();
  ags_route_lv2_audio_run_get_type();
  
  /* gui */
  //TODO:JK: move me
  ags_led_get_type();
  ags_indicator_get_type();
  ags_vindicator_get_type();
  ags_hindicator_get_type();
  ags_dial_get_type();
  ags_notebook_get_type();
  ags_piano_get_type();
}

void
monothek_application_context_quit(AgsApplicationContext *application_context)
{  
  gtk_main_quit();
}

MonothekApplicationContext*
monothek_application_context_new()
{
  MonothekApplicationContext *xorg_application_context;

  xorg_application_context = (MonothekApplicationContext *) g_object_new(AGS_TYPE_XORG_APPLICATION_CONTEXT,
									NULL);

  return(xorg_application_context);
}