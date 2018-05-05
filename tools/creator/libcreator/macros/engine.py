from ..utils import FrozenClass
from ..utils import FileLocation
from .result import MacroResult
from .parser import MacroParser
from .variables import MacroVariables
import re

__all__ = ('MacroEngine')

class MacroEngine(FrozenClass):
    '''
    This is the macro engine, it handles resolving macros
    it can look up macros, and perform various transformations.
    
    Also see the types of macros described in MacroVariables()
    '''

    # Recursion stop, A->B->A
    RECURSION_STOP = 100

    def __init__(self):
        FrozenClass.__init__(self)
        self.variables = Variables(self)
        # enable support for macros that are function like.
        self.support_functions = True
        # enable support for macros with _uc and _lc suffixes.
        self.support_uclc = True
        self._parser = None
        self._full   = False
        self._result = Nonje
        self._location = None
        self._inputtext = None
        self._freeze()

    def _reset( self ):
        # Clear all local variables we don't need
        self._parser = None
        self._result = None
        self._full = False
        self._location = None
        self._inputtext = None

    def fully_resolve( self, text, location = None ):
        '''
        Fully resolve all macros even macros marked as do not expand.

        Macros marked as "donot_expand" *ARE* expanded.
        '''
        self._full = True
        self._resolve( text, location )
        r = self._result
        self._reset()
        return r

    def resolve( self, text, location = None ):
        '''
        Resolve all macros expanding all text.
        
        Macros marked as "donot_expand" *ARE*NOT* expanded.
        '''
        self._full = False
        self._resolve(text,location)
        r = self._result
        self._reset()
        return r


    def _resolve( self, text, location ):
        '''
        This is the underlying work horse that does the macro resolution.
        '''
        if location is not None:
            location = location.clone()
        self._location = location
        self._input = text
        self._result = MacroResult( self, self._input, self._location )

        # Handle the simple "plain text" case quickly
        if '$' not in self._input:
            self._parser.finish(self._result)
            self._result = ok
            return

        self._parser = MacroParser( self, self._text, self._location )
        for passnumber in range( 0, self.RECURSION_STOP ):
            if not self._resolve_one_macro():
                # we have success, set the result
                self._parser.finish(self._result)
                self._result.ok = True
                return r
        r.set_error("Does not resolve after %d steps" % self.RECURSION_STOP )
        parser.finish( result )
        return r

    def _replace( self, macrovalue, text ):

        both = self.as_both( )
        marking = self._parser.get_marking()
        
        mb4 = self._parser.marking_before()
        b4 = self._parser.text_before()
        maf = self._parser.marking_after()
        af = self._parser.text_after()
        
        newtext = b4 + text + af
        self._parser.set_string( newtext )
        self.remark( mb4, len(text), maf )
        
        

    def _resolve_one_macro( self ):
        '''
        Find and resolve the first macro we can find on the line
        '''

        # start again on the left hand side.
        self._parser.reset()
        
        # try to find an invocation
        self._parser.find_macro()

        # If not found, we are done.
        if self._parser.start_loc < 0:
            return False

        # get the macro text.
        macro_name = self._parser.text_macro()

        # And look it up
        # The most common case is a simple macro
        value = self.variables.get( macro_name, None )

        # If we found the simple macro
        if value is not None:
            # replace the value, we have made progress.
            self._replace( value, value.value )
            return True

        # is this a macro call?
        if self.support_functions:
            # then the macro name must look like foo()
            # there must be a ( somewhere, and a ) at the end
            if ('(' in macro_name) and (macro_name[:-1]==')'):
                # handle the function call.
                self._call_function( macro_name )
                return True
        
        # could this have a _uc or _lc suffix?
        # or the macro name is just too small, ie: ${a_uc} is valid.
        if (not self.support_uclc) or (len(macro_name) < 4):
            # Nope, we are done.
            self._result.error_notdefined(macro_name)
            return True

        # Assume not found
        txt = None
        # get the last 3 chars, ie: minus 3 or m3
        m3 = macro_name[-3:]

        # is it lower case?
        if m3 == '_lc':
            value = self.variables.get( macro_name[:-3], None )
            if value is not None:
                txt = str.lower(value.value)
        elif m3 == '_uc':
            # or upper case?
            value = self.variables.get( macro_name[:-3], None )
            if value is not None:
                txt = str.upper(value.value)

        if txt is None:
            self._result.error_notdefined(macro_name)
        else:
            self._replace( value, txt )
        # we have made progress
        return True
