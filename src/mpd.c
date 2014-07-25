/* mpd.c
 * Copyright (C) 2014 Jack Pugmire
 *
 * The majority of this file was taken from mpdinfo
 * <https://github.com/jepugs/mpdinfo>. The copyright notice is reproduced
 * below:
 *
 * Copyright (C)  2014  Jack Pugmire
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mpd.h"

void mpd_info_str(const char fmt[], char* const dest, int len,
                  struct mpd_connection *conn, struct mpd_status *stat,
                  struct mpd_song *song)
{
    int t;
    const char *s;
    int loc = 0; // location in the destination string
    int l;
    char tmp[8];
    for (int i = 0; i < strlen(fmt); ++i) {
        if (loc >= len)
            return;

        if (fmt[i] == '%') {
            switch(fmt[i+1]) {
            case 'n':
                s = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
                l = strlen(s);
                l = l >= len - loc ? len - loc : l;
                strncpy(&dest[loc], s, l);
                loc += l;
                break;
            case 'a':
                s = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
                l = strlen(s);
                l = l >= len - loc ? len - loc : l;
                strncpy(&dest[loc], s, l);
                loc += l;
                break;
            case 'l':
                s = mpd_song_get_tag(song, MPD_TAG_ALBUM, 0);
                l = strlen(s);
                l = l >= len - loc ? len - loc : l;
                strncpy(&dest[loc], s, l);
                loc += l;
                break;
            case 'c':
                t = mpd_status_get_elapsed_time(stat);
                snprintf(tmp, 8, "%u:%02u", t / 60, t % 60);
                s = tmp;
                l = strlen(s);
                l = l >= len - loc ? len - loc : l;
                strncpy(&dest[loc], s, l);
                loc += l;
                break;
            case 'r':
                t = mpd_status_get_total_time(stat) -
                    mpd_status_get_elapsed_time(stat);
                snprintf(tmp, 8, "%u:%02u", t / 60, t % 60);
                s = tmp;
                l = strlen(s);
                l = l >= len - loc ? len - loc : l;
                strncpy(&dest[loc], s, l);
                loc += l;
                break;
            case 't':
                t = mpd_status_get_total_time(stat);
                snprintf(tmp, 8, "%u:%02u", t / 60, t % 60);
                s = tmp;
                l = strlen(s);
                l = l >= len - loc ? len - loc : l;
                strncpy(&dest[loc], s, l);
                loc += l;
                break;
            case '%':
                if (loc >= len)
                    break;
                dest[loc++] = '%';
                break;
            default:
                tmp[0] = '%';
                tmp[1] = fmt[i+1];
                l = 2 >= len - loc ? len - loc : 2;
                strncpy(&dest[loc], tmp, l);
                loc += 2;
                break;
            }
        ++i;
        } else {
            if (loc < len) {
                dest[loc++] = fmt[i];
            }
        }
    }
}

void get_mpd_info(const char fmt[], char* const dest, int len)
{
    struct mpd_connection *conn;
    struct mpd_status *stat;
    struct mpd_song *song;

    conn = mpd_connection_new(NULL, 0, 0);
    if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        mpd_connection_free(conn);
        fprintf(stderr, "error: Could not connect to mpd.\n");
        return;
    }
    stat = mpd_run_status(conn);
    song = mpd_run_current_song(conn);

    mpd_info_str(fmt, dest, len, conn, stat, song);

    mpd_song_free(song);
    mpd_status_free(stat);
    mpd_connection_free(conn);
}
