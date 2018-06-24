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

class t_extension : public xemmai::t_extension
{
	t_slot_of<t_type> v_type_wait;
	t_slot_of<t_type> v_type_timer;
	t_slot_of<t_type> v_type_loop;

	static void f_main(t_extension* a_extension, const t_value& a_callable);

public:
	t_extension(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
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
		return const_cast<t_extension*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_scoped f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_extension* t_extension::f_extension<t_extension>() const
{
	return this;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_wait>()
{
	return v_type_wait;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_timer>()
{
	return v_type_timer;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_loop>()
{
	return v_type_loop;
}

}

#endif
