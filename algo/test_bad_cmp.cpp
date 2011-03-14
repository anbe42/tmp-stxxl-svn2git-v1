/***************************************************************************
 *  algo/test_bad_cmp.cpp
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2002 Roman Dementiev <dementiev@mpi-sb.mpg.de>
 *  Copyright (C) 2011 Andreas Beckmann <beckmann@cs.uni-frankfurt.de>
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

//! \example algo/test_bad_cmp.cpp
//! This is an example of how NOT to use \c stxxl::sort() algorithm.
//! Here min_value and max_value are used as keys which is forbidden.

#include <stxxl/mng>
#include <stxxl/sort>
#include <stxxl/vector>


struct my_type
{
    typedef unsigned key_type;

    key_type _key;
    key_type _data;
    key_type key() const
    {
        return _key;
    }

    my_type() { }
    my_type(key_type __key) : _key(__key), _data(0) { }

    static my_type min_value()
    {
        return my_type((std::numeric_limits<key_type>::min)());
    }
    static my_type max_value()
    {
        return my_type((std::numeric_limits<key_type>::max)());
    }

    ~my_type() { }
};

std::ostream & operator << (std::ostream & o, const my_type & obj)
{
    o << obj._key;
    return o;
}

bool operator < (const my_type & a, const my_type & b)
{
    return a.key() < b.key();
}

bool operator == (const my_type & a, const my_type & b)
{
    return a.key() == b.key();
}

bool operator != (const my_type & a, const my_type & b)
{
    return a.key() != b.key();
}

struct cmp : public std::less<my_type>
{
    my_type min_value() const
    {
        return my_type::min_value();
    }
    my_type max_value() const
    {
        return my_type::max_value();
    }
};


int main()
{
#if STXXL_PARALLEL_MULTIWAY_MERGE
    STXXL_MSG("STXXL_PARALLEL_MULTIWAY_MERGE");
#endif
    unsigned memory_to_use = 128 * 1024 * 1024;
    typedef stxxl::vector<my_type> vector_type;

    const stxxl::int64 n_records =
        stxxl::int64(384) * stxxl::int64(1024 * 1024) / sizeof(my_type);
    vector_type v(n_records);

    stxxl::int64 aliens, not_stable;
    int bs = vector_type::block_type::size;

    STXXL_MSG("Filling vector with min_value..., input size = " << v.size() << " elements (" << ((v.size() * sizeof(my_type)) >> 20) << " MiB)");
    for (vector_type::size_type i = 0; i < v.size(); i++) {
        v[i]._key = 0;
        v[i]._data = i + 1;
    }

    STXXL_MSG("Checking order...");
    STXXL_MSG(((stxxl::is_sorted(v.begin(), v.end(), cmp())) ? "OK" : "WRONG"));

    STXXL_MSG("Sorting (using " << (memory_to_use >> 20) << " MiB of memory)...");
    stxxl::sort(v.begin(), v.end(), cmp(), memory_to_use);

    STXXL_MSG("Checking order...");
    STXXL_MSG(((stxxl::is_sorted(v.begin(), v.end(), cmp())) ? "OK" : "WRONG"));

    aliens = not_stable = 0;
    for (vector_type::size_type i = 0; i < v.size(); i++) {
        if (v[i]._data < 1)
            ++aliens;
        else if (v[i]._data != i + 1)
            ++not_stable;
        v[i]._data = i + 1;
    }
    STXXL_MSG("elements that were not in the input:     " << aliens);
    STXXL_MSG("elements not on their expected location: " << not_stable);

    STXXL_MSG("Sorting subset (using " << (memory_to_use >> 20) << " MiB of memory)...");
    stxxl::sort(v.begin() + bs - 1, v.end() - bs + 2, cmp(), memory_to_use);

    STXXL_MSG("Checking order...");
    STXXL_MSG(((stxxl::is_sorted(v.begin(), v.end(), cmp())) ? "OK" : "WRONG"));

    aliens = not_stable = 0;
    for (vector_type::size_type i = 0; i < v.size(); i++) {
        if (v[i]._data < 1)
            ++aliens;
        else if (v[i]._data != i + 1)
            ++not_stable;
        v[i]._data = i + 1;
    }
    STXXL_MSG("elements that were not in the input:     " << aliens);
    STXXL_MSG("elements not on their expected location: " << not_stable);

    STXXL_MSG("Filling vector with max_value..., input size = " << v.size() << " elements (" << ((v.size() * sizeof(my_type)) >> 20) << " MiB)");
    for (vector_type::size_type i = 0; i < v.size(); i++) {
        v[i]._key = unsigned(-1);
        v[i]._data = i + 1;
    }

    STXXL_MSG("Sorting subset (using " << (memory_to_use >> 20) << " MiB of memory)...");
    stxxl::sort(v.begin() + bs - 1, v.end() - bs + 2, cmp(), memory_to_use);

    STXXL_MSG("Checking order...");
    STXXL_MSG(((stxxl::is_sorted(v.begin(), v.end(), cmp())) ? "OK" : "WRONG"));

    aliens = not_stable = 0;
    for (vector_type::size_type i = 0; i < v.size(); i++) {
        if (v[i]._data < 1)
            ++aliens;
        else if (v[i]._data != i + 1)
            ++not_stable;
        v[i]._data = i + 1;
    }
    STXXL_MSG("elements that were not in the input:     " << aliens);
    STXXL_MSG("elements not on their expected location: " << not_stable);

    STXXL_MSG("Done, output size=" << v.size() << " block size=" << bs);

    return 0;
}

// vim: et:ts=4:sw=4
