#pragma once

#include <functional>
#include <unordered_set>


namespace vcsdk {

/// work-around. function objects cannot be stored in a container, because they cannot be compared. therefore,
/// this class represents a "tagged" function with an identifier value being used as the tag. since the tag can
/// be compared, the tag along with it's associated function can be store in a container.
template <typename callback_type>
class event_listener : private std::pair<unsigned int, callback_type> {
public:

	// inherit STL pair constructor.
	using std::pair<unsigned int, callback_type>::pair;

	/// retrieve the identifier for the event listener.
	const unsigned int& id() const { return this->first; }

	/// retrieve the function callback for the event listener.
	const callback_type& get() const { return this->second; }

	/// compare event listeners by id.
	friend bool operator ==(const event_listener& lhs, const event_listener& rhs) {
		return lhs.id() == rhs.id();
	}

	/// hash event listeners by id.
	struct hash {
		std::size_t operator()(const event_listener& listener) const {
			return std::hash<unsigned int>{}(listener.id());
		}
	};
};

} // namespace vcsdk