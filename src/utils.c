/*
 * Copyright (C) 2014 Ted Meyer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <xcb/xcb.h>

#include "config.h"
#include "defaults.h"
#include "utils.h"
#include "lwxt.h"
#include "lwbi.h"






typedef struct glist{
    struct glist * next;
    char * name;
    graph * data;
} glist;

glist * graphs = NULL;

graph * get_named_graph(char * name) {
    glist * temp = graphs;
    while(temp) {
        if (!strncmp(name, temp -> name, strlen(name))) {
            return temp -> data;
        } temp = temp -> next;
    }
    glist * addition = malloc(sizeof(glist));
    addition -> name = name;
    addition -> data = make_new_graph();
    addition -> next = graphs;
    graphs = addition;
    return addition -> data;
}

unsigned long long old_down=0, old_up=0;
unsigned long net_tick = 0;
void update_network(char * interface) {
    if (time(NULL)-net_tick > 2) {
        time((time_t *)&net_tick);

        FILE * fp = fopen("/proc/net/dev", "r");
        if (fp == NULL) {
            return;
        }

        char * buffer = malloc(4096), * bt = buffer, temp = 0;
        while( (temp=fgetc(fp)) != EOF) {
            *(bt++) = temp;
        }
        fclose(fp);
        bt = strstr(buffer, interface);
        if (bt == 0) {
            free(buffer);
            return;
        }
        bt += (strlen(interface) + 2);

        unsigned long long up, down;
        sscanf(bt, "%llu %llu", &down, &up);

        int ud = up-old_up, dd = down-old_down;

        char * netname = calloc(0, strlen(interface) + 4);
        snprintf(netname, strlen(interface)+4, "%s up", interface);

        if (old_down != 0 && old_up != 0) {
            add_to_graph(dd, get_named_graph(netname));
            netname[strlen(interface)+1] = 'd';
            netname[strlen(interface)+2] = 'o';
            add_to_graph(ud, get_named_graph(netname));
        }

        old_down = down;
        old_up = up;

        free(buffer);
        free(netname);
    }
}

int eot = 0;
char * get_net_graph(baritem * item) {
    return graph_to_string(get_named_graph((item -> source)+8));
}

int get_number_of_desktops () {
    return get_x11_property(NET_NUMBER_DESKTOPS, _CARDINAL_);
}

int get_current_desktop () {
    return get_x11_property(NET_CURRENT_DESKTOP, _CARDINAL_);
}

// make this configurable with regex somehoe
char * get_desktops_info(baritem * source) {
    int numdesk = get_number_of_desktops();
    int curdesk = get_current_desktop();
    int swap = 0;

    int dsktplen = numdesk * 4 - 1;
    char * result = malloc(dsktplen);

    for(swap=0; swap < dsktplen; swap++) {
        int sqp = swap%4;
        result[swap] = sqp==3?' ':DESKTOP_DEFAULT[sqp];
    }

    result[curdesk*4 + 0] = DESKTOP_CURRENT[0];
    result[curdesk*4 + 1] = DESKTOP_CURRENT[1];
    result[curdesk*4 + 2] = DESKTOP_CURRENT[2];

    return result;
}

char * get_active_window_name(baritem * source) {
    char * window_title = malloc(256); // max displayed window size
    get_title(window_title, 256);
    return window_title;
}

char * get_time_format(baritem * item) {
    char exec[100] = {0};
    snprintf(exec, sizeof exec, "date +'%s'", item -> format);
    FILE * desc = popen(exec, "r");
    char * msg = calloc(0, 20);
    int msg_c = 0; char msg_s;
    if (desc) {
        while( (msg_s = fgetc(desc)) != '\n') {
            msg[msg_c++] = msg_s;
        }
        if (msg_c < 20) {
            msg[msg_c] = 0;
        }
        pclose(desc);
    }
    return msg;
}


unsigned long lastime = 0;
char * weather = NULL;
char * get_weather(baritem * item) {
    if (time(NULL)-lastime > 1800) {
        time((time_t *)&lastime);

        time((time_t*)&lastime);
        int weather_parse_size = 1024;
        char * weather_s = malloc(weather_parse_size);
        char * host = "weather.noaa.gov";
        char * url  = "/pub/data/observations/metar/decoded/" WEATHER_LOCATION ".TXT";

        if (!url_to_memory(weather_s, weather_parse_size, url, host, "208.59.215.33")) {
            return NULL;
        }

        char * temp = strstr(weather_s, "Temperature") + 13;
        char * humd = strstr(weather_s, "Dew Point") + 11;

        int temperature, humidity;
        sscanf(temp, "%i", &temperature);
        sscanf(humd, "%i", &humidity);

        if (weather != NULL) {
            free(weather);
        }
        if (weather_s != NULL) {
            free(weather_s);
        }
        weather = calloc(0, 8);
        snprintf(weather, 8, "%i/%i", temperature, humidity);
    }

    int length = strlen(weather);
    char * weather2 = malloc(length+1);
    snprintf(weather2, length+1, "%s", weather);
    return weather2;
}

char * get_battery(baritem * item) {
    char batt[5] = "BAT0";
    char c = (item -> source)[8];
    batt[3] = c;
    char * msg = calloc(0,11);
    int battery_percent = get_battery_percent(batt);
    snprintf(msg, 11, "batt%c:%i%%", c, battery_percent);

    // change color here
    return msg;
}

char * get_volume_level(baritem * item) {
    char * result = malloc(8);
    snprintf(result, 8, "<<VOL>>");
    return result;
}

char * bar_map = "▁▂▃▄▅▆▇";

void recalc_max(graph * gr) {
    int ctr = 0, i = 0;
    for(; i < GRAPHLENGTH; i++) {
        if ((gr -> graph)[i] > ctr) {
            ctr = (gr -> graph)[i];
        }
    }
    gr -> max = ctr;
}

void add_to_graph(int i, graph * gr) {
    int old = (gr -> graph)[gr -> start];
    (gr -> graph)[gr -> start] = i;
    if (i > gr -> max) {
        gr -> max = i;
    }
    if (old ==  gr -> max) {
        recalc_max(gr);
    }
    gr -> start = ((gr -> start)+1)%GRAPHLENGTH;
}

char * graph_to_string(graph * gr) {
    char * result = malloc(GRAPHLENGTH*3+1);
    int ctr = 0, i = 0;
    for(; i < GRAPHLENGTH*3+1; i++) {
        result[i] = 0;
    } i = 0;
    for(; i < GRAPHLENGTH; i++) {
        int val = (gr -> graph)[((gr -> start)+i)%GRAPHLENGTH]*7/((gr -> max)+1) + 1;
        if (val < 0) {
            val = 0;
        }
        result[ctr++] = bar_map[(val-1)*3+0];
        result[ctr++] = bar_map[(val-1)*3+1];
        result[ctr++] = bar_map[(val-1)*3+2];
    }
    result[GRAPHLENGTH*3] = 0;
    return result;
}

graph * make_new_graph() {
    graph * g = NULL;
    g = malloc(sizeof(graph));
    g -> start = 0;
    g -> max = 0;
    int i=0; for(; i<GRAPHLENGTH; i++) {
        (g->graph)[i] = 0;
    }
    return g;
}

int get_socket(int port_number, char* ip) {
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_number);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0) {
        perror ("inet_pton error occured");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
       perror("Error : Connect Failed");
       return -1;
    }

    return sockfd;
}

void host_to_ip(char *ptr, char* address) {
    char ** pptr;
    char str[INET6_ADDRSTRLEN];
    struct hostent * hptr;

    if ( (hptr = gethostbyname(ptr)) == NULL) {
        strcpy(address, "127.0.0.1"); // hit localhost
        return;
    }

    pptr = hptr->h_addr_list;
    for ( ; *pptr != NULL; pptr++) {
        strcpy( address,  inet_ntop(hptr->h_addrtype,
                       *pptr, str, sizeof(str)));
        return;
    }

    strcpy(address, "127.0.0.1");
}

char * generate_header(char * url, char * host) {
    char * header =
        "GET %s "
        "HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Cache-Control: no-cache\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
        "Pragma: no-cache\r\n"
        "Accept-Encoding: gzip,deflate,sdch\r\n"
        "Accept-Language: en-US,en;q=0.8,ru;q=0.6,el;q=0.4\r\n\r\n";
    char * filled_header = malloc(2048); // this should be big enough i think
    memset(filled_header, 0, 2048);
    snprintf(filled_header, 2048, header, url, host);
    return filled_header;
}

int url_to_memory(char * buffer, int buf_size, char * url, char * host, char * ip) {
    int n = 0;

    memset(buffer, 0, buf_size);

    int sockfd = get_socket(80, ip);
    if (sockfd < 0) {
        return 0;
    }
    char * header = generate_header(url, host);
    n = write(sockfd, header, strlen(header));
    if (n < 0) {
        return 0;
    }

    do {
        n = read(sockfd, buffer, buf_size-1);
        buffer[n] = 0;
    }
    while ( n == buf_size );

    free(header);
    shutdown(sockfd, 2);
    return 1;
}
