///////////////////////////////////////////////////////////////////////////////
//  File         :    wlCore.cxx                                             //
//  Author       :    Wilfrid Lefer                                          //
//  Comment      :    Core classe pour mes developpements                    //
//  Related file :    wlCore.h                                               //
//                    Wilfrid Lefer - UPPA - 2003                            //
///////////////////////////////////////////////////////////////////////////////


#include "wlCore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FORMAT(type) { \
\
  va_list args; \
  char s[MAXSTR+1], *c, *cc; \
  int backslashr = 0; \
\
  va_start(args, format); \
  vsnprintf(s, MAXSTR, format, args); \
  va_end(args); \
\
  if ((int)(strlen(this->line)+strlen(s)) > this->maxline) { \
    if ((this->line = (char *)realloc(this->line, this->maxline+MAXSTR+1)) == NULL) { \
      this->line[0] = '\0'; \
      this->Error("can't allocate memory to print a so long message"); \
      return; \
    } else this->maxline += MAXSTR; \
  } \
\
  if (s[0] == '\r') { \
    memmove(s, &s[1], strlen(s)); \
    backslashr = this->line[0] == '\0'; \
  } \
\
  for (c=s ; *c!='\0' ;) { \
    for (cc=&this->line[strlen(this->line)] ; *c!='\0' && *c!='\n' ; c++,cc++) \
      if (*c=='\r' && c[1]=='\0') { \
	*cc = '\0'; \
	return; \
      } else *cc = *c; \
    *cc = '\0'; \
    if (*c == '\n') \
      c++; \
    switch (type) { \
      case 0: \
        printf("%s%s: %s\n", backslashr ? "\r" : "", this->GetClassName(), this->line); \
        break; \
      case 1: \
        printf("%s%s: Warning: %s\n", backslashr ? "\r" : "", this->GetClassName(), this->line); \
        break; \
      case 2: \
        printf("%s%s: Error: %s\n", backslashr ? "\r" : "", this->GetClassName(), this->line); \
    } \
    this->line[0] = '\0'; \
  } \
}

wlCore::wlCore(int debug)
{
  this->Debug = debug;
  this->maxline = MAXSTR;
  if ((this->line = (char *)malloc(this->maxline+1)) == NULL) {
    fprintf(stderr, "wlCore: error: can't allocate memory\n");
    exit(-1);
  }
  this->line[0] = '\0';
}

void
wlCore::Trace(char *format, ...)
{
  if (!this->Debug)
    return;
  FORMAT(0);
}

void
wlCore::Error(char *format, ...)
{
  FORMAT(2);
}

void
wlCore::Print(char *format, ...)
{
  FORMAT(0);
}

void
wlCore::Warning(char *format, ...)
{
  FORMAT(1);
}
