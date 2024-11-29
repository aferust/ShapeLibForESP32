#ifndef FSHOOKS_H_INCLUDED
#define FSHOOKS_H_INCLUDED

#include "shapefil_private.hpp"
#include "shapefil.hpp"

extern void *shp_fs;

SAFile SADFOpen(const char *pszFilename, const char *pszAccess,
                void *pvUserData);
SAOffset SADFRead(void *p, SAOffset size, SAOffset nmemb, SAFile &file);

SAOffset SADFWrite(const void *p, SAOffset size, SAOffset nmemb,
                   SAFile &file);
SAOffset SADFSeek(SAFile &file, SAOffset offset, int whence);

SAOffset SADFTell(SAFile &file);
int SADFFlush(SAFile &file);
int SADFClose(SAFile &file);

int SADRemove(const char *filename, void *pvUserData);
void SADError(const char *message);

#endif // FSHOOKS_H_INCLUDED