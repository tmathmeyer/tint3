#include <libnotify/notify.h>

#include "confparse.h"
#include "tint3.h"
#include "vdesk.h"
#include "system.h"
#include "weather.h"
#include "netinfo.h"

#define PARSEBLOCK(x, c) do{block*__b__=(x);__extension__(c);}while(0)
#define ID(z) else if(!strcmp(__b__->id, (#z)))

dlist *questions(baritem *meh) {
    (void)meh;
    return dlist_new();
}

void infer_type(block *conf_inf, baritem *ipl) {
    ipl->update = &questions;
    if (ipl->shell) {
        ipl->click = &shell_exec;
    }

    PARSEBLOCK(conf_inf, {if (0);        
        ID(workspace) {
            spawn_vdesk_thread(ipl);
            ipl->update = NULL;
            ipl->elements = get_desktops_info(ipl);
        }

        ID(clock) {
            ipl->update = &get_time_format;
            set_timeout(ipl);
        }

        ID(active) {
            if (!strncmp(conf_inf->source, "window_title", 12)) {
                ipl->update = &get_active_window_name;
            }
        }

        ID(text) {
            ipl->update = &get_plain_text;
        }

        ID(weather) {
            spawn_weather_thread(ipl);
            ipl->update = NULL;
            ipl->elements = get_weather(ipl);
        }

        ID(scale) {
            if (!strncmp(conf_inf->source, "battery", 7)) {
                ipl->update = &get_battery;
            } else if (!strncmp(conf_inf->source, "alsa", 4)) {
                ipl->update = &get_volume_level;
            }
        }

        ID(graph) {
            if (!strncmp(conf_inf->source, "network", 7)) {
                ipl->update = &get_net_graph;
            } else if (!strncmp(conf_inf->source, "group", 5)) {
            }
        }

        ID(shell) {
            ipl->update=&shell_cmd; 
        }

        else {
            goto precompile;
        }
    });
    return;
precompile:
    (void) "precomile target for different output logging";
#ifdef LIBNOTIFY
    puts(conf_inf->name);
    notify_init ("tint3 config error");
    NotifyNotification *Hello =
        notify_notification_new(
                "tint3 config error",
                "you seem to be using an invalid ID:",
                conf_inf->name);
    notify_notification_show (Hello, NULL);
    g_object_unref(G_OBJECT(Hello));
    notify_uninit();
#endif
}
