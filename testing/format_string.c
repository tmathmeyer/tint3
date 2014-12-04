#include <string.h>
#include <stdio.h>


int main()
{
   /*
        %T = temperature
        $D = dew point
        %H = relative humidity
        %R = rainfall (last hour)
        %P = pressure
        %W = weather
        %S = Sky Conditions
      */

    char dest[100] = {0};

    printf("chars: %i\n", format_string(dest, "the %H is bad"));

    puts(dest);
}

char * get_source() {
    char * string = "weather: something";
}

int temperature(int start, char * source, char * dest) {
    dest[start+0] = 'F';
    dest[start+1] = 'U';
    dest[start+2] = 'C';
    dest[start+3] = 'K';
    return start+4;
}

int dew_point(int start, char * source, char * dest) {
    return start;
}

int humidity(int start, char * source, char * dest) {
    snprintf(dest+start, strlen("humidity")+1, "humidity");
    return start+8;
}

int rainfall(int start, char * source, char * dest) {
    return start;
}

int pressure(int start, char * source, char * dest) {
    return start;
}

int weather(int start, char * source, char * dest) {
    return start;
}

int condition(int start, char * source, char * dest) {
    return start;
}


int format_string(char * dest, char * fmt) {
    int (*T)(int, char *, char *) = temperature;
    int (*D)(int, char *, char *) = dew_point;
    int (*H)(int, char *, char *) = humidity;
    int (*R)(int, char *, char *) = rainfall;
    int (*P)(int, char *, char *) = pressure;
    int (*W)(int, char *, char *) = weather;
    int (*S)(int, char *, char *) = condition;




    char * source = get_source();
    int e = 0;
    int i = 0;
    int d = 0;
    for(;fmt[i];i++) {
        if (fmt[i] == '%') {
            d = !d;
        } else if (d) {
            d = !d;
            switch(fmt[i]) {
                case 'T':
                    e = T(e, source, dest); break;
                case 'D':
                    e = D(e, source, dest); break;
                case 'H':
                    e = H(e, source, dest); break;
                case 'R':
                    e = R(e, source, dest); break;
                case 'P':
                    e = P(e, source, dest); break;
                case 'W':
                    e = W(e, source, dest); break;
                case 'S':
                    e = S(e, source, dest); break;
                case '%':
                    dest[e++] = '%'; break;
                default:
                    perror("FAIL");
            }
        } else {
            dest[e++] = fmt[i];
        }
    }
    return e;
}

