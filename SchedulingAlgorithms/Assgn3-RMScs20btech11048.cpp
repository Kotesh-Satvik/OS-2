#include <iostream>
#include <fstream>

using namespace std;

class process
{
    public:
        int p_id;   // process id
        int t;      // processing time (fixed)
        int p;      // period
        int k;      // number of repetitions
        int d;      // deadline
    //  Assumption : deadline is same as the period for each process.
        int flag;
        int rt;     // remaining time
        int rep;    // number of completed repetitions
        int wt;     // waiting time
};

int main()
{
    string filename("input.txt");
    ifstream inp_file (filename);

    int n;
    inp_file >> n;

    process p[n];
    for(int i=0;i<n;i++)
    {
        inp_file >> p[i].p_id >> p[i].t >> p[i].p >> p[i].k;
        p[i].d = p[i].p;
    }

    // Rate monotonic
    ofstream rm_log ("RM-Log.txt");
    ofstream rm_stats ("RM-Stats.txt");

    process x[n];
    for(int i=0;i<n;i++)
    {
        x[i] = p[i];
        x[i].rt = x[i].t;
        x[i].flag = 1;
        x[i].rep = 0;
        x[i].wt=0;
        rm_log << "Process P" << x[i].p_id << ": Processing time = " << x[i].t << "; Deadline = " 
        << x[i].d << "; period = " << x[i].p << " joined the system at time 0." << endl;
    }

    for(int i=0;i<n;i++)
    {
        for(int j=i+1;j<n;j++)
        {
            if(x[j].t < x[i].t)
            {
                process temp = x[i];
                x[i] = x[j];
                x[j] = temp;
            }
        }
    }
    
    int time = 0;
    int index;
    int prev_k = -1;
    int deadline_misses = 0;

    while(1)
    {
        for(int i=0;i<n;i++)
        {
            if(x[i].flag==1)
            {
                if(time==0) rm_log << "Process P" << x[i].p_id << " starts execution at time 0." << endl; 
                else
                {
                    if(index == -1)
                    {
                        rm_log << "CPU is idle till time " << time << "." << endl;
                        rm_log << "Process P" << x[i].p_id << " starts execution at time " << time+1 << "." << endl;                        
                    }
                    else if(i!=index && x[index].rt==x[index].t)
                    {
                        if(x[i].rt==x[i].t)
                        rm_log << "Process P" << x[i].p_id << " starts execution at time " << time+1 << "." << endl;
                        else
                        rm_log << "Process P" << x[i].p_id << " resumes execution at time " << time+1 << "." << endl;
                    } 
                    else if(index!=i && x[index].rt!=x[index].t)
                    {
                        rm_log << "Process P" << x[index].p_id << " is preempted by Process P" << x[i].p_id << " at time "
                        << time << ". Remaining process time : " << x[index].rt << endl;
                        rm_log << "Process P" << x[i].p_id << " starts execution at time " << time+1 << "." << endl;
                    }   
                    else if(index==i && prev_k!=x[i].rep)
                    {
                      rm_log << "Process P" << x[i].p_id << " starts execution at time " << time+1 << "." << endl;
                    }                 
                }
                
                x[i].rt--;
                if(x[i].rt==0)
                {
                    x[i].flag = 0;
                    x[i].rt = x[i].t;
                    x[i].rep++;
                    rm_log << "Process P" << x[i].p_id << " finishes execution at time " << time+1 << "." << endl;
                }
                prev_k = x[i].rep;
                index = i;
                break;
            }
        }
        time++;

        for(int i=0;i<n;i++)
        {
          if(x[i].flag==1 && i!=index)
          {
            x[i].wt++;
          }
        }

        for(int i=0;i<n;i++)
        {
            for(int j=1;j<=x[i].k;j++)
            {
                if(time==x[i].p*j)
                {
                    // cout << x[i].flag << endl;
                    if(x[i].flag==1)
                    {
                      deadline_misses++;
                      if(index==i)
                      rm_log << "Process P" << x[i].p_id << " is terminated at time " << time << " as it fails to meet the deadline." << endl;
                      else 
                      rm_log << "Process P" << x[i].p_id << " fails to meet the deadline by time " << time << "."<< endl;
                      x[i].rep++;
                      x[i].rt = x[i].t;
                      x[i].flag = 0;
                    }
                    if(j!=x[i].k)
                    {
                      x[i].flag = 1;
                    }
                }
            }
        }

        // Condition when no process is being executed.
        int max_flag = 0;
        for(int i=0;i<n;i++)
        {
            if(x[i].flag==1) max_flag = 1;
        }
        int status = 1;
        {
            for(int i=0;i<n;i++)
            {
                if(x[i].rep!= x[i].k)
                status = 0;
            }
        }
        if(max_flag==0)
        {
            if(status==1) break;
            else
            {
                index = -1;
            }
        }
    }

    int num = 0;
    for(int i=0;i<n;i++) num = num + x[i].k;
    rm_stats << "Number of processes that came into the system : " << num << "." << endl;
    rm_stats << "Number of processes that successfully completed : " << num-deadline_misses << "." << endl;
    rm_stats << "Number of process that missed their deadlines : " << deadline_misses << "." << endl;

    rm_stats << "\nAVERAGE WAITING TIMES : " << endl;
    for(int i=0;i<n;i++)
    {
      double wt = (double)x[i].wt/x[i].k;
      rm_stats << "Average waiting time of P" << x[i].p_id << " : " << wt << "." <<endl;
    }

    double avg_wt = 0;
    for(int i=0;i<n;i++)
    {
        avg_wt += (double)x[i].wt;
    }
    avg_wt = avg_wt/num;

    rm_stats << "\nAverage waiting time for a single process is : " << avg_wt << "." << endl;


    rm_log.close();
    rm_stats.close();

    return 0;
}
