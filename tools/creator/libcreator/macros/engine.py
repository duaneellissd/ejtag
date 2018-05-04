from ..utils import FrozenClass
from ..utils import Location
from .result import Result
from .variables import Variables

import array

__all__ = ('Engine')
DOLLAR = ord('$')
LBRACE = ord('{')
RBRACE = ord('}')

class funkystring( array.array ):
    # https://stackoverflow.com/questions/4868291/how-to-subclass-array-array-and-have-its-derived-constructor-take-no-parameters
    def __new__(cls,typecode=None):
        return array.array.__new__(cls,'h')
    def as_str( self ):
        return ''.join(map(lambda x: chr(x&0x0ff), a ))
    def find( self, value ):
        try:
            return self.index(value)
        except:
            return -1

def _to_numbers( s ):
    # Convert a string to an array of integers
    # Example: "dog" becomes [100,111,103]
    return fastarray('h',map(ord,s))

def _to_string( a ):
    # map the array of integers back to strings.
    # Note, some locations may be +0x100
    # This +0x100 must be removed
    return 

class Engine(FrozenClass):
    """This is the macro engine"""

    # Recursion stop, A->B->A
    DEPTH_MAX = 50
    # Recursion stop A->B->A
    LOOP_MAX = 50

    def __init__(self):
        self.variables = Variables(self)

    def resolve( self, text, location = None ):
        """
        Resolve this text, expanding all normal macros
        """

        # Handle the simple case
        result = Result(engine, text, location)
        if '${' not in text:
            result.result = text
            return

        # convert the text to            
        nums = _numify( text )

        nums = self._resolve( self, result, nums )
        if len(result.error) == 0:
            result.result = _tostr(nums)
        else:
            result.result = result.error
        return result

    def _resolve( self, result, nums):
        # this exists to monitor Recursion Depth.

        # short circuit - are we done?
        dollar_loc = nums.find( DOLLAR )
        if dollar_loc < 0:
            return nums
        if (dollar_loc+4) >= len(nums):
            result.history.append( "here: " + _tostr(nums))
            result.history.append( "      " + " "*loc + "^here" )
            result.declare_error("Syntax error")
            return nums
        result.depth += 1
        if result.depth > self.DEPTH_MAX:
            result.declare_error("recursion > %d" % self.DEPTH_MAX )
            return text
        nums = self.__resolve( self, result, nums )
        result.depth -= 1
        return nums

    def __resolve( self, result, nums ):
        # handle situation of "text ${this} then ${that}"
        lhs = 0
        rhs = len(nums)
        while lhs < rhs:
            
            dloc = nums.find(DOLLAR)
            if dloc < 0:
                break
            lb   = nums.find(LBRACE,dloc)

        dloc = nums.find(DOLLAR)
                





