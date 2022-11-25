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
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_library, t_global, f_global())
XEMMAI__LIBRARY__TYPE(t_library, wait)
XEMMAI__LIBRARY__TYPE(t_library, timer)
XEMMAI__LIBRARY__TYPE(t_library, loop)

}

#endif
