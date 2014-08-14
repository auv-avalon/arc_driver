#include <boost/test/unit_test.hpp>
#include <arc_driver/Dummy.hpp>

using namespace arc_driver;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    arc_driver::DummyClass dummy;
    dummy.welcome();
}
