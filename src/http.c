/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "http.h"

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

char * generate_header(char * url, char * host) {
    char * header =
        "GET %s "
        "HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Cache-Control: no-cache\r\n"
        "Accept: text/html,application/xhtml+xml,"
        "application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
        "Pragma: no-cache\r\n"
        "Accept-Encoding: gzip,deflate,sdch\r\n"
        "Accept-Language: en-US,en;q=0.8,ru;q=0.6,el;q=0.4\r\n\r\n";
    char * filled_header = malloc(2048); // this should be big enough i think
    memset(filled_header, 0, 2048);
    snprintf(filled_header, 2048, header, url, host);
    return filled_header;
}

int hostname_to_ip(char * hostname , char* ip) {
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ( (he = gethostbyname( hostname ) ) == NULL) {
        herror("gethostbyname");
        return 0;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++) {
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 1;
    }

    return 0;
}


int url_to_memory(char * buffer, int buf_size, char * url, char * host, char * ip) {
    int n = 0;

    memset(buffer, 0, buf_size);

    int sockfd = get_socket(80, ip);
    if (sockfd < 0) {
        perror("error, could not open socket");
        return 0;
    }
    char * header = generate_header(url, host);
    n = write(sockfd, header, strlen(header));
    if (n < 0) {
        perror("error, could not write to socket");
        shutdown(sockfd, 0);
        close(sockfd);
        return 0;
    }

    do {
        n = read(sockfd, buffer, buf_size-1);
        buffer[n] = 0;
    }
    while ( n == buf_size );

    free(header);
    shutdown(sockfd, 0);
    close(sockfd);
    return 1;
}
