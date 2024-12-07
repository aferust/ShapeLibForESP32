#include "fshooks.hpp"

fs::FS *shp_fs = nullptr;

SAFile *SADFOpen(const char *pszFilename, const char *pszAccess,
                 void *pvUserData)
{
    (void)pvUserData;

    auto file = new fs::File(shp_fs->open(pszFilename, pszAccess));
    if (!(*file))
    {
        delete file;
        return nullptr;
    }
    return file;
}

SAOffset SADFRead(void *p, SAOffset size, SAOffset nmemb, SAFile *file)
{
    SAOffset bytesRead = file->read((uint8_t *)p, size * nmemb);

    if (bytesRead == size * nmemb)
    {
        return nmemb;
    }
    else
    {
        return bytesRead / size;
    }
}

SAOffset SADFWrite(const void *p, SAOffset size, SAOffset nmemb, SAFile *file)
{
    size_t bytesWritten = file->write((const uint8_t *)p, size * nmemb);
    if (bytesWritten == size * nmemb)
    {
        return nmemb;
    }
    else
    {
        return 0;
    }
}

SAOffset SADFSeek(SAFile *file, SAOffset offset, int whence)
{
    return file->seek(offset, (fs::SeekMode)whence) ? 0 : -1;
}

SAOffset SADFTell(SAFile *file)
{
    return file->position();
}

int SADFFlush(SAFile *file)
{
    file->flush();
    return 0;
}

int SADFClose(SAFile *file)
{
    if (file)
    {
        file->close();
        delete file;
        return 0;
    }
    return -1;
}

int SADRemove(const char *filename, void *pvUserData)
{
    (void)pvUserData;
    return (int)shp_fs->remove(filename);
}

void SADError(const char *message)
{
    ESP_LOGE(SHP_LOG_TAG, "%s\n", message);
}