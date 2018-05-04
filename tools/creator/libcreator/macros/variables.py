from ..utils import FrozenClass

class VariableValue( FrozenClass ):
    """This represents a variable"""
    def __init__(self,name,value, location):
        FrozenClass.__init__(self)
        self.name = name
        self.value = value
        self.used = False
        self.external_def = False
        self.donot_expand = False
        if location is not None:
            location = location.clone()
        self.location = location
        self._freeze()

class Variables(object):
    """Variables used in a macro engine"""

    def __init__(self, engine):
        FrozenClass.__init__(self)
        self.variables = dict()

    def define( self, name, value, location = "Unknown" ):
        """Define a variable with a value, defined at some location"""
        var = VariableValue( name,value,location)
        self.variables[ name ] = var
        return var

    def external_def( self, name, location = None ):
        """
        Mark the variable as externally defined

        What this means is the variable can never be expanded
        and is known out side of this macro system.

        If this macro exists and a fully evaluated name
        is required then an undefined error has occurred.
        """

        var = self.variables.get( name, None )
        if var is None:
            var = self.define( name, None, location )
        var.external_def = True

    def donot_expand( self, name, location = None ):
        """
        Some macros should not be expanded unless explicitly requested.
        
        An example might be a macro in a makefile, here in this tool
        we might know the value of the macro, but we don't want to
        resolve the macro at this time.

        Other times we might require the name be fully resolved.

        """
        var = self.variables.get( name, None )
        if var is None:
            var = self.define( name, None, location )
        var.donot_expand = True

    def exists( self, name ):
        """Return true if this macro name exists."""
        return name in self.variables

    def lookup( self, name ):
        """Lookup and return this variables value"""
        self.variables.get( name, default )
       
