/* mpd.h --- get information from mpd
 * Copyright (C) 2014 Jack Pugmire
 */

#include <mpd/client.h>
#include <stdio.h>
#include <string.h>

void mpd_info_str(const char fmt[], char* const dest, int len,
                  struct mpd_connection *conn, struct mpd_status *stat,
                  struct mpd_song *song);

void get_mpd_info(const char fmt[], char* const dest, int len);
