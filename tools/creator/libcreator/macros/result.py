from ..utils import FrozenClass
from .variables import Variables


class Result(FrozenClass):
    """This represents the result of a macro evaluation"""

    def __init__(self, engine, input ):
        FrozenClass.__init__(self, engine, input, location)
        self.engine = engine
        self.location = location
        self.history = []
        self.error = ""
        self.result = ""
        self.input = input
        self.depth = 0

    def declare_error(self, msg ):
        if len(self.error)==0:
            self.error = msg

    def history_as_str( self ):
        return "\n".join( self.history ) + '\n'



            
    
