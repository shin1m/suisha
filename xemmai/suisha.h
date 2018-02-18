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
	template<typename T, typename T_super> friend class xemmai::t_define;

private:
	t_slot v_type_wait;
	t_slot v_type_timer;
	t_slot v_type_loop;

	static void f_main(t_extension* a_extension, const t_value& a_callable);

	template<typename T>
	void f_type__(t_scoped&& a_type);

public:
	t_extension(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
	{
		return f_global();
	}
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_scoped f_as(T a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), std::forward<T>(a_value));
	}
};

template<>
inline void t_extension::f_type__<t_wait>(t_scoped&& a_type)
{
	v_type_wait = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_timer>(t_scoped&& a_type)
{
	v_type_timer = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_loop>(t_scoped&& a_type)
{
	v_type_loop = std::move(a_type);
}

template<>
inline const t_extension* t_extension::f_extension<t_extension>() const
{
	return this;
}

template<>
inline t_object* t_extension::f_type<t_wait>() const
{
	return v_type_wait;
}

template<>
inline t_object* t_extension::f_type<t_timer>() const
{
	return v_type_timer;
}

template<>
inline t_object* t_extension::f_type<t_loop>() const
{
	return v_type_loop;
}

}

#endif
