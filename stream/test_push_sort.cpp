/***************************************************************************
 *  stream/test_push_sort.cpp
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2004 Roman Dementiev <dementiev@mpi-sb.mpg.de>
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

//! \example stream/test_push_sort.cpp
//! This is an example of how to use some basic algorithms from
//! stream package. This example shows how to create
//! \c sorted_runs data structure
//! using \c stream::use_push specialization of \c stream::runs_creator class

#include "stxxl/stream"


typedef unsigned value_type;


struct Cmp : public std::binary_function<value_type, value_type, bool>
{
    typedef unsigned value_type;
    bool operator () (const value_type & a, const value_type & b) const
    {
        return a < b;
    }
    value_type max_value()
    {
        return 0xffffffff;
    }
    value_type min_value()
    {
        return 0x0;
    }
};

int main()
{
    // special parameter type
    typedef stxxl::stream::use_push<value_type> InputType;
    typedef stxxl::stream::runs_creator<InputType, Cmp, 4096, stxxl::RC> CreateRunsAlg;
    typedef CreateRunsAlg::sorted_runs_type SortedRunsType;

    unsigned size = (30 * 1024 * 128 / (sizeof(value_type) * 2));

    unsigned i = 0;

    Cmp c;
    CreateRunsAlg SortedRuns(c, 1024 * 128);
    value_type oldcrc(0);

    stxxl::random_number32 rnd;
    //stxxl::random_number<> rnd_max;
    unsigned cnt = size;
    while (cnt > 0)
    {
        const value_type tmp = rnd();
        oldcrc += tmp;
        SortedRuns.push(tmp);         // push into the sorter
        --cnt;
    }

    SortedRunsType Runs = SortedRuns.result();     // get sorted_runs data structure
    assert(stxxl::stream::check_sorted_runs(Runs, Cmp()));

    // merge the runs
    stxxl::stream::runs_merger<SortedRunsType, Cmp> merger(Runs, Cmp(), 1024 * 128 / 10);
    stxxl::vector<value_type> array;
    STXXL_MSG(size << " " << Runs.elements);
    STXXL_MSG("CRC: " << oldcrc);
    value_type crc(0);
    for (i = 0; i < size; ++i)
    {
        crc += *merger;
        array.push_back(*merger);
        ++merger;
    }
    STXXL_MSG("CRC: " << crc);
    assert(stxxl::is_sorted(array.begin(), array.end(), Cmp()));
    assert(merger.empty());

    return 0;
}
