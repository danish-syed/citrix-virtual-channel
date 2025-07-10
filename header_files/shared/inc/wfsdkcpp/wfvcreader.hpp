#pragma once

#include "wfvirtualchannel.hpp"


class wfvcreader {
private:

    wfvirtualchannel _vc;
    wftaskqueue* _task_queue;
    wfevent _stop_signal;
    std::thread _thread;
    wfvirtualchannel::DataFn _ondata;

public:

    /// constructor.
    wfvcreader(const wfvirtualchannel& vc, wftaskqueue* task_queue, wfvirtualchannel::DataFn&& ondata)
        : _vc(vc)
        , _ondata(std::move(ondata))
        , _task_queue(task_queue)
        , _stop_signal(false, false)
        , _thread(&wfvcreader::thread_proc, this) {
    }

    /// constructor. copy construction is not allowed.
    wfvcreader(const wfvcreader&) = delete;

    /// constructor. move construction is not allowed.
    wfvcreader(wfvcreader&&) = delete;

    /// destructor.
    ~wfvcreader() {
        _stop_signal.set();
        if (_thread.joinable()) {
            _thread.join();
        }
    }

    void set_task_queue(wftaskqueue* task_queue) { _task_queue = task_queue; }

    void thread_proc() {
        while (!_stop_signal.waitFor(0)) {
            // allocate the buffer on the heap for now, to work around the issue that std::function objects
            // dont support lambdas that capture move-only objects. hopefully, a future version of c++ will
            // introduce support for std::unique_function to resolve this.
            using buffer_type = wfuserbuffer<512>;
            auto buf = std::make_shared<buffer_type>();
            if (_vc.read(_stop_signal, *buf, buf->capacity(), buf->mutable_size()) != FALSE && buf->size() > 0) {

                _task_queue->post_task(wfasynctask(_task_queue->allocate_queue_id("vcdata"), [this, buf]() {
                    _ondata(*buf);
                    return true;
                    }));
            }
        }
    }
};