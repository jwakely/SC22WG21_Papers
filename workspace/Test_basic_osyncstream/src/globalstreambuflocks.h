#ifndef GLOBALSTREAMBUFLOCKS_H_
#define GLOBALSTREAMBUFLOCKS_H_

#include <unordered_map>
#include <mutex>
#include <memory>

namespace detail__ {
using spmx=std::shared_ptr<std::mutex>;

class global_streambuf_locks {
	mutable std::mutex mx{};
	std::unordered_map<void *,std::weak_ptr<std::mutex>> thelocks{};
	using guard=std::lock_guard<std::mutex>;
public:
	spmx get_lock(void *sbufptr){
		guard lk{mx};
		spmx result{};
		std::weak_ptr<std::mutex>& mxptr=thelocks[sbufptr];
		if (mxptr.expired()) {
			mxptr=result=std::make_shared<std::mutex>();
		} else {
			result=mxptr.lock();
		}
		return result;
	}
	void release_lock(spmx& sp,void *sbufptr){
		guard lx{mx};
		std::weak_ptr<std::mutex>& mxptr=thelocks[sbufptr];
		sp.reset();
		if (mxptr.expired()){
			thelocks.erase(sbufptr);
		}
	}
	// testing only
	auto size() const {
		guard lx{mx};
		return thelocks.size();
	}
};

extern global_streambuf_locks thelocks;

} /* namespace detail__ */

#endif /* GLOBALSTREAMBUFLOCKS_H_ */
