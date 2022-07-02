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
        int wt;      // waiting time
};

int priorityIndex(process* x, int n)
{
    int index = 0;
    for(int i=0;i<n;i++)
    {
      if(x[i].flag==1) index = i;
    }
    for(int i=0;i<n;i++)
    {
        if(x[i].flag==1 && x[i].d <= x[index].d)
        {
            index = i;
        }
    }
    if(x[index].flag==0) return -1;
    return index;
}

int main()
{

    string filename ("input.txt");
    ifstream inp_file(filename);

    int n;
    inp_file >> n;
    // cin >> n;

    process p[n];
    for(int i=0;i<n;i++)
    {
        inp_file >> p[i].p_id >> p[i].t >> p[i].p >> p[i].k;
        p[i].d = p[i].p;
    }

    // Earliest Deadline First
    ofstream edf_log ("EDF-Log.txt");
    ofstream edf_stats ("EDF-Stats.txt");

    process x[n];
    for(int i=0;i<n;i++)
    {
        x[i] = p[i];
        x[i].rt = x[i].t;
        x[i].flag = 1;
        x[i].rep = 0;
        x[i].wt = 0;
        edf_log << "Process P" << x[i].p_id << ": Processing time = " << x[i].t << "; Deadline = " 
        << x[i].d << "; period = " << x[i].p << " joined the system at time 0." << endl;
    }


    int time = 0;
    int index;
    int prev_index;
    int prev_k;
    int deadline_misses = 0;

    while(1)
    { 
      index = priorityIndex(x,n);
      // cout << time << " - " << index << endl;
      if(x[index].flag==1)
      {
          if(time==0) edf_log << "Process P" << x[index].p_id << " starts execution at time 0." << endl; 
          else
          {
              if(prev_index == -1)
              {
                  edf_log << "CPU is idle till time " << time << "." << endl;
                  edf_log << "Process P" << x[index].p_id << " starts execution at time " << time+1 << "." << endl;                        
              }
              else if(index!=prev_index && x[prev_index].rt!= x[prev_index].t) 
              {
                  edf_log << "Process P" << x[prev_index].p_id << " is preempted by Process P" << x[index].p_id << " at time "
                  << time << ". Remaining process time : " << x[prev_index].rt << endl;
                  edf_log << "Process P" << x[index].p_id << " starts execution at time " << time+1 << "." << endl;
              }
              else if(index!=prev_index && x[prev_index].rt == x[prev_index].t)
              {
                  if(x[index].rt==x[index].t)
                  edf_log << "Process P" << x[index].p_id << " starts execution at time " << time+1 << "." << endl;
                  else
                  edf_log << "Process P" << x[index].p_id << " resumes execution at time " << time+1 << "." << endl;
              } 
              else if(index==prev_index && prev_k!=x[index].rep)
              {
                edf_log << "Process P" << x[index].p_id << " starts execution at time " << time+1 << "." << endl;
              }
          }
          
          x[index].rt--;
          if(x[index].rt==0)
          {
              x[index].flag = 0;
              x[index].rt = x[index].t;
              x[index].rep++;
              edf_log << "Process P" << x[index].p_id << " finishes execution at time " << time+1 << "." << endl;
          }
          prev_index = index;
          prev_k = x[index].rep;
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
              if(x[i].flag==1)
              {
                deadline_misses++;
                if(index==i)
                edf_log << "Process P" << x[i].p_id << " is terminated at time " << time << " as it fails to meet the deadline." << endl;
                else
                edf_log << "Process P" << x[i].p_id << " fails to meet the deadline by time " << time << "."<< endl;
                x[i].rep++;
                x[i].flag = 0;
                x[i].rt = x[i].t;
              }
              if(j!=x[i].k)
              {
                x[i].flag = 1;
                x[i].d = x[i].p*(j+1);
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
    
    // Condition to break out of the loop
    if(max_flag==0)
    {
      if(status==1) break;
      else
      {
          prev_index = -1;
      }
    }
  }

  int num = 0;
    for(int i=0;i<n;i++) num = num + x[i].k;
    edf_stats << "Number of processes that came into the system : " << num << "." << endl;
    edf_stats << "Number of processes that successfully completed : " << num-deadline_misses << "." << endl;
    edf_stats << "Number of process that missed their deadlines : " << deadline_misses << "." << endl;

    edf_stats << "\nAVERAGE WAITING TIMES : " << endl;
    for(int i=0;i<n;i++)
    {
      double wt = (double)x[i].wt/x[i].k;
      edf_stats << "Average waiting time of P" << x[i].p_id << " : " << wt << "." <<endl;
    }
    
    double avg_wt = 0;
    for(int i=0;i<n;i++)
    {
        avg_wt += (double)x[i].wt;
    }
    avg_wt = avg_wt/num;

    edf_stats << "\nAverage waiting time for a single process is : " << avg_wt << "." << endl;


  edf_log.close();
  edf_stats.close();

  return 0;
}
