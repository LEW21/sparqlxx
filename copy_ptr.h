#include <memory>

namespace sparqlxx
{
	template <typename T>
	struct [[gnu::visibility("default")]] copy_ptr: public std::unique_ptr<T>
	{
		copy_ptr() {}

		copy_ptr(const copy_ptr& o): std::unique_ptr<T>(o ? std::make_unique<T>(*o) : nullptr) {}
		copy_ptr(copy_ptr&& o): std::unique_ptr<T>(std::move(o)) {}

		copy_ptr(const std::unique_ptr<T>& o): std::unique_ptr<T>(o ? std::make_unique<T>(*o) : nullptr) {}
		copy_ptr(std::unique_ptr<T>&& o): std::unique_ptr<T>(std::move(o)) {}

		auto operator=(const copy_ptr& o) {std::unique_ptr<T>::operator=(o ? std::make_unique<T>(*o) : nullptr); return *this;}
		auto operator=(copy_ptr&& o) {std::unique_ptr<T>::operator=(std::move(o)); return *this;}

		auto operator=(const std::unique_ptr<T>& o) {std::unique_ptr<T>::operator=(o ? std::make_unique<T>(*o) : nullptr); return *this;}
		auto operator=(std::unique_ptr<T>&& o) {std::unique_ptr<T>::operator=(std::move(o)); return *this;}

		auto operator=(const T& o) {std::unique_ptr<T>::operator=(std::make_unique<T>(o)); return *this;}
		auto operator=(T&& o) {std::unique_ptr<T>::operator=(std::make_unique<T>(std::move(o))); return *this;}
	};
}
