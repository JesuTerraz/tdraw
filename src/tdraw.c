
#include "draw.h"
#include "queue.h"
#include "model.h"

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXTHREADS          1

// Let each thread know it's ID
struct thread_input {
    int tid;
};

typedef struct write_semaphore {
    int drawing;                // screen buffer must not be written to during draw.
    int writers;                // number of writers at the moment
    pthread_cond_t draw_done;   // screen done drawing.
    pthread_cond_t write_done;  // writers done.
    pthread_mutex_t mtx;
} WSemaphore;

static WSemaphore semaphore;

static pthread_t dtid;                  // Drawing thread.
static pthread_t btid;               // Balancing thread.
static pthread_t wtids[MAXTHREADS];     // Writing threads.
static struct thread_input inputs[MAXTHREADS];

/*
 * Each thread as a designated queue it must process.
 * Each queue contains a certain [row, col] area on the screen.
 * Only access these with TDX
*/
static Queue tqueues[MAXTHREADS];
static pthread_cond_t queue_notempty[MAXTHREADS];
static pthread_mutex_t queue_mutex[MAXTHREADS];

void
write_entry(void)
{
    // Check if writing can start (no draw)
    pthread_mutex_lock(&semaphore.mtx);
    while (semaphore.drawing) {
        pthread_cond_wait(&semaphore.draw_done, &semaphore.mtx);
    }

    // Make sure a draw can't start.
    semaphore.writers++;

    pthread_mutex_unlock(&semaphore.mtx);
}

void
write_exit(void)
{
    pthread_mutex_lock(&semaphore.mtx);
    semaphore.writers--;

    // Signal in case draw is waiting. 
    if (semaphore.writers == 0)
        pthread_cond_signal(&semaphore.write_done);

    pthread_mutex_unlock(&semaphore.mtx);
}

/*
 * Pick up pixels from tqueues[wtid] and set them using
 * set_pixel(Pixel *pix);
*/
void *
write_routine(void *arg)
{
    (void)arg;
    // Pose colors = { WHITE, WHITE };
    // Pose3D pose = { { 1, 1 },  5 };
    // Pose velocity = { 1, 1 };

    // PixelDef pixeldefs[1] = {
    //     {'V', colors, {{ 0, 0 }, 0}},
    // };

    // Model *m;
    // m = create_model(pose, velocity, 1, pixeldefs);

    int x = (rand() % WINSIZE.x) + 1;
    int y = (rand() % WINSIZE.y) + 1;

    Pose3D p = { { x, y }, 1 };
    Pose v = { 1, 1 };
    Pose colors = { (rand() % WHITE), (rand() % WHITE) };
    char c = '0' + (rand() % 10);
    PixelDef pixeldefs[5] = {
        {c, colors, {{ 0, 0 }, 0}},
        {c, colors, {{ 0, 1 }, 0}},
        {c, colors, {{ 0, 2 }, 0}},
        {c, colors, {{ 1, 1 }, 0}},
        {c, colors, {{ -1, 1 }, 0}},
    };

    Model *m;
    m = create_model(p, v, 5, pixeldefs);

    int drawn = 0;

    while(1)
    {
        // Retrieve pixel from queue.
        MoveCmd r = (rand() % MOVE_RIGHT) + 1;

        // Write Pixel To Buf.
        // Entry Routine.
        write_entry();
        
        if (!drawn) {
            draw_model(m);
            drawn = 1;
        }
        
        move_model(m, r);
        // Actual Routine.

        // Exit Routine
        write_exit();
    }

    return (NULL);
}

void
draw_entry(void)
{
    // No writer should start
    pthread_mutex_lock(&semaphore.mtx);
    semaphore.drawing = 1;

    // Wait til writers are done.
    while (semaphore.writers) {
        pthread_cond_wait(&semaphore.write_done, &semaphore.mtx);
    }

    pthread_mutex_unlock(&semaphore.mtx);
}

void
draw_exit(void)
{
    // Let writers start again.
    pthread_mutex_lock(&semaphore.mtx);
    semaphore.drawing = 0;
    pthread_mutex_unlock(&semaphore.mtx);

    // Wake up any sleeping writers.
    pthread_cond_broadcast(&semaphore.draw_done);
}

/*
 * Lock the buffer and draw all contents to screen.
*/
void *
draw_routine(void *arg)
{
    (void)arg;

    pthread_detach(pthread_self());

    while(1)
    {
        // Entry Routine.
        draw_entry();

        // Actual Routine.
        draw();

        // Exit Routine.
        draw_exit();

        usleep(10000);
    }

    return (NULL);
}

/*
 * Handle library calls to set pixels. Delegates to the correct
 * queue.
*/
void *
balance_routine(void *arg)
{
    (void)arg;

    // Entry Routine.
    // Read library call from somewhere....
    Pixel *pix;
    int rc, tdx;

    (void)pix;
    tdx = 0;

    // Get TDX somehow....
    // int tdx = TO_TDX(pix->pose.p.x, pix->pose.p.y);

    if (!(rc = pthread_mutex_trylock(&queue_mutex[tdx]))) {
        // queue_push(tqueues[tdx]);
        pthread_mutex_unlock(&queue_mutex[tdx]);
    }

    return (NULL);
}

void
start_tdraw(void)
{
    int i;

    (void)tqueues;
    init_scr(0);

    semaphore.drawing = 0;
    semaphore.writers = 0;
    pthread_mutex_init(&semaphore.mtx, NULL);
    pthread_cond_init(&semaphore.write_done, NULL);
    pthread_cond_init(&semaphore.draw_done, NULL);

    // Only one drawing thread:
    pthread_create(&dtid, NULL, draw_routine, NULL);
    
    // Multiple writers:
    for (i = 0; i < MAXTHREADS; i++) {
        inputs[i].tid = i;
        pthread_mutex_init(&queue_mutex[i], NULL);
        pthread_cond_init(&queue_notempty[i], NULL);
        pthread_create(&wtids[i], NULL, write_routine, &inputs[i]);
    }

    pthread_create(&btid, NULL, balance_routine, NULL);

}