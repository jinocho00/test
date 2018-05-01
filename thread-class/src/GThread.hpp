#include <thread>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <iostream>
//#include <cstring>
#include <string>
#include <chrono>
#include <cassert>

typedef enum {
    MSG_POST_USER_DATA = 0,
    MSG_TIMER,
    MSG_EXIT_THREAD
} MessageId;

typedef struct {
    std::string msg;
    int year;
} UserData;

typedef struct {
    GSource source;
    int test1;
    int test2;
} GIdleSource;

class ThreadMsg
{
public:
    ThreadMsg(MessageId mid, const UserData* data)
    {
        id = mid;
        msg = data;
    };
    int id;
    const UserData* msg;
};

class GThread
{
public:

  GThread();
  ~GThread();

  void Run();
  void Stop();

  void PostMsg(const UserData* data);

  gboolean idlePrepare(GSource *source,gint *timeout_);
  gboolean idleCheck(GSource *source);
  gboolean idleDispatch(GSource *source,GSourceFunc callback,gpointer user_data);

private:
  std::unique_ptr<GMainLoop, void(*)(GMainLoop*)> mLoop;
  std::thread mThread;
  std::atomic_bool gIsRunning;
  GSourceFuncs mFuncs;
  GMainContext *mContext;
  GIdleSource *mSource;
};
