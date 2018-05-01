#include "GThread.hpp"

using namespace std;

GThread::GThread()
  :mFuncs(idlePrepare, idleCheck, idleDispatch, NULL)
{
  /* set source functions
  mFuncs.prepare = idlePrepare;
  mFuncs.check = idleCheck;
  mFuncs.dispatch = idleDispatch;
  mFuncs.finalize = NULL;
   */

  /* create a new source */
  mSource = (GIdleSource*)g_source_new (&mFuncs, sizeof(GIdleSource));
  /* create a context */
  mContext = g_main_context_new ();
  /* attach source to context */
  g_source_attach ((GSource *)mSource, mContext);
  /* create a main loop with context */
  mLoop(g_main_loop_new(mContext, FALSE), g_main_loop_unef)
  /* set the callback for this source */
  g_source_set_callback ((GSource *)mSource, Process, mLoop.get(), NULL);

  gIsRunning = false;
}

GThread::~GThread()
{
  while (!g_main_loop_is_running(mLoop.get())) {
    std::this_thread::yield();
  }

  if (gIsRunning) {
    g_main_loop_quit(mLoop.get());
    mThread.join();
    g_main_context_unref(mContext);
    g_source_destroy(mSource);
  }
}

void GThread::Run()
{
  if (gIsRunning.exchange(true)) {
    // assert(0 && "Loop is already running");
    return;
  }

  mThread = std::thread(g_main_loop_run, mLoop.get());
}

void GThread::Stop()
{
  if (gIsRunning.exchange(false)) {
    // assert(0 && "Loop is already stopped");
    return;
  }

  g_main_loop_quit(mLoop.get());
  mThread.join();
}

gboolean GThread::idlePrepare(GSource *source,gint *timeout_)
{
    *timeout_ = 1;
    //g_print ("prepare\n");
    return false;
}

gboolean GThread::idleCheck(GSource *source)
{
	static int count = 0;
    GSampleSource *src = (GSampleSource *)source;

	if (++count % src->test1)
        return FALSE;

    src->test2++;
    return TRUE;
}

gboolean GThread::idleDispatch(GSource *source,GSourceFunc callback,gpointer user_data)
{
    GSampleSource *src = (GSampleSource *)source;
    g_print ("dispatch src->test2 = %d\n", src->test2);
    return TRUE;
 
    if (callback(user_data))
        return TRUE;
    else
        return FALSE;
}

void WorkerThread::PostMsg(const UserData* data)
{
    //ASSERT_TRUE(m_thread);

    ThreadMsg* threadMsg = new ThreadMsg(MSG_POST_USER_DATA, data);

    // Add user data msg to queue and notify worker thread
    std::unique_lock<std::mutex> lk(m_mutex);
    m_queue.push(threadMsg);
    m_cv.notify_one();
}

gboolean GThread::Process(gpointer data)
{
    static int i = 0;
	//struct timeval tv;

    //gettimeofday(&tv, NULL);
    //g_print ("timeout_callback called %d times. %ld\n",++i, tv.tv_usec);
    if (100 == i++)
    {
        g_main_loop_quit((GMainLoop*)data);
        return FALSE;
    }

    return TRUE;
}










void Process()
{
    m_timerExit = false;
    std::thread timerThread(&WorkerThread::TimerThread, this);

    while (1)
    {
        ThreadMsg* msg = 0;
        {
            // Wait for a message to be added to the queue
            std::unique_lock<std::mutex> lk(m_mutex);
            while (m_queue.empty())
                m_cv.wait(lk);

            if (m_queue.empty())
                continue;

            msg = m_queue.front();
            m_queue.pop();
        }

        switch (msg->id)
        {
            case MSG_POST_USER_DATA:
            {
                //ASSERT_TRUE(msg->msg != NULL);

                // Convert the ThreadMsg void* data back to a UserData*
                const UserData* userData = static_cast<const UserData*>(msg->msg);

                cout << userData->msg.c_str() << " " << userData->year << " on " << THREAD_NAME << endl;

                // Delete dynamic data passed through message queue
                delete userData;
                delete msg;
                break;
            }

            case MSG_TIMER:
                cout << "Timer expired on " << THREAD_NAME << endl;
                delete msg;
                break;

            case MSG_EXIT_THREAD:
            {
                m_timerExit = true;
                timerThread.join();

                delete msg;
                std::unique_lock<std::mutex> lk(m_mutex);
                while (!m_queue.empty())
                {
                    msg = m_queue.front();
                    m_queue.pop();
                    delete msg;
                }

                cout << "Exit thread on " << THREAD_NAME << endl;
                return;
            }

            default:
                cout << "undefined message" << endl;
                //ASSERT();
        }
    }
}
