from ..utils import FrozenClass
from .variables import MacroVariables

__all__=('MacroResult')

class MacroResult(FrozenClass):
    """This represents the result of a macro evaluation"""

    def __init__(self, engine, input, location ):
        FrozenClass.__init__(self)
        # what engine owns this.
        self.engine = engine
        # Where is this macro being invoked.
        self.location = location
        # What is the transformation history
        self.history = []
        # Was there an error?
        self.error_msg = ""
        # What is the result of the macro
        self.result = ""
        # Input text
        self.input = input
        # Used to track recursion errors
        self.depth = 0
        # In total, any problems?
        self.ok = False
        # Do not allow modification
        self._freeze()

    def error_notdefined( self, name ):
        '''
        Provide a consistent result when something is not defined.
        '''
        self.set_error("%s: is not defined" % name )

    def set_error(self, msg ):
        '''
        Set the error message, do not override an existing message.
        '''
        if len(self.error)==0:
            self.error = msg

    def history_as_str( self ):
        '''
        Return the transformation history as a giant string.
        '''
        return "\n".join( self.history ) + '\n'
