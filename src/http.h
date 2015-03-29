/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef _NETWORK_H_
#define _NETWORK_H_

int url_to_memory(char *buffer, int buf_size,
                  char *url, char *host, char *ip);

int hostname_to_ip(char *hostname , char *ip);

int get_socket(int port_number, char *ip);

char *generate_header(char *url, char *host);

#endif
