#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <semaphore.h>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sys/time.h>
#include <random>

class thread_stats // stats of each thread
{
    public:
    double tot_wt;
    struct timeval req_time;
    struct timeval entry_time;
    struct timeval exit_time;
    double wt;
    double max_wt;
};

int nw, nr, kw, kr, read_count;
double mean_cs, mean_rem;
thread_stats* r_stats;
thread_stats* w_stats;
sem_t mutex, rw_mutex, in;
char now[20];

std::mutex mtx;  // mutex lock for printing

std::ofstream output_file;
std::default_random_engine rnd;
std::exponential_distribution<double> d1;
std::exponential_distribution<double> d2;

void getTime()
{
  time_t curr_time;
  curr_time = time(NULL);

  tm *tm_local = localtime(&curr_time);
  sprintf(now,"%.2d:%.2d:%.2d", tm_local->tm_hour, tm_local->tm_min, tm_local->tm_sec);
  
}

void updateStats(int i, int p) // p-0 for reader and p-1 for writer
{
  thread_stats* temp;
  if(p==0)
    temp = &r_stats[i];
  else if(p==1)
    temp = &w_stats[i];

  temp->wt = (temp->entry_time.tv_sec - temp->req_time.tv_sec)*1e6 + (temp->entry_time.tv_usec - temp->req_time.tv_usec);
  temp->tot_wt += temp->wt;
  temp->max_wt = std::max(temp->max_wt,temp->wt);
}

void writer(int index)
{
  std::thread::id t_id = std::this_thread::get_id();
  for(int i=1;i<=kw;i++)
    {
      mtx.lock();
      gettimeofday(&w_stats[i].req_time, NULL);
      getTime();
      output_file << i << " CS request by Writer thread " << t_id << " at " << now << std::endl;
      mtx.unlock();

      sem_wait(&in);
      sem_wait(&rw_mutex);
      sem_post(&in);
      
      mtx.lock();
      gettimeofday(&w_stats[i].entry_time, NULL);
      getTime();
      output_file << i << " CS entry by Writer thread " << t_id << " at " << now << std::endl;
      updateStats(index, 1);
      mtx.unlock();

      std::this_thread::sleep_for(std::chrono::microseconds(1000000*(int)d1(rnd)));

      mtx.lock();
      gettimeofday(&w_stats[i].exit_time, NULL);
      getTime();
      output_file << i << " CS exit by Writer thread " << t_id << " at " << now << std::endl;
      mtx.unlock();

      sem_post(&rw_mutex);
      
      std::this_thread::sleep_for(std::chrono::microseconds(1000000*(int)d2(rnd)));

    }
}

void reader(int index)
{
  std::thread::id t_id = std::this_thread::get_id();
  for(int i=1;i<=kr;i++)
    {
      mtx.lock();
      gettimeofday(&r_stats[i].req_time, NULL);
      getTime();
      output_file << i << " CS request by Reader thread " << t_id << " at " << now << std::endl;
      mtx.unlock();

      // lock
      sem_wait(&in);
      sem_wait(&mutex);
      read_count++;
      if(read_count == 1)
        sem_wait(&rw_mutex);
      sem_post(&in);
      sem_post(&mutex);
      
      mtx.lock();
      gettimeofday(&r_stats[i].entry_time, NULL);
      getTime();
      output_file << i << " CS entry by Reader thread " << t_id << " at " << now << std::endl;
      updateStats(index, 1);
      mtx.unlock();

      std::this_thread::sleep_for(std::chrono::microseconds(1000000*(int)d1(rnd)));

      mtx.lock();
      gettimeofday(&r_stats[i].exit_time, NULL);
      getTime();
      output_file << i << " CS exit by Reader thread " << t_id << " at " << now << std::endl;
      mtx.unlock();
      
      // unlock
      sem_wait(&mutex);
      read_count--;
      if(read_count == 0)
        sem_post(&rw_mutex);
      sem_post(&mutex);
      
      std::this_thread::sleep_for(std::chrono::microseconds(1000000*(int)d2(rnd)));

    }
}

int main()
{
    std::ifstream inp_file; 
    inp_file.open("inp-params.txt");
    inp_file >> nw >> nr >> kw >> kr >> mean_cs >> mean_rem;

    std::vector<std::thread> w_threads;
    std::vector<std::thread> r_threads;
  
    d1 = std::exponential_distribution<double>(1/mean_cs);
    d2 = std::exponential_distribution<double>(1/mean_rem);
  
    w_stats = (thread_stats*)malloc(nw*sizeof(thread_stats));
    r_stats = (thread_stats*)malloc(nr*sizeof(thread_stats));
  
    sem_init(&mutex, 0, 1);
    sem_init(&rw_mutex, 0, 1);
    sem_init(&in, 0, 1);
    read_count = 0;
    
    output_file.open("FairRW-log.txt");
    for(int i=0;i<nw;i++)
    {
      w_stats[i].max_wt = 0;
      w_stats[i].tot_wt = 0;
      w_threads.push_back(std::thread(writer,i));
    }

    for(int i=0;i<nr;i++)
    {
      r_stats[i].max_wt = 0;
      r_stats[i].tot_wt = 0; 
      r_threads.push_back(std::thread(reader,i));    
    }

    for(auto &th : w_threads)
    {
        th.join();
    }
    for(auto &th : r_threads)
    {
        th.join();    
    }

    return 0;
}