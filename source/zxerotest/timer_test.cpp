#include "app.h"
#include "module.h"
#include "network.h"
#include "event_loop_thread.h"
#include "log.h"
#include <stdio.h>

using namespace zxero;
using namespace zxero::network;

int cnt = 0;
event_loop* g_loop = nullptr;

void printTid()
{
	//printf("[%s][%d] tid\n", to_iso_extended_string(now()).c_str(), this_thread::get_id());
    LOG_INFO << "[" << to_iso_extended_string(now()) << "][" << this_thread::get_id() << "] tid";
}

void print(const char* msg)
{
	//printf("[%s][%d] %s\n", to_iso_extended_string(now()).c_str(), this_thread::get_id(), msg);
    LOG_INFO << "[" << to_iso_extended_string(now()) << "][" << this_thread::get_id() << "] " << msg;
	if (++cnt >= 20)
	{
		g_loop->quit();
	}
}

void cancel(timer_id tid)
{
	g_loop->cancel(tid);
	//printf("[%s][%d] canceled\n", to_iso_extended_string(now()).c_str(), this_thread::get_id());
    LOG_INFO << "[" << to_iso_extended_string(now()) << "][" << this_thread::get_id() << "] canceled";
}

int main(void)
{
	printTid();
	this_thread::sleep(seconds(1));
	{
		event_loop loop;
		g_loop = &loop;

		print("main");
		loop.run_after(seconds(1), bind(print, "once1"));
		loop.run_after(seconds(1)+milliseconds(500), bind(print, "once1.5"));
		loop.run_after(seconds(2) + milliseconds(500), bind(print, "once2.5"));
		loop.run_after(seconds(3) + milliseconds(500), bind(print, "once3.5"));
		timer_id t45 = loop.run_after(seconds(4) + milliseconds(500), bind(print, "once4.5"));
		loop.run_after(seconds(4) + milliseconds(200), bind(cancel, t45));
		loop.run_after(seconds(4) + milliseconds(700), bind(cancel, t45));
		loop.run_every(seconds(2), bind(print, "every2"));
		timer_id t3 = loop.run_every(seconds(3), bind(print, "every3"));
		loop.run_after(seconds(9) + milliseconds(1), bind(cancel, t3));

		loop.loop();
		print("main loop exits");
	}
	{
		event_loop_thread loop_thread;
		event_loop* loop = loop_thread.start_loop();
		loop->run_after(seconds(2), printTid);
		this_thread::sleep(seconds(3));
		print("thread loop exits");
	}

	return 0;
}
