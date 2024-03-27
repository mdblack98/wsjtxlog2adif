/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>


#define VERSION "20240327.0"

char *yourcall, *yourgrid;

char *getDateTimeString()
{
    // Buffer to hold the formatted date/time string
    static char dateTimeStr[64];

    if (dateTimeStr == NULL)
    {
        // Memory allocation failed
        return NULL;
    }

    // Get current time
    time_t now = time(NULL);

    // Convert time_t to struct tm (local time)
    struct tm *localTime = localtime(&now);

    if (localTime == NULL)
    {
        // localtime failed
        return NULL;
    }

    // Format the date and time
    strftime(dateTimeStr, 16, "%Y%m%d %H%M%S", localTime);

    return dateTimeStr;
}

char *strsep(char **stringp, const char *delim)
{
    char *rv = *stringp;

    if (rv)
    {
        *stringp += strcspn(*stringp, delim);

        if (**stringp)
        {
            *(*stringp)++ = '\0';
        }
        else
        {
            *stringp = 0;
        }
    }

    return rv;
}

const char *getHamBand(char *s)
{
    double frequency = atof(s);

    if (frequency >= 1.8 && frequency <= 2.0)
    {
        return "160M";
    }
    else if (frequency >= 3.5 && frequency <= 4.0)
    {
        return "80M";
    }
    else if (frequency >= 5.3 && frequency <= 5.4)
    {
        return "60M";
    }
    else if (frequency >= 7.0 && frequency <= 7.3)
    {
        return "40M";
    }
    else if (frequency >= 10.1 && frequency <= 10.15)
    {
        return "30M";
    }
    else if (frequency >= 14.0 && frequency <= 14.35)
    {
        return "20M";
    }
    else if (frequency >= 18.068 && frequency <= 18.168)
    {
        return "17M";
    }
    else if (frequency >= 21.0 && frequency <= 21.45)
    {
        return "15M";
    }
    else if (frequency >= 24.89 && frequency <= 24.99)
    {
        return "12M";
    }
    else if (frequency >= 28.0 && frequency <= 29.7)
    {
        return "10M";
    }
    else if (frequency >= 50 && frequency <= 54)
    {
        return "6M";
    }
    else if (frequency >= 144 && frequency <= 148)
    {
        return "2M";
    }
    else if (frequency >= 222 && frequency <= 225)
    {
        return "1.25M";
    }
    else if (frequency >= 420 && frequency <= 450)
    {
        return "70CM";
    }
    else if (frequency >= 902 && frequency <= 928)
    {
        return "33CM";
    }
    else if (frequency >= 1240 && frequency <= 1300)
    {
        return "23CM";
    }
    else if (frequency >= 2300 && frequency <= 2450)
    {
        return "13CM";
    }
    else
    {
        return "Out of amateur bands";
    }
}

char *mode(char *s)
{
    static char buf[32];

    if (strcmp(s, "FT8") == 0)
    {
        sprintf(buf, "<MODE:3>FT8");
    }
    else if (strcmp(s, "FT4") == 0)
    {
        sprintf(buf, "<MODE:4>MFSK <SUBMODE:3>FT4");
    }
    else
    {
        sprintf(buf, "<MODE:%ld>%s", strlen(s), s);
    }

    return buf;
}

void toUpper(char *str)
{
    while (*str)
    {
        *str = toupper((unsigned char) * str);
        str++;
    }
}

void removeChar(char *str, char charToRemove)
{
    char *src, *dest;

    for (src = dest = str; *src != '\0'; src++)
    {
        *dest = *src;

        if (*dest != charToRemove) { dest++; }
    }

    *dest = '\0';
}

void wsjtxlog2adif(FILE *fpin, FILE *fpout)
{
    char buf[4096];
    char *s;
    toUpper(yourcall);
    toUpper(yourgrid);

    fprintf(fpout, "ADIF Export\n");
    fprintf(fpout, "<ADIF_VER:5>3.1.1\n");
    s = getDateTimeString();
    fprintf(fpout, "<CREATED_TIMESTAMP:%ld> %s\n", strlen(s), s);
    fprintf(fpout, "<PROGRAMID:> wsjtxlog2adif\n");
    fprintf(fpout, "<PROGRAMVERSION:%ld> %s\n", strlen(VERSION), VERSION);
    fprintf(fpout, "<EOH>\n");

    while (fgets(buf, sizeof(buf), fpin))
    {
        s = buf;
        fprintf(fpout, "<STATION_CALLSIGN:%ld>%s ", strlen(yourcall), yourcall);
        fprintf(fpout, "<MY_GRIDSQUARE:%ld>%s ", strlen(yourgrid), yourgrid);
        char *p = strsep(&s, ",\r\n");
        removeChar(p, '-');
        fprintf(fpout, "<QSO_DATE:%ld>%s ", strlen(p), p);
        p = strsep(&s, ",\r\n");
        removeChar(p, ':');
        fprintf(fpout, "<TIME_ON:%ld>%s ", strlen(p), p);
        p = strsep(&s, ",\r\n");
        removeChar(p, '-');
        fprintf(fpout, "<QSO_DATE_OFF:%ld>%s ", strlen(p), p);
        p = strsep(&s, ",\r\n");
        removeChar(p, ':');
        fprintf(fpout, "<TIME_OFF:%ld>%s ", strlen(p), p);
        p = strsep(&s, ",\r\n");
        fprintf(fpout, "<CALL:%ld>%s ", strlen(p), p);
        p = strsep(&s, ",\r\n");

        if (strlen(p) > 0)
        {
            fprintf(fpout, "<GRID:%ld>%s ", strlen(p), p);
        }

        p = strsep(&s, ",\r\n");
        fprintf(fpout, "<FREQ:%ld>%s ", strlen(p), p);
        fprintf(fpout, "<BAND:%ld>%s ", strlen(getHamBand(p)), getHamBand(p));


        p = strsep(&s, ",\r\n");
        fprintf(fpout, "%s ", mode(p));

        p = strsep(&s, ",\r\n");
        fprintf(fpout, "<RST_SENT:%ld>%s ", strlen(p), p);

        p = strsep(&s, ",\r\n");
        fprintf(fpout, "<RST_RCVD:%ld>%s ", strlen(p), p);
        p = strsep(&s, ",\r\n");
        fprintf(fpout, "<POWER:%ld>%s ", strlen(p), p);
        p = strsep(&s, ",\r\n");
        fprintf(fpout, "<COMMENT:%ld>%s ", strlen(p), p);

        fprintf(fpout, "<EOR>\n");
    }
}

int main(int argc, char *argv[])
{
    FILE *fpin, *fpout;

    if (argc != 5)
    {
        fprintf(stderr, "Version 1.0\n");
        fprintf(stderr, "Usage: %s infile outfile yourcall yourgrid\n", argv[0]);
        exit(1);
    }

    fpin = fopen(argv[1], "r");

    if (fpin == NULL)
    {
        perror(argv[1]);
        exit(2);
    }

    fpout = fopen(argv[2], "w");

    if (fpout == NULL)
    {
        perror(argv[1]);
        exit(2);
    }

    yourcall = strdup(argv[3]);
    yourgrid = strdup(argv[4]);
    wsjtxlog2adif(fpin, fpout);
    fclose(fpin);
    fclose(fpout);
    return 0;
}
