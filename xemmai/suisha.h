#ifndef XEMMAIX__SUISHA__SUISHA_H
#define XEMMAIX__SUISHA__SUISHA_H

#include <suisha/loop.h>
#include <xemmai/convert.h>

namespace xemmaix::suisha
{

using namespace ::suisha;
using namespace xemmai;

class t_wait;
class t_timer;
class t_loop;

class t_library : public xemmai::t_library
{
	t_slot_of<t_type> v_type_wait;
	t_slot_of<t_type> v_type_timer;
	t_slot_of<t_type> v_type_loop;

	static void f_main(t_library* a_library, const t_pvalue& a_callable);

public:
	using xemmai::t_library::t_library;
	virtual void f_scan(t_scan a_scan);
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define();
	template<typename T>
	const T* f_library() const
	{
		return f_global();
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return f_global()->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_library*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_pvalue f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_library<typename t::t_library>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_library* t_library::f_library<t_library>() const
{
	return this;
}

XEMMAI__LIBRARY__TYPE(t_library, wait)
XEMMAI__LIBRARY__TYPE(t_library, timer)
XEMMAI__LIBRARY__TYPE(t_library, loop)

}

#endif
