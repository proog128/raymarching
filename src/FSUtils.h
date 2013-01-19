#ifndef FSUTILS_H
#define FSUTILS_H

#include <string>
#include <SDL_thread.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>

typedef void(*OnFileChangedFunc)();

struct FileWatcherData
{
    HANDLE stopEvent;
    std::string filename;
    OnFileChangedFunc f;
};

int FileWatcherThread(void* d)
{
    FileWatcherData* data = (FileWatcherData*)d;

    char lpDrive[4];
    char lpFile[_MAX_FNAME];
    char lpDir[_MAX_PATH];
    char lpExt[_MAX_EXT];

    _splitpath_s(data->filename.c_str(), lpDrive, 4, lpDir, _MAX_PATH, lpFile, _MAX_FNAME, lpExt, _MAX_EXT);
    
    HANDLE Events[2];
    Events[0] = FindFirstChangeNotification(lpDir, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
    Events[1] = data->stopEvent;

    bool Stop = false;
    while(!Stop)
    {
        struct stat sb;
        stat(data->filename.c_str(), &sb);

        DWORD Status = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        if(Status == WAIT_OBJECT_0)
        {
            // Some file in directory was modified
            
            struct stat sb2;
            stat(data->filename.c_str(), &sb2);
            if(sb2.st_mtime != sb.st_mtime)
            {
                data->f();
            }
            
            FindNextChangeNotification(Events[0]);
        }
        else if(Status == WAIT_OBJECT_0 + 1)
        {
            Stop = true;
        }
    }
    FindCloseChangeNotification(Events[0]);
    CloseHandle(data->stopEvent);
    delete data;

    return 0;
}

FileWatcherData* WatchFile(const std::string& filename, OnFileChangedFunc func)
{
    FileWatcherData* data = new FileWatcherData;
    data->stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    data->filename = filename;
    data->f = func;

    SDL_CreateThread(FileWatcherThread, data);

    return data;
}

void StopFileWatcher(FileWatcherData* data)
{
    SetEvent(data->stopEvent);
}

template<int code>
void CreateSDLEventFunc()
{
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = code;
    event.user.data1 = 0;
    event.user.data2 = 0;
    SDL_PushEvent(&event);
}

#endif
