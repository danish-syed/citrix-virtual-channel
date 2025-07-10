#pragma once

#include "wfasynctask.hpp"

/// an async task queue. simplifies multithreaded programs by posting tasks to a main thread.
class wftaskqueue {
public:

	/// destructor.
	virtual ~wftaskqueue() {}

	/// allocate a work queue id. different types of tasks can be posted to different internal queues.
	virtual std::size_t allocate_queue_id(const std::string& label) = 0;

	/// posts the async task to the main thread or event loop and executes it from there.
	virtual void post_task(wfasynctask&& task) = 0;
};