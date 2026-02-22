#include "worker.h"



// MACROS

#define WORKER_EXIT_TIMEOUT 1  // wait time for shell_stop
#define WORKER_STOP_TIMEOUT 2  // wait time for shell_kill
#define WORKER_KILL_TIMEOUT 1  // wait time for worker_kill



// INTERNAL

static void* worker_loop(void* arg)
{
    return TODO;
}


static void worker_destroy(Worker* w)
{
    if(w == NULL) return;
    if(w->state == DEAD) return;    // prevent double destroy
    pthread_mutex_destroy(&w->mutex_lock);
    pthread_cond_destroy(&w->flag);
    w->state = DEAD;

    return;
}



// INTERFACE

bool worker_init(Worker* w, unsigned int id)
{
    if(w == NULL) return false;

    w->state = DEAD;    // asume dead until suceesful creation
    if(pthread_mutex_init(&w->mutex_lock, NULL)) return false;
    if(pthread_cond_init(&w->flag, NULL))
    {
        pthread_mutex_destroy(&w->mutex_lock);
        return false;
    }

    w->worker_id = id;
    w->executor = shell_init();
    w->state = INIT;
    w->action = CONTINUE;

    return true;
}


bool worker_start(Worker* w)
{
    if(w == NULL || w->state == DEAD) return false;
    if(w->state == RUNNING || w->state == WAITING) return true;

    pthread_mutex_lock(&w->mutex_lock);
    w->action = CONTINUE;       // action == NONE => run normally
    int r_create = 0;       // weather creation happened with error
    

    if(w->state == INIT || w->state == STOPPED) // create nonexistent thread
        r_create = pthread_create(&w->handle, NULL, worker_loop, NULL);
    //state == HALTED || RESETTING
    else pthread_cond_signal(&w->flag);

    pthread_mutex_unlock(&w->mutex_lock);
    if(r_create) // creation failed
    {
        worker_destroy(w); 
        return false;
    }

    return true;
}


void worker_kill(Worker* w)
{
    if(w == NULL) return;
    if(w->state == DEAD) return;

    pthread_mutex_lock(&w->mutex_lock);
    shell_kill(&w->executor, WORKER_KILL_TIMEOUT);
    pthread_mutex_unlock(&w->mutex_lock);

    pthread_cancel(w->handle);
    pthread_join(w->handle, NULL);

    worker_destroy(w);
    return;
}


bool worker_request(Worker* w, worker_action action)
{
    if(w == NULL) return false;
    if(w->state == DEAD) return false;    // Signaling dead thread is useless -> avoid null mutex locking

    bool action_sent = false;
    pthread_mutex_lock(&w->mutex_lock);
    if(w->action == CONTINUE)
    {
        w->action = action;
        pthread_cond_signal(&w->flag);
        action_sent = true;
    }
    pthread_mutex_unlock(&w->mutex_lock);

    return  action_sent;
}
