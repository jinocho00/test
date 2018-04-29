#include <glib.h>
#include <sys/time.h>
#include <time.h>

typedef struct {
    GSource source;
    int test1;
    int test2;
} GSampleSource;

gboolean callback(gpointer data)
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

gboolean prepare(GSource *source,gint *timeout_)
{
    *timeout_ = 1;
    //g_print ("prepare\n");
    return FALSE;
}

gboolean check(GSource *source)
{
	static int count = 0;
    GSampleSource *src = (GSampleSource *)source;

	if (++count % src->test1)
        return FALSE;

    src->test2++;
    return TRUE;
}

gboolean dispatch(GSource *source,GSourceFunc callback,gpointer user_data)
{
    GSampleSource *src = (GSampleSource *)source;
    g_print ("dispatch src->test2 = %d\n", src->test2);
    return TRUE;
 
    if (callback(user_data))
        return TRUE;
    else
        return FALSE;
}

int main()
{
    GMainLoop *loop = NULL;
    GMainContext *context;
    GSampleSource *source;
    int id;
	
    //create a variable of type GSourceFuncs
    GSourceFuncs SourceFuncs =
    {
        prepare,
        check,
        dispatch,
        NULL
    };
	
    //create a new source
    source = (GSampleSource *)g_source_new (&SourceFuncs, sizeof(GSampleSource));
	source->test1 = 100;
    source->test2 = 0;
    //create a context
    context = g_main_context_new ();
	
    //attach source to context
    id = g_source_attach ((GSource *)source,context);
	
    //create a main loop with context
    loop = g_main_loop_new (context,FALSE);
 
    //set the callback for this source
    g_source_set_callback ((GSource *)source,callback,loop,NULL);
	
    g_main_loop_run (loop);
    g_main_loop_unref (loop);
	
    return 0;
}