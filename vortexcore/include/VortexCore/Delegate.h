#pragma once
#include <functional>
#include <ppl.h>
#include <concrt.h>
#include <concurrent_vector.h>
#include <list>
#include <future>
#include <thread>




//normal delegate
// TODO: special list for functions that have no params

//TODO: wrap functions with a shared_ptr to allow deletion
// + operator returns a shared ptr as handle
// container also only stores shared ptrs to allow find

namespace Vt {
		struct Async {

		};


		template<class R, class... Args> class Delegate {

		public:
			Delegate () {
			}

			~Delegate () {
			}

			std::shared_ptr<std::function < typename R (typename Args...)>> operator+=(std::function<typename R (typename Args...)> fun) {
				mLock.lock_read ();
				auto f = std::make_shared < std::function < typename R (typename Args...)>> (fun);
				mDelegates.push_back (f);
				mLock.unlock ();
				return f;
			}

			Delegate& operator-=(std::shared_ptr < std::function < typename R (typename Args...) >> fun) {
				auto r = std::find (mDelegates.begin (), mDelegates.end (), fun);
				mLock.lock ();
				(*r) = nullptr;
				mLock.unlock ();
				return *this;
			}


			template <typename LTy = void, typename T = R>
			typename std::enable_if<std::is_void<R>::value == false && std::is_same<LTy, Async>::value == false, std::list<T>>::type
				operator() (Args... args) {
					return this->invoke (args...);
				}

			template <typename LTy = void, typename T = R>
			typename std::enable_if<std::is_void<R>::value == true && std::is_same<LTy, Async>::value == false, T>::type
				operator() (Args... args) {
					this->invoke (args...);
				}

			template <typename LTy, typename T = R>
			typename std::enable_if<std::is_void<R>::value == true && std::is_same<LTy, Async>::value == true, T>::type
				operator() (Args... args) {
					this->invoke<LTy> (args...);
				}

			template <typename LTy, typename T = R>
			typename std::enable_if < std::is_void<R>::value == false && std::is_same<LTy, Async>::value == true, std::list<std::future<typename T>>>::type
				operator() (Args... args) {
					return this->invoke<LTy, T> (args...);
				}
		private:
			//serial version
			template <typename LTy = void, typename T = R>
			typename std::enable_if<std::is_void<R>::value == false && std::is_same<LTy, Async>::value == false, std::list<T>>::type
				invoke (Args... args) {
					mLock.lock_read ();
					std::list<T> r;
					for (auto i : mDelegates) {
						if (i != nullptr)
							r.emplace_back ((*i) (args...));
					}
					mLock.unlock ();
					return r;
				}

			template <typename LTy = void, typename T = R>
			typename std::enable_if<std::is_void<R>::value == true && std::is_same<LTy, Async>::value == false, T>::type
				invoke (Args... args) {
					mLock.lock_read ();
					for (auto i : mDelegates) {
						if (i != nullptr)
							(*i) (args...);
					}
					mLock.unlock ();
				}
			//async version
			template <typename LTy, typename T = R>
			typename std::enable_if<std::is_void<R>::value == true && std::is_same<LTy, Async>::value == true, T>::type
				invoke (Args... args) {
					mLock.lock_read ();
					for (auto i : mDelegates) {
						if (i != nullptr)
							std::async (std::launch::async, (*i), args...);
					}
					mLock.unlock ();
				}

			template <typename LTy, typename T = R>
			typename std::enable_if < std::is_void<R>::value == false && std::is_same<LTy, Async>::value == true, std::list < std::future<typename T >> >::type
				invoke (Args... args) {
					mLock.lock_read ();
					std::list<std::future<T>> r;
					for (auto i : mDelegates) {
						if (i != nullptr)
							r.emplace_back (std::async (std::launch::async, (*i), args...));
					}
					mLock.unlock ();
					return r;
				}
			Concurrency::concurrent_vector < std::shared_ptr < std::function < typename R (typename Args...) >> > mDelegates;
			Concurrency::reader_writer_lock mLock;
		};
}

