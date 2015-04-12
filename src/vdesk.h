#ifndef _T3_VDESK_
#define _T3_VDESK_

#include "tint3.h"

char *get_desktops_info(baritem *item);
void *vdesk_listen(void *DATA);
void spawn_vdesk_thread(baritem *ipl);

#endif