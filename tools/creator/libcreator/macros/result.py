from ..utils import FrozenClass
from .variables import MacroVariables

__all__=('MacroResult')

class MacroResult(FrozenClass):
    """This represents the result of a macro evaluation"""

    def __init__(self, engine, input, location ):
        FrozenClass.__init__(self)
        # what engine owns this.
        self.engine = engine
        # What is the transformation history
        self.history = []
        # Where is this macro being invoked.
        self.location = location
        if self.location != None:
            self.history.append("%s: Evaluating" % location )
        self.history.append("start: %s" % input)
        # Was there an error?
        self.error_msg = None
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
        if self.error_msg is None:
            self.error_msg = msg

    def history_as_str( self ):
        '''
        Return the transformation history as a giant string.
        '''
        return "\n".join( self.history )
