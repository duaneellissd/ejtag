
import unittest
import sys
import os

# "libcreator" is in ".." so add that to the path.
# But we do not do ".." because some IDES (Visual Stupid)
# often execute where the CWD is not this directory.
# So we use __file__ (the abs path of this file) to calculate accordingly
_f=os.path.abspath(__file__)
_dhere = os.path.dirname(_f)
_dparent = os.path.dirname(_dhere)
sys.path.append( _dparent )

import libcreator

class libcreator_unittest( unittest.TestCase ):
    # Assumption:  each module should have a selftest_NAMEHERE() function.
    # 
    # Here, we want to control at this level what order various modules
    # are tested, for example we want to test the low level items.
    # before we test the HIGH level items-
    #
    # to do that, we name the test functions by using a number 
    # sequence, but we need to leave gaps. So name your test cases 
    # with a 6 digit number like this: test_DDDDDD_foobar()
    #
    # Second, skip over numbers by using 100, 200, 300
    # this lets us insert something by using for example: 110, 120
    #
    # All low level standalone files that do not require another
    # module to work should be in the range 001000 
    #
    # Second level should use 002000 - and so on
    # After the second level, just do your best for numbering.
    #
    # The NUMBER lets us sort tests in an order.
    #
    # From the documentation:
    #   https://docs.python.org/3/library/unittest.html#organizing-test-code
    #   The "WidgitTestCase"
    # Quote:
    #   Note The order in which the various tests will be run is 
    #   determined by sorting the test method names with respect 
    #   to the built-in ordering for strings.
    
    def test_000000_frozen_class(self):
        libcreator.utils.frozenclass.selftest_frozenclass()
    def test_000005_file_location(self):
        libcreator.utils.filelocation.selftest_filelocation()

    # Reserve range 0011xx - for macro testing.
    def test_001100_macro_parser(self):
        libcreator.macros.parser.selftest_parser()

    def test_001200_macrovariables(self):
        libcreator.macros.variables.selftest_macrovariables()

    def test_001300_macroengine(self):
        libcreator.macros.engine.seftest_macroengine()

if __name__ == '__main__':
    unittest.main()
