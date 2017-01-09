
#define _GNU_SOURCE

#include <string.h>
#include "colors.h"

// get a color from a string, and save it into the context
unlong _getcolor(DC *dc, const char *colstr) {
    XColor color;
    if(!XAllocNamedColor(dc->dpy, dc->wa.colormap, colstr, &color, &color)) {
        printf("cannot allocate color '%s'\n", colstr);
        return 0;
    }
    return color.pixel;
}

unlong alphaset(unlong color, uint8_t alpha) {
    uint32_t mod = alpha;
    mod <<= 24;
    return (0x00ffffff & color) | mod;
}

uint8_t hex(char c) {
    if (c >= '0' && c <= '9') {
        return (uint8_t)(c-'0');
    }
    if (c >= 'a' && c <= 'f') {
        return (uint8_t)(10+c-'a');
    }
    if (c >= 'A' && c <= 'F') {
        return (uint8_t)(10+c-'A');
    }
    return 0;
}

unlong getcolor(DC *dc, const char *colstr) {
    char *rgbcs = strdup(colstr);
    char *freeme = rgbcs;
    uint8_t value;
    switch(strlen(colstr)) {
        case 4: // #rgb
        case 7: // #rrggbb
            free(freeme);
            return _getcolor(dc, colstr);
        case 9: // #aarrggbb
            value = 16*hex(colstr[1]) + hex(colstr[2]);
            rgbcs += 2;
            break;
        case 5: // #argb
            value = hex(colstr[1]) * 17;
            rgbcs += 1;
            break;
    }

    rgbcs[0] = '#';
    unlong result = _getcolor(dc, rgbcs);
    result = alphaset(result, value);
    free(freeme);
    return result;
}

