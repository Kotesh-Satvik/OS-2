#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <mutex>

class time
{
    int hr;
    int min;
    int sec;
};

class thread_stats
{
    int tot_wt;
    int wt;
    int max_wt;
};

int n,k,l1,l2,key;

std::atomic_flag TASlock = ATOMIC_FLAG_INIT;
int CASlock(0);
int j;
bool *waiting;
thread_stats* stats;


std::mutex mtx;
std::mutex cas_mtx;
std::ofstream TAS_output, CAS_output, Bounded_CAS_output;

int compare_and_swap(int* value, int expected, int new_value)
{
    int temp = *value;
    if(temp == expected)
    {
        *value = new_value;
    }
    return temp;
}

void printTime(std::ofstream* fp)
{
    time_t curr_time;
    curr_time = time(NULL);

    tm *tm_local = localtime(&curr_time);
    *fp << std::setfill('0') << std::setw(2) << tm_local->tm_hour << ":" << std::setfill('0')
        << std::setw(2) << tm_local->tm_min << ":" << std::setfill('0') << std::setw(2) 
        << tm_local->tm_sec ;
}

void TAStestCS(int index)
{
    std::thread::id t_id = std::this_thread::get_id();
    for(int i=1;i<=k;i++)
    {
        mtx.lock();
        TAS_output << i << "th CS request at ";
        printTime(&TAS_output);
        TAS_output << " by thread " << t_id << std::endl;
        mtx.unlock();

        while(std::atomic_flag_test_and_set_explicit(&TASlock, std::memory_order_acquire));
        mtx.lock();
        TAS_output << i << "th CS entry at ";
        printTime(&TAS_output);
        TAS_output << " by thread " << t_id << std::endl;
        mtx.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        mtx.lock();
        TAS_output << i << "th CS exit at ";
        printTime(&TAS_output);
        TAS_output << " by thread " << t_id << std::endl;
        mtx.unlock();
        std::atomic_flag_clear_explicit(&TASlock, std::memory_order_release);
        std::this_thread::sleep_for(std::chrono::seconds(1));

    }
    

}

void CAStestCS(int index)
{
    std::thread::id t_id = std::this_thread::get_id();
    for(int i=1;i<=k;i++)
    {
        mtx.lock();
        CAS_output << i << "th CS request at ";
        printTime(&CAS_output);
        CAS_output << " by thread " << t_id << std::endl;
        mtx.unlock();
        
        int temp;
        do
        {
            cas_mtx.lock();
            temp = compare_and_swap(&CASlock, 0, 1);
            cas_mtx.unlock();
        }while(temp!=0);
        // while(compare_and_swap(&CASlock, 0, 1)!=0);

        mtx.lock();
        CAS_output << i << "th CS entry at ";
        printTime(&CAS_output);
        CAS_output << " by thread " << t_id << std::endl;
        mtx.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        mtx.lock();
        CAS_output << i << "th CS exit at ";
        printTime(&CAS_output);
        CAS_output << " by thread " << t_id << std::endl;
        mtx.unlock();
        CASlock = 0;
        std::this_thread::sleep_for(std::chrono::seconds(1));

    }
}

void BoundedCAStestCS(int index)
{
    std::thread::id t_id = std::this_thread::get_id();
    for(int i=1;i<=k;i++)
    {
        mtx.lock();
        Bounded_CAS_output << i << "th CS request at ";
        printTime(&Bounded_CAS_output);
        Bounded_CAS_output << " by thread " << t_id << std::endl;
        mtx.unlock();

        waiting[index] = true;
        key = 1;
        while(waiting[index] && key==1)
        {
            cas_mtx.lock();
            key = compare_and_swap(&CASlock,0,1);
            cas_mtx.unlock();
        }
        waiting[index] = false;

        // while(compare_and_swap(&lock, 0, 1)!=0);
        mtx.lock();
        Bounded_CAS_output << i << "th CS entry at ";
        printTime(&Bounded_CAS_output);
        Bounded_CAS_output << " by thread " << t_id << std::endl;
        mtx.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        mtx.lock();
        Bounded_CAS_output << i << "th CS exit at ";
        printTime(&Bounded_CAS_output);
        Bounded_CAS_output << " by thread " << t_id << std::endl;
        mtx.unlock();

        j = (index+1)%n;
        while((j!=index) && !waiting[j])
            j = (j+1)%n;
        if(j==index)
            CASlock = 0;
        else 
            waiting[j] = false;
        std::this_thread::sleep_for(std::chrono::seconds(1));

    }
}

int main()
{

    std::ifstream inp_file;
    inp_file.open("inp-params.txt");
    inp_file >> n >> k >> l1 >> l2;

    waiting = (bool*)malloc(n*sizeof(bool));
    stats = (thread_stats*)malloc(n*sizeof(thread_stats));
    
    for(int i=0;i<n;i++)
    {
        waiting[i] = false;
    }

    TAS_output.open("TAS_ME_Output.txt");
    std::vector<std::thread> threads;
    for(int i=0;i<n;i++)
    {
        threads.push_back(std::thread(TAStestCS,i));
    }

    for (auto &th : threads) 
    {    
        th.join();
    }
    threads.clear();
    TAS_output.close();

    CAS_output.open("CAS_ME_Output.txt");
    for(int i=0;i<n;i++)
    {
        threads.push_back(std::thread(CAStestCS,i));
    }

    for (auto &th : threads) 
    {    
        th.join();
    }
    threads.clear();
    CAS_output.close();

    Bounded_CAS_output.open("Bounded_CAS_ME_Output.txt");
    for(int i=0;i<n;i++)
    {
        threads.push_back(std::thread(BoundedCAStestCS,i));
    }

    for (auto &th : threads) 
    {    
        th.join();
    }
    threads.clear();
    Bounded_CAS_output.close();
    
    return 0;
}
