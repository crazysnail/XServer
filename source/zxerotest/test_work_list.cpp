//
//  test_work_list.cpp
//  zxerotest
//
//  Created by zXero on 14-3-18.
//  Copyright (c) 2014Äê zxero. All rights reserved.
//

#include "test_work_list.h"
#include "types.h"
#include "thread.h"
#include "log.h"
#include <queue>
#include <boost/ptr_container/ptr_vector.hpp>
//#include <boost/circular_buffer.hpp>

using namespace zxero;

mutex g_mutex;
condition_variable_any g_cond;
std::queue<int> g_work_list;
bool g_exit = false;

int get_work()
{
    mutex::scoped_lock lock(g_mutex);
    while(g_work_list.size() == 0)
    {
        g_cond.wait(g_mutex);
        if (g_exit)
            return -1;
    }
    
    int w = g_work_list.front();
    g_work_list.pop();
    
    return w;
}

void push_work(int w)
{
    LOG_WARN << w ;
    mutex::scoped_lock lock(g_mutex);
    g_work_list.push(w);
    g_cond.notify_all();
}

void work_thread()
{
    int work = -1;
    while((work = get_work()) != -1)
    {
        LOG_INFO << work;
    }
    
    if (!g_exit)
        LOG_FATAL << "error quit";
}

int main()
{
    int i = 0;
    for (; i < 50000; ++i)
        push_work(i);
    ptr_vector<thread_t> threads_;
    for ( int t = 0; t < 8; ++t )
    {
        threads_.push_back(new thread_t(work_thread, "work"));
        threads_.back().start();
    }
    
    for ( ; i < 1000000; ++i) {
        push_work(i);
    }
    
    while (g_work_list.size()) {
        this_thread::sleep(seconds(1));
    }
    
    this_thread::sleep(seconds(20));
    g_exit = true;
    g_cond.notify_all();
    
    for (unsigned int t = 0; t < threads_.size(); ++t) {
        threads_[t].join();
    }
    
    return 0;
}






