#include "tdraw.h"

#include "queue.h"
#include "logger.h"

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MAXTHREADS          (4)
#define MAX_FPS             (60)

#define REPORT_FPS          (1)
#define CALCULATE_PIXEL_OPS (REPORT_FPS & 1)

// Let each thread know it's ID
struct thread_input {
    int tid;
    int pixel_ops;              // Num pixel operations (use semaphore)
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

static int
to_tdx(Pose p)
{
    if (WINSIZE.x <= 0)
        return (0);

    if (p.x < 1)
        return (0);

    if (p.x > WINSIZE.x)
        return (MAXTHREADS - 1);

    return (((p.x - 1) * MAXTHREADS) / (WINSIZE.x));
}

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
 * - set_pixel & remove_pixel
*/
void *
write_routine(void *arg)
{
    // Pixel *pix;
    PixelOp *pop;
    ModelOp *mop;
    int tid;
    struct thread_input *input = (struct thread_input *) arg;
    tid = input->tid;

    while(1)
    {
        /* 
         * Retrieve pixel from queue.
        */
        pthread_mutex_lock(&queue_mutex[tid]);

        // Queue empty, need to wait for something to fill.
        while (!tqueues[tid].len)
            pthread_cond_wait(&queue_notempty[tid], &queue_mutex[tid]);

        mop = queue_pop(&tqueues[tid]);
        pthread_mutex_unlock(&queue_mutex[tid]);

        if (!mop || !mop->pops.len)
            continue;

        // Entry Routine.
        write_entry();

        // Actual Routine.
        while (mop->pops.len)
        {
            pop = queue_pop(&mop->pops);

            switch (pop->op)
            {
                case SET:
                    set_pixel(pop->pixel, pop->opts.pose);
                    break;
                case REMOVE:
                    remove_pixel(pop->pixel, pop->opts.pose);
                    break;
                case MOVE:
                    // This shouldn't happen...
                    break;
                default:
                    break;
            }

            input->pixel_ops++;
            free(pop);
        }

        // Exit Routine
        write_exit();

        free(mop);
    }

    return (NULL);
}

void
draw_entry(void)
{
    // No writer should start
    pthread_mutex_lock(&semaphore.mtx);
    semaphore.drawing = 1;

    // Wait til writers and models are done.
    while (semaphore.writers)
        pthread_cond_wait(&semaphore.write_done, &semaphore.mtx);

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
 * Calculate the number of operations performed by workers.
 *
 * Requires draw_entry().
*/
int
calculate_pixel_operations()
{
    int i;
    int num_operations = 0;

    for (i = 0; i < MAXTHREADS; i++)
    {
        num_operations += inputs[i].pixel_ops;
        inputs[i].pixel_ops = 0;
    }

    return (num_operations);
}

/*
 * Lock the buffer and draw all contents to screen.
*/
void *
draw_routine(void *arg)
{
    (void)arg;
    struct timespec end, start;
    double delta = 0.0, elapsed_time = 0.0, avg_delta = 0.0;
    double fps = 0.0, time_left = 0.0;
    double num_operations = 0.0, total_operations = 0.0;
    int frames = 0;
    double max_time_per_frame = 1.0 / MAX_FPS; // In seconds...

    struct timespec last_fps;

    pthread_detach(pthread_self());

    clock_gettime(CLOCK_MONOTONIC, &last_fps);

    if (REPORT_FPS)
        set_level(CALCULATE_PIXEL_OPS ? LOG_DEBUG : LOG_INFO);

    while(1)
    {
        // Entry Routine.
        draw_entry();

        // Actual Routine.
        clock_gettime(CLOCK_MONOTONIC, &start);
        draw();
        clock_gettime(CLOCK_MONOTONIC, &end);

        // I think this will get compiled out if set to 0.
        if (CALCULATE_PIXEL_OPS)
        {
            num_operations = (double) calculate_pixel_operations();
            total_operations += num_operations;
        }

        // Exit Routine.
        draw_exit();

        // Calculate how long the draw took.
        delta = (end.tv_sec - start.tv_sec)
            + ((end.tv_nsec - start.tv_nsec)  / 1000000000.0);

        if (REPORT_FPS)
        {
            // Increment number of frames & calculate FPS.
            frames++;
            avg_delta += delta;
            elapsed_time = (end.tv_sec - last_fps.tv_sec)
                + ((end.tv_nsec - last_fps.tv_nsec) / 1000000000.0);

            if (elapsed_time >= 1.0)
            {
                fps = frames / elapsed_time;
                avg_delta = avg_delta / frames;

                info("FPS: %.6f (MAX %d)", fps, MAX_FPS);
                debug("\tavg delta / frame: %.6f", avg_delta);
                debug("\ttotal ops: %.0f", total_operations);
                debug("\tavg ops / frame: %.3f", ((total_operations / frames)));
                debug("\tavg ops / thread: %.3f", ((total_operations / MAXTHREADS)));

                frames = 0;
                avg_delta = 0.0;
                total_operations = 0.0;

                clock_gettime(CLOCK_MONOTONIC, &last_fps);
            }
        }

        // Check if we are going too fast...
        time_left = max_time_per_frame - delta;
        if (time_left > 0) 
        {
            // info("Need to sleep %.6f", time_left);
            usleep(time_left * 890000); // ~.89s
        }
    }

    return (NULL);
}

/*
 * Handle library calls to set pixels. Delegates to the correct
 * queue.
*/
void *
balance_routine(Model *model, OperationType op)
{
    // Entry Routine.
    PixelOp *pop;
    // MNode *model_id;
    Pixel *pix;
    ModelOp *mops[MAXTHREADS];

    int rc, tdx;
    int i;

    (void)rc;

    if (!model || !model->len)
        return (NULL);

    if (check_bounds(model->p.p, WINSIZE))
        return (NULL);

    for (tdx = 0; tdx < MAXTHREADS; tdx++)
    {
        mops[tdx] = malloc(sizeof(ModelOp));
    }

    // Separate each pop into its mop.
    for (i = 0; i < model->len; i++)
    {
        pop = malloc(sizeof(PixelOp));
        if (!pop)
            return (NULL);

        pix = model->pixels[i];
        
        pop->opts.pose = add_pose3d(model->p, pix->pose);
        pop->pixel = pix;
        pop->op = op;

        tdx = to_tdx(pop->opts.pose.p);

        queue_push(&mops[tdx]->pops, pop);
    }

    // Delegate
    for (tdx = 0; tdx < MAXTHREADS; tdx++)
    {
        if (!mops[tdx]->pops.len)
            continue;

        pthread_mutex_lock(&queue_mutex[tdx]);
        queue_push(&tqueues[tdx], mops[tdx]);

        if (tqueues[tdx].len == 1)
            pthread_cond_signal(&queue_notempty[tdx]);

        pthread_mutex_unlock(&queue_mutex[tdx]);
    }

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
    for (i = 0; i < MAXTHREADS; i++)
    {
        inputs[i].tid = i;
        inputs[i].pixel_ops = 0;
        tqueues[i].len = 0;
        tqueues[i].head = NULL;
        tqueues[i].tail = NULL;
        pthread_mutex_init(&queue_mutex[i], NULL);
        pthread_cond_init(&queue_notempty[i], NULL);
        pthread_create(&wtids[i], NULL, write_routine, &inputs[i]);
    }

    set_level(LOG_INFO);

}