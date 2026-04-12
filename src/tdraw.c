#include "tdraw.h"

#include "queue.h"

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXTHREADS          1

#define TO_TDX(row, col)    ((row - 1) / (WINSIZE.x / MAXTHREADS))

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
// static pthread_t btid;               // Balancing thread.
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
    // Pixel *pix;
    PixelOp *pop;
    int tid;
    struct thread_input *input = (struct thread_input *) arg;
    tid = input->tid;

    while(1)
    {
        // Retrieve pixel from queue.
        pthread_mutex_lock(&queue_mutex[tid]);

        // Queue empty, need to wait for something to fill.
        while (!tqueues[tid].len)
        {
            pthread_cond_wait(&queue_notempty[tid], &queue_mutex[tid]);
        }

        pop = queue_pop(&tqueues[tid]);
        pthread_mutex_unlock(&queue_mutex[tid]);

        if (!pop || !pop->pixel)
            continue;

        // Entry Routine.
        write_entry();

        // Actual Routine.
        switch (pop->op) {
            case SET:
                set_pixel(pop->pixel);
                break;
            case REMOVE:
                remove_pixel(pop->pixel);
                break;
            case MOVE:
                /* Erase where the pixel was. */
                remove_pixel(pop->pixel);

                Pose t = add_pose(pop->pixel->pose.p, pop->opts.offset);
                pop->pixel->pose.p = t;

                /* Fill where pixel is now*/
                set_pixel(pop->pixel);
                break;
            default:
                break;
        }

        // Exit Routine
        write_exit();

        // Free the PixelOperation.
        free(pop);
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

        usleep(100000);
    }

    return (NULL);
}

/*
 * Handle library calls to set pixels. Delegates to the correct
 * queue.
*/
void *
balance_routine(PixelOp *pop)
{
    // Entry Routine.
    QNode *node;
    int rc, tdx;

    (void)rc;

    if (!pop || !pop->pixel)
        return (NULL);

    if (check_bounds(pop->pixel->pose.p, WINSIZE))
    {
        return (NULL);
    }

    // The screen is divided horizontally.
    tdx = TO_TDX(pop->pixel->pose.p.x, pop->pixel->pose.p.y);
    node = create_qnode(pop);

    if (!node)
    {
        return (NULL);
    }

    // Ideally a try lock here...
    pthread_mutex_lock(&queue_mutex[tdx]);
    queue_push(&tqueues[tdx], node);

    if (tqueues[tdx].len == 1)
        pthread_cond_signal(&queue_notempty[tdx]);

    pthread_mutex_unlock(&queue_mutex[tdx]);

    return (NULL);
}

void
start_tdraw(void)
{
    int i;

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
        tqueues[i].len = 0;
        tqueues[i].head = NULL;
        tqueues[i].tail = NULL;
        pthread_mutex_init(&queue_mutex[i], NULL);
        pthread_cond_init(&queue_notempty[i], NULL);
        pthread_create(&wtids[i], NULL, write_routine, &inputs[i]);
    }

    // pthread_create(&btid, NULL, balance_routine, NULL);

}