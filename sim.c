#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "job.h"

#define QUANTUM 100 //milliseconds
#define MAX_JOBS 100

struct job *job_queue[MAX_JOBS]; 
int job_count = 0; 

// feedback queues
struct job *feedback_queues[4][MAX_JOBS];
int queue_sizes[4] = {0, 0, 0, 0}; 

char **global_argv;

int remaining;
struct job *running;//currently running job
void dispatcher(int signum) {
  if (running) {
      int q = QUANTUM;
      if (running->time_remaining < QUANTUM) {
          q = running->time_remaining;
      }
      running->time_remaining -= q;
      remaining -= q;
      if (running->num_interrupts > running->next_interrupt) {
          running->interrupts[running->next_interrupt] -= q;
      }
  }
  else {
    remaining -= QUANTUM;
    return;
  }
  printf("Trying to select a job... job_count = %d\n", job_count);

  //if job completed, record throughput, wait turnaround and
  //normalized turnaround times.
  if (running->time_remaining <= 0) {
      int completion_time = atoi(global_argv[3]) - remaining;
      printf("Job %d completed:\n", running->id);
      printf("  Arrival time: 0\n");
      printf("  Completion time: %d\n", atoi(global_argv[3]) - remaining);
      printf("  Service time: %d\n", running->total_time);
      printf("  Turnaround time: %d\n", atoi(global_argv[3]) - remaining);
      printf("  Normalized turnaround time: %.2f\n", (float)completion_time / running->total_time);
      running = NULL;
  }

  //check for new process arrival with 2% chance.

  //check for I/O completion of all processes 
  for (int i = 0; i < job_count; i++) {
      struct job *j = job_queue[i];

      if (j->num_interrupts > j->next_interrupt) {
          j->interrupts[j->next_interrupt] -= 40;

          if (j->interrupts[j->next_interrupt] <= 0) {
              j->next_interrupt++;
              printf("Job %d completed an i/o operation\n", j->id);

              if (strcmp(global_argv[2], "SRT") == 0) {
                  // re-sort job and move it back to queue
                  int pos = job_count;
                  while (pos > 0 && job_queue[pos - 1]->time_remaining > j->time_remaining) {
                      job_queue[pos] = job_queue[pos - 1];
                      pos--;
                  }
                  job_queue[pos] = j;
                  job_count++;
              } else {
                  feedback_queues[0][queue_sizes[0]] = j;
                  queue_sizes[0]++;
              }
          }
      }
  }

  //select new process
  if (!running) {
      if (strcmp(global_argv[2], "SRT") == 0) {
          if (job_count > 0) {
              running = job_queue[0];
              for (int i = 0; i < job_count - 1; i++) {
                  job_queue[i] = job_queue[i + 1];
              }
              job_count--;
              printf("Job %d selected\n", running->id);
          }
      } else {
          for (int i = 0; i < 4; i++) {
              if (queue_sizes[i] > 0) {
                  running = feedback_queues[i][0];
                  for (int j = 0; j < queue_sizes[i] - 1; j++) {
                      feedback_queues[i][j] = feedback_queues[i][j + 1];
                  }
                  queue_sizes[i]--;
                  printf("Job %d selected\n", running->id);
                  break;
              }
          }
      }
  }
  //Set up new alarm  
    struct itimerval tm;
  tm.it_interval.tv_sec = 0;
  tm.it_interval.tv_usec = 0;
  tm.it_value.tv_sec = 0;
  if (running) {
      //if job hasn't finished, set the alarm based on the remaining time or quantom
      int next_time = (running->time_remaining < QUANTUM) ? running->time_remaining : QUANTUM;
      tm.it_value.tv_usec = next_time * 1000;
  }
  setitimer(ITIMER_VIRTUAL, &tm, NULL);
}


int main (int argc, char **argv) {
  global_argv = argv;

  //process command line arguments.  You should check for validity.
  if (argc != 4) {
      printf("Usage: %s <random_seed> <algorithm> <time>\n", argv[0]);
      return 1;
  }

  if (atoi(argv[1]) <= 0) {
      printf("Please enter a valid random seed integer\n");
      return 1;
  }
  srandom(atoi(argv[1]));

  if (strcmp(argv[2], "SRT") != 0 && strcmp(argv[2], "Feedback") != 0) {
      printf("Algorithm can only be 'Feedback' or 'SRT'\n");
      return 1;
  }

  if (atoi(argv[3]) <= 0) {
      printf("Time must be positive.\n");
      return 1;
  }
  remaining = atoi(argv[3]);

int init_jobs = 6;
struct job *j = generate_next_job();
// generate initial jobs
  for (int i = 0; i < init_jobs; i++) {  
      struct job *j = generate_next_job();
      if (strcmp(argv[2], "SRT") == 0) {
          int position = job_count;
          while (position > 0 && job_queue[position - 1]->time_remaining > j->time_remaining) {
              job_queue[position] = job_queue[position - 1];
              position--;
          }
          job_queue[position] = j;
          job_count++;
      } else {
          feedback_queues[0][queue_sizes[0]] = j;
          queue_sizes[0]++;
      }
  }

  //set the alarm for next event
while (remaining > 0) {
    dispatcher(0); 
    usleep(QUANTUM * 1000); 
}  struct itimerval tm; 
  tm.it_interval.tv_sec = 0; 
  tm.it_interval.tv_usec = 0;
  tm.it_value.tv_sec=0; 
  //if we will block for I/O before quantum expires, set timer 
  tm.it_value.tv_usec = 1000*(j->interrupts[j->next_interrupt] < QUANTUM ? j->interrupts[j->next_interrupt] : QUANTUM); 
  //Note that this ignores the possibility that the the job might complete
  //before the quantum expires.  You will need to fix that.
  setitimer(ITIMER_VIRTUAL,&tm,NULL); 
  if (setitimer(ITIMER_VIRTUAL, &tm, NULL) == -1) {
      perror("setitimer failed");
  } else {
      printf("settimr successful\n");
  }
  return 0; 
}

