from ..utils import FrozenClass
from ..utils import FileLocation
from ..utils import FileLocation_builtin
from .value import MacroValue

class MacroVariables(FrozenClass):
    """
    This represents a collection (dictionary?) of macros 
    that are used by the macro engine.
    """

    def __init__(self, engine):
        FrozenClass.__init__(self)
        self.variables = dict()
        self._freeze()

    def define( self, name, value, location = None ):
        '''
        Define a variable with a value

        Location is a string or None, indicating where it was defined.
        It could be a file with a line number, ie: "foo.txt:1234"
        Or a string like "command-line", or "internal"
        '''
        var = MacroValue( name,value,location)
        self.variables[ name ] = var
        return var

    def external_def( self, name, location = None ):
        """
        Add or mark an existing variable as externally defined

        What this means is the variable can never be expanded
        and its value is known only by an out side thing.

        If this macro exists and a fully evaluated name
        is required then an undefined error has occurred.
        """

        var = self.variables.get( name, None )
        if var is None:
            var = self.define( name, None, location )
        var.external_def = True

    def donot_expand( self, name, value = None, location = None ):
        """
        Add or mark an existing variable as a do-not-expand value.
        
        Other times we might require the name be fully resolved.

        """
        var = self.variables.get( name, None )
        if var is None:
            var = self.define( name, None, location )
        var.donot_expand = True

    def exists( self, name ):
        """Return true if this macro name exists."""
        return name in self.variables

    def get( self, name, default_value = None ):
        """Lookup and return this variables value"""
        return self.variables.get( name, default_value )

def selftest_macrovariables():
    print("Test: %s" % __file__ )
    vars = MacroVariables( None )
    vars.define("Arthur", "Dent", FileLocation("HitchHiker",42))
    assert( vars.exists('Arthur' ) )
    # here we do not define a location
    vars.define("HeartOfGold", "Ship" )
    # this has a location
    vars.donot_expand("AnswerToLife","42", FileLocation_builtin() )
    # this does not have a location, Deep Thought is working on it
    vars.external_def("The Question" )

    assert( vars.exists( 'Arthur' ))
    assert( not vars.exists('Dent' ))
    v = vars.get('Arthur')
    assert( v.value == "Dent")
    assert( str(v.location) == 'HitchHiker:42')
    assert( v.name == 'Arthur' )
    assert( not v.external_def )
    assert( not v.donot_expand )

    # test the boolean flags for variables
    v = vars.get('HeartOfGold')
    assert( v.value == "Ship")
    assert(v.location == None)
    assert( not v.external_def )
    assert( not v.donot_expand )
    # change this
    vars.donot_expand("HeartOfGold")
    assert( v.donot_expand )
    assert( not v.external_def )
    vars.external_def( "HeartOfGold" )
    assert( v.external_def )

    # lookup the unknown
    s = "foo bar abc"
    v = vars.get('i-do-not-exist', s )
    assert( v == s )
    
    vars.donot_expand("NewVar1")
    v = vars.get("NewVar1")
    assert( v.value == None )
    assert( v.location == None )
    assert( v.donot_expand )
    assert( v.external_def == False )

    vars.external_def("NewVar2")
    v = vars.get("NewVar2")
    assert( v.value == None )
    assert( v.location == None )
    assert( v.donot_expand == False)
    assert( v.external_def == True )

    assert( len( vars.variables ) == 6 )
    # redefine a variable
    vars.define("Arthur", "See the 1981 movie")
    # no more or less variables.
    assert( len( vars.variables ) == 6 )
    # and the value was replaced..
    v = vars.get( "Arthur" )
    assert( v.value == "See the 1981 movie")

    print("Success: %s" % __file__ )
