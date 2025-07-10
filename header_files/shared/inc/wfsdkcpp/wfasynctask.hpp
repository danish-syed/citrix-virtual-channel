#pragma once


/// a simple class which packages up a lambda expression or other callback function into an
/// easy to use interface for user applications to queue and dispatch on their own event loops.
class wfasynctask {
public:

	/// the callback function contract type
	using RunnerFn = std::function<bool(void)>;

	/// constructor.
	wfasynctask(std::size_t id, RunnerFn&& fn)
		: _id(id)
		, _fn(std::move(fn)) {
	}

	/// returns a work queue id that can be used by applications to separate and prioritize different types of callbacks.
	std::size_t id() { return _id; }
	
	/// execute the callback function.
	void run() { (void)_fn(); }

private:

	RunnerFn _fn; /// the callback function
	std::size_t _id; /// the work queue id

};