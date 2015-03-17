#define _DEFAULT_SOURCE
#include "vdesk.h"
#include "format.h"

static int current_desktop = 0;
static fmt_map *formatmap = 0;

// roman numerals up to 3000
uint roman_numeral(char *buffer, uint num) {
    uint position = 0;
    uint special[13] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
    uint sp = 0;
    while(num) {
        while(num >= special[sp]) {
            switch(special[sp]) {
                case 900:
                    buffer[position++] = 'C';
                case 1000:
                    buffer[position++] = 'M';
                    break;
                case 400:
                    buffer[position++] = 'C';
                case 500:
                    buffer[position++] = 'D';
                    break;
                case 90:
                    buffer[position++] = 'X';
                case 100:
                    buffer[position++] = 'C';
                    break;
                case 40:
                    buffer[position++] = 'X';
                case 50:
                    buffer[position++] = 'L';
                    break;
                case 9:
                    buffer[position++] = 'I';
                case 10:
                    buffer[position++] = 'X';
                    break;
                case 4:
                    buffer[position++] = 'I';
                case 5:
                    buffer[position++] = 'V';
                    break;
                case 1:
                    buffer[position++] = 'I';
            }
            num -= special[sp];
        }
        sp++;
    }
    return position;
}

int get_number_of_desktops () {
    return get_x11_property(NET_NUMBER_DESKTOPS, _CARDINAL_);
}

int get_current_desktop () {
    return get_x11_property(NET_CURRENT_DESKTOP, _CARDINAL_);
}

uint16_t strcons(char *dest, const char *src, uint16_t ctr) {
    uint res = ctr;
    while(ctr --> 0) {
            dest[ctr] = src[ctr];
    }
    return res;
}

int _arabic_numerals(int place, char * string) {
    return place + sprintf(string+place, "%i", current_desktop);
}

int _roman_numerals(int place, char *string) {
    return place + roman_numeral(string+place, current_desktop);
}

int _japanese_numerals(int place, char *string) {
    return place + sprintf(string+place, "四");
}

char *get_desktops_info(baritem *source) {
    if (!source) {
        return NULL;
    }
    if (!(source -> format)) {
        perror("It seems you've not included a format string for the virtual desktop information.");
        perror("Please read the changes to configuration.md, github.com/tmathmeyer/tint3.");
        exit(0);
        return NULL;
    }
    if (!formatmap) {
        formatmap = initmap(8);
        fmt_map_put(formatmap, 'N', &_arabic_numerals);
        fmt_map_put(formatmap, 'R', &_roman_numerals);
        fmt_map_put(formatmap, 'J', &_japanese_numerals);
    }

    current_desktop = 1;
    char default_desktop[10] = {0};
    char selected_desktop[10] = {0};
    uint8_t sel = 0;
    uint8_t wpos = 0;
    uint8_t rpos = 0;
    int numdesk = get_number_of_desktops();
    int curdesk = get_current_desktop();
    char tmp_bfr[32] = {0};
    uint8_t ctr = 0;
    uint16_t offset = 0;
    while((source->format)[rpos]) {
        if ((source->format)[rpos] == ' ') {
            sel = 1;
            wpos = 0;
            rpos++;
            continue;
        }
        (sel?selected_desktop:default_desktop)[wpos++] = (source->format)[rpos++];
    }
    
    while(ctr < numdesk) {
        if (ctr == curdesk) {
            offset += format_string(tmp_bfr+offset, selected_desktop, formatmap);
        } else {
            offset += format_string(tmp_bfr+offset, default_desktop, formatmap);
        }
        offset += strcons(tmp_bfr+offset, " ", 1);
        current_desktop++;
        ctr++;
    }

    char *result = calloc(offset+1, sizeof(char));
    memcpy(result, tmp_bfr, offset);
    return result;
}