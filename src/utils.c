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
#include "config.h"
#include "defaults.h"
#include "utils.h"


batt_info * get_battery_information() {
    batt_info * bi = malloc(sizeof (batt_info));
    bi -> icon = "X";
    bi -> percentage = 0;

    char * path = "/sys/class/power_supply/BAT0";
    FILE * fp;
    char battery_read[512];
    char tmp[64];

    int energy_now = 1;
    int energy_full = 1;
    int battery_charging = 0;


    snprintf(battery_read, sizeof battery_read, "%s/%s", path, "energy_now");
    fp = fopen(battery_read, "r");
    if(fp != NULL) {
        if (fgets(tmp, sizeof tmp, fp)) energy_now = atoi(tmp);
        fclose(fp);
    } else {
        return NULL;
    }

    snprintf(battery_read, sizeof battery_read, "%s/%s", path, "energy_full");
    fp = fopen(battery_read, "r");
    if(fp != NULL) {
        if (fgets(tmp, sizeof tmp, fp)) energy_full = atoi(tmp);
        fclose(fp);
    } else {
        return NULL;
    }

    snprintf(battery_read, sizeof battery_read, "%s/%s", path, "status");
    fp = fopen(battery_read, "r");
    if(fp != NULL) {
        if (fgets(tmp, sizeof tmp, fp)) {
            battery_charging = (strncmp(tmp, "Discharging", 11) != 0);
        }
        fclose(fp);
    } else {
        return bi;
    }

    bi -> percentage = energy_now / (energy_full / 100);
    if (bi -> percentage > 90) {
        bi -> icon = "⮒";
    } else if (bi -> percentage > 65) {
        bi -> icon = "⮏";
    } else if (bi -> percentage > 10) {
        bi -> icon = "⮑";
    } else if (bi -> percentage > 0) {
        bi -> icon = "⮐";
    } else {
        bi -> icon = "?";
    }
    if (battery_charging) {
        bi -> icon = "⮎";
    }

    return bi;
}


unsigned long long old_down=0, old_up=0;
net_info * net = NULL;
net_info * get_net_info (void) {
    if (net == NULL) {
        net = malloc(sizeof(net_info));
        net -> timeout = 0; // 2 seconds
        net -> lastime = 0; // start over;
        net -> up = make_new_graph();
        net -> down = make_new_graph();
    }

    if (time(NULL) - net -> lastime > net -> timeout) {
        time((time_t*)&(net -> lastime));
        FILE * fp = fopen("/proc/net/dev", "r");
        if (fp == NULL) {
            return net;
        }

        char * buffer = malloc(4096), * bt = buffer, temp = 0;
        while( (temp=fgetc(fp)) != EOF) {
            *(bt++) = temp;
        }
        fclose(fp);
        bt = strstr(buffer, NETIFACE);
        if (bt == 0) {
            free(buffer);
            return net;
        }
        bt += (strlen(NETIFACE) + 2);

        unsigned long long up, down;
        sscanf(bt, "%llu %llu", &down, &up);

        int ud = up-old_up, dd = down-old_down;

        add_to_graph(dd, net -> down);
        add_to_graph(ud, net -> up);

        old_down = down;
        old_up = up;

        free(buffer);
    }

    return net;
}


weather_info * weather = NULL;
weather_info * get_weather() {

    if (weather == NULL) {
        weather = malloc(sizeof(weather_info));
        weather -> timeout = 1800; // 30 minutes
        weather -> lastime = 0; // start over;
        weather -> temperature = 0;
        weather -> humidity = 0;
        weather -> condition = NULL;
    }

    if (time(NULL) - weather -> lastime > weather -> timeout) {
        time((time_t*)&(weather -> lastime));
        int weather_parse_size = 1024;
        char * weather_s = malloc(weather_parse_size);
        char * host = "weather.noaa.gov";
        char * url  = "/pub/data/observations/metar/decoded/" WEATHER_LOCATION ".TXT";

        url_to_memory(weather_s, weather_parse_size, url, host, "208.59.215.33");

        char * temp = strstr(weather_s, "Temperature") + 13;
        char * humd = strstr(weather_s, "Humidity") + 10;

        sscanf(temp, "%i", &(weather -> temperature));
        sscanf(humd, "%i", &(weather -> humidity));

        free(weather_s);
    }


    return weather;
}

char * get_desktops_info() {
    FILE * query = popen(DESKTOP_QUERY, "r");
    FILE * count = popen(DESKTOP_COUNT, "r");
    if (count == NULL || query == NULL) {
        return NULL;
    }
    int numdesk = 0;
    int curdesk = 0;
    int swap = 0;

    swap = fscanf(query, "%i", &curdesk);
    swap = fscanf(count, "%i", &numdesk);
    fclose(count);
    fclose(query);

    int dsktplen = numdesk * 4 - 1;
    char * result = malloc(dsktplen);

    for(swap=0; swap < dsktplen; swap++) {
        int sqp = swap%4;
        result[swap] = sqp==3?' ':DESKTOP_DEFAULT[sqp];
    }

    result[(curdesk-DESKTOP_ZIDEX)*4 + 0] = DESKTOP_CURRENT[0];
    result[(curdesk-DESKTOP_ZIDEX)*4 + 1] = DESKTOP_CURRENT[1];
    result[(curdesk-DESKTOP_ZIDEX)*4 + 2] = DESKTOP_CURRENT[2];

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

void url_to_memory(char * buffer, int buf_size, char * url, char * host, char * ip) {
    int n = 0;

    memset(buffer, 0, buf_size);

    int sockfd = get_socket(80, ip);
    char * header = generate_header(url, host);
    n = write(sockfd, header, strlen(header));

    do {
        n = read(sockfd, buffer, buf_size-1);
        buffer[n] = 0;
    }
    while ( n == buf_size );

    free(header);
    shutdown(sockfd, 2);
}