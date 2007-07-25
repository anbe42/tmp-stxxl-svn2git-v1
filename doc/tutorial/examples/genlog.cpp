/***************************************************************************
 *            genlog.cpp
 *
 *  Mon Jun 21 14:54:54 2004
 *  Copyright  2004  Roman Dementiev
 *  dementiev@mpi-sb.mpg.de
 ****************************************************************************/

#include <stxxl.h>

#include <fstream>
#include <ctime>

struct LogEntry
{
  long long unsigned from; // callers number
  long long unsigned to;   // destination number
  time_t timestamp;        // time of event
  int event;               // event type 1 - call started
                           //            2 - call ended
};

struct cmp
{
	bool operator () (const LogEntry & a, const LogEntry & b) const
	{
		return a.timestamp < b.timestamp ;
	}
	static LogEntry min_value()
	{
		LogEntry e; e.timestamp = (std::numeric_limits<time_t>::min)(); return e;
	}
	static LogEntry max_value()
	{
		LogEntry e; e.timestamp = (std::numeric_limits<time_t>::max)(); return e;
	}
};

typedef stxxl::VECTOR_GENERATOR<LogEntry,1,1>::result vector_type;

std::ostream & operator << (std::ostream & i, const LogEntry & entry)
{
  i << entry.from << " ";
  i << entry.to << " ";
  i << entry.timestamp<< " ";
  i << entry.event << "\n";
  return i;
}


int main(int argc, char * argv[])
{
  if(argc < 5)
  {
	  STXXL_MSG("Usage: "<<argv[0]<<" ncalls avcalls main logfile")
	  STXXL_MSG(" ncalls  - number of calls")
	  STXXL_MSG(" avcalls - average number of calls per client")
	  STXXL_MSG(" main    - memory to use (in MB)")
	  STXXL_MSG(" logfile - file name of the output")
	  
	  return 0;
  }
  unsigned M = atol(argv[3])*1024*1024;
  const long long int ncalls = atoll(argv[1]);
  const int av_calls = atol(argv[2]);
  const long long int nclients = ncalls/av_calls;
  long long int calls_made = 0;
  
  time_t now = time(NULL);

  vector_type log;
  log.reserve(2*ncalls);
  
  stxxl::random_number<> rnd;

  for(	long long int number = 0; 
	  	number < nclients && calls_made < ncalls; 
  		++number )
  {
    unsigned serv = (std::min)((long long int)rnd(av_calls*2),(ncalls - calls_made));
    LogEntry e;
    e.from = number;

    time_t cur = now;

    while(serv-- > 0)
    {
      	cur += 1 + rnd(3600*24);

      	e.to = rnd(nclients);
      	e.timestamp = cur;

		++calls_made;
		e.event = 1;
		log.push_back(e);
	
		cur += 1 + rnd(1800);
		e.timestamp = cur;
		e.event = 2;
	
		log.push_back(e);
    }
  }
  
  // sort events in order of their appereance
  stxxl::sort(log.begin(),log.end(),cmp(),M); 
  
  std::fstream out(argv[4],std::ios::out);

  std::copy(log.begin(),log.end(),std::ostream_iterator<LogEntry>(out));
  
  STXXL_MSG("\n"<<calls_made<<" calls made.")
  STXXL_MSG("The log is written to '"<<argv[4]<<"'.")
  
  return 0;
}
