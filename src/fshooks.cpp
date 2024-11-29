#include "fshooks.hpp"

void *shp_fs = nullptr;

SAFile SADFOpen(const char *pszFilename, const char *pszAccess,
                void *pvUserData)
{
    (void)pvUserData;
    return ((fs::FS *)shp_fs)->open(pszFilename, pszAccess);
}

SAOffset SADFRead(void *p, SAOffset size, SAOffset nmemb, SAFile &file)
{
    SAOffset bytesRead = file.read((uint8_t *)p, size * nmemb);

    if (bytesRead == size * nmemb)
    {
        return nmemb;
    }
    else
    {
        return bytesRead / size;
    }
}

SAOffset SADFWrite(const void *p, SAOffset size, SAOffset nmemb,
                   SAFile &file)
{
    return (SAOffset)file.write((const uint8_t *)p, size * nmemb);
}

SAOffset SADFSeek(SAFile &file, SAOffset offset, int whence)
{
    return file.seek(offset, (fs::SeekMode)whence) ? 0 : -1;
}

SAOffset SADFTell(SAFile &file)
{
    return file.position();
}

int SADFFlush(SAFile &file)
{
    file.flush();
    return 0;
}

int SADFClose(SAFile &file)
{
    file.close();
    return 0;
}

int SADRemove(const char *filename, void *pvUserData)
{
    (void)pvUserData;
    return (int)((fs::FS *)shp_fs)->remove(filename);
}

void SADError(const char *message)
{
    Serial.println(message);
}