from ..utils import FrozenClass
from ..utils import FileLocation
from ..utils import _mswindows, _linux, _mac, _posix
from ..utils import _python2, _python3
from .result import MacroResult
from .parser import MacroParser
from .variables import MacroVariables
import tokenize
import io
import re

if _python2:
    import StringIO
    import token
    ENCODING = token.N_TOKENS+1
    NUMBER = token.NUMBER
    NAME = token.NAME
    STRING = token.STRING
    ENDMARKER = token.ENDMARKER
    OP = token.OP

if _python3:
    NUMBER = tokenize.NUMBER
    ENCODING = tokenize.ENCODING
    NAME = tokenize.NAME
    STRING = tokenize.STRING
    ENDMARKER = tokenize.ENDMARKER
    OP = tokenize.OP

# The list below may seem bizarre, and not really required but 
# they are here to support macro that need or want to call
# a python function, for example: ${str.upper(${hello,world})}
# 
# When a function call is discovered, the name of the function
# is contained in a string, this string needs to be converted
# in to a Python Callable, to do that we basically do this:
#
#     x = eval("str.upper")
#
# Because 'str' is built in, there is no required import
# however other things for example: "os.getcwd()" requires
# the importation of the OS module
#
# Thus, here we import a huge number of things so that
# macros have as many functions as possible.

# note to other: Please keep this these sorted alphabetical
import atexit
import binascii
import binhex
import bz2
import copy
import datetime
import email
import errno
import fnmatch
import formatter
import glob
import json
import math
import os
import random
import smtplib
import stat
import string
import sys
import sys
import tarfile
import tempfile
import time
import time
import types
import zipfile

if _mswindows:
    import msvcrt
    import winsound
    import posixpath
else:
    import posix


__all__ = ('MacroEngine')

re_func_call = re.compile("^(?P<name>[-a-zA-Z0-9_.]+)[(]\s*(?P<params>.*)\s*[)]$")

def NoneToBlank( str_none ):
    if str_none == '' or str_none == 'None':
        return ''
    raise Exception("input is not None")

class mytoken:
    def __init__(self,*args):
        if _python3:
            t = args[0]
            self.type  = t.type
            self.value = t.string
            self.lhs   = t.start[1]
            self.rhs   = t.end[1]
        else:
            # type Text start(lineno,lhs) end(lineno,rhs) line
            self.type = args[0]
            self.value = args[1]
            self.lhs = args[2][1]
            self.rhs = args[3][1]
        if self.type != NUMBER:
            return
        if '.' in self.value:
            self.value = float(self.value)
        elif 'E+' in t.string:
            self.value = float(self.value)
        elif 'E-' in t.string:
            self.value = float(self.value)
        else:
            self.value = int(self.value)

class _tokenconverter():
    def __init__( self, engine, pstr, tlist ):
        self.engine = engine
        self.pstr = pstr
        self.tlist = tlist[:]
        self.param = list()

    def assert_ENCODING(self):
        t = self.tlist.pop(0)
        assert( t.type == ENCODING )
        
    def next_token(self):
        if len(self.tlist) == 0:
            # The end
            return None

        # Remove token.
        t1 = self.tlist.pop(0)
        if t1.type == ENDMARKER:
            return None

        # We should not have a comma here
        if t1.value == ',' and (t1.type==OP):
            self.engine._result.set_error("Illegal comma")
            return None

        # is this a number?
        if t1.value in ('+','-') and (t1.type == OP):
            # next must be a number
            if len(self.tlist) == 0:
                self.engine._result.set_error("missing number?")
                return None
            t2 = self.tlist.pop(0)
            if t1.value=='-':
                t1.value = -t2.value
            return t1.value
        if t1.type == NUMBER:
            return t1.value
        if t1.type == STRING:
            # remove the quote marks.
            return t1.value[1:-1]

        if t1.type != NAME:
            self.engine._result.set_error("invalid: %s" % t1.value)
            return None
        while len(self.tlist) > 1:
            # Peek at the next token
            t2 = self.tlist[0]
            # if we hit a comma, we are done with this param.
            if (t2.type == tokenize.OP) and (t2.value==','):
                break

            # Values like foo.bar is parsed as 3 tokens
            #   NAME (op=.) NAME
            # Convert these to a single NAME, ie: "NAME.NAME"
            if (t2.type == tokenize.OP) and (t2.value in ('.','-','+')):
                t2.type = tokenize.NAME
                # continue below
            if t2.type==tokenize.NAME:
                t1.value = self.pstr[t1.lhs:t2.rhs]
                self.tlist.pop(0)
                continue
            self.engine._result.set_error("Invalid: %s" % t1.value )
            return None
        return t1.value

    def consume_comma(self):
        if len(self.tlist) == 0:
            return False
        t1 = self.tlist[0]
        if t1.type == tokenize.ENDMARKER:
            return False
        if (t1.value == ',') and (t1.type==tokenize.OP):
            # remove the comma
            self.tlist.pop(0)
            return True
        self.engine._result.set_error("Syntax not a comma?")
        

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
        self.variables = MacroVariables(self)
        # enable support for macros that are function like.
        self.support_functions = True
        # enable support for macros with _uc and _lc suffixes.
        self.support_uclc = True
        self._parser = None
        self._full   = False
        self._result = None
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

    def _finish( self ):
        '''
        We are finished replacing text
        '''
        self._parser.open_loc= 0
        self._parser.close_loc = len(self._parser.data)
        self._parser.update_parts()
        s = self._parser.text_macro
        self._result.history.append("fini: %s" % s )
        self._result.result = s
        if self._result.error_msg is None:
            self._result.ok = True

    def _resolve( self, text, location ):
        '''
        This is the underlying work horse that does the macro resolution.
        '''
        if location is not None:
            location = location.clone()
        self._location = location
        self._inputtext = text
        self._result = MacroResult( self, self._inputtext, self._location )
        self._parser = MacroParser( self, self._inputtext, self._location )

        # Handle the simple "plain text" case quickly
        if '$' not in self._inputtext:
            self._finish()
            return

        for passnumber in range( 0, self.RECURSION_STOP ):
            # if something said error, we are done
            if self._result.error_msg:
                return
            if self._resolve_one_macro():
                # Success, we resolved something!
                continue
            else:
                # No more macros found
                self._finish()
                return
        self._result.set_error("Does not resolve after %d steps" % self.RECURSION_STOP )
        return


    def _resolve_one_macro( self ):
        '''
        Find and resolve the first macro we can find on the line
        '''

        # restart again on the left hand side.
        self._parser.restart()
        
        # try to find an invocation
        self._parser.find_macro()
        if self._parser.error_msg:
            # Syntax error
            self._result.set_error(self._parser.error_msg)
            return

        # If not found, we are done.
        if self._parser.close_loc < 0:
            return False

        # get the macro part that is the macro
        macro_name = self._parser.text_macro
        # we do not want the ${}
        macro_name = macro_name[2:-1]
        # And look it up
        # The most common case is a simple macro
        value = self.variables.get( macro_name, None )

        # If we found the simple macro
        if value is not None:
            # replace the value, we have made progress.
            value.used = True
            self._parser.replace( self._result, value.value )
            return True

        # is this a macro call?
        if self.support_functions:
            # then the macro name must look like foo()
            # there must be a ( somewhere, and a ) at the end
            if ('(' in macro_name) and (macro_name[-1:]==')'):
                # handle the function call.
                self._call_function( macro_name )
                return True

        # It could be a python constant, ie: "math.pi" is a constant
        ok = False
        try:
            const = eval(macro_name)
            ok = True
        except:
            ok = False

        if ok:
            self._parser.replace( self._result, str(const))
            # We replaced
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
            return False
        else:
            self._parser.replace( self._result, txt )
            return True

    def _call_function( self, macrotext ):
        m = re_func_call.match( macrotext )
        if m == None:
            self._result.set_error("bad call: %s" % macrotext)
            return
        name = m.group( "name")
        params = m.group("params").strip()
        callable = None
        try:
            callable = eval(name)
        except:
            callable = None
        if callable is None:
            self._result.set_error("macro: %s() does not exist" % name )
            return
        if len(params) == 0:
            plist = []
        else:
            plist = self._parse_params(params)
        if self._result.error_msg:
            # We are done... 
            return

        ok = False
        text = None
        try:
            text = callable( *plist )
            # This is not the greatest solution
            # but it solves the 80 to 90% case...
            # if this becomes a problem, try creating a string
            # then use the "eval()" function to eval the string.
            if text is None:
                text = ''
            text = str(text)
            ok = True
        except Exception as e:
            ok = False
            msg = "error: Exception: %s => %s" % (macrotext, str(e))
            self._result.set_error( msg )
        if ok:
            self._parser.replace( self._result, text )
            return
        else:
            # The exception handler (above) set the error message
            return

    def _parse_params( self, pstr ):
        # Convert param string into array of strings
        if _python3:
            plist = self._parse_params_py3( pstr )
        elif _python2:
            plist = self._parse_params_py2( pstr )
        else:
            assert("What python is this")
        p = _tokenconverter( self, pstr, plist )
        if _python3:
            p.assert_ENCODING()
        params = list()
        need_param = False
        while True:
            if self._result.error_msg is not None:
                break
            this_param = p.next_token()
            if this_param is None:
                if need_param:
                    self._result.set_error("syntax after comma")
                break
            params.append(this_param)
            need_param = p.consume_comma()
        return params

    def _parse_params_py3( self, pstr ):
        # Python 3 tokenize is very different then python2
        result = list()
        rl = io.BytesIO( pstr.encode('utf-8') ).readline
        tlist = list( tokenize.tokenize( rl ) )
        for t in tlist:
            result.append( mytoken(t) )
        return result

    def _parse_params_py2( self, pstr ):
        result = list()
        fh = StringIO.StringIO( pstr )
        rl = fh.readline
        def eater( *params ):
            result.append( mytoken( *params ) )
        tokenize.tokenize( rl, eater)
        return result

def test_nochange(e):
    # Test no change
    r = e.resolve("This is my text")
    print(r.history_as_str())
    assert( r.ok )
    assert( r.result == "This is my text" )
    r = e.fully_resolve("The Meaning Of Life")
    assert( r.ok )
    assert( r.result == "The Meaning Of Life" )
    print(r.history_as_str())

def test_simple_replacement(e):
    e.variables.define("DOG_NAME", "Dolly")
    e.variables.define("PET_NAME", "${DOG_NAME}")
    r = e.resolve("Dog's name: ${DOG_NAME}")
    assert( r.ok )
    assert( r.result == "Dog's name: Dolly")
    print(r.history_as_str())
    r = e.resolve("Pet name: ${PET_NAME}")
    assert(r.ok)
    assert( r.result == "Pet name: Dolly")
    print(r.history_as_str())

def test_undef(e):
    r = e.resolve("Dog's name: ${CAT_NAME}")
    assert( not r.ok )
    e.variables.define("PET_NAMEnope", "${DaG_NAME}")
    r = e.resolve("My ${PET_NAME_nope} d.n.e.")
    print(r.history_as_str())
    assert( not r.ok )

def test_deepnesting(e):
    for x in range(ord('A'), ord('Z')):
        e.variables.define( "VAR_%c" % (x+0), "${VAR_%c}" % (x+1) )
    e.variables.define("VAR_Z", "wow!")
    r = e.resolve("He said ${VAR_A}")
    assert(r.ok)
    assert(r.result == "He said wow!")
    # there is too much history here
    #print(r.history_as_str())

def test_syntax(e):
    r = e.resolve("${Duane-missing-brace")
    assert(not r.ok)
    print(r.history_as_str())

def test_endless(e):
    e.variables.define("A", "${B}")
    e.variables.define("B", "${A}")
    r = e.resolve("Value: ${A}")
    # this is just spew ...  don't print it
    #print(r.history_as_str())
    assert(not r.ok)

def test_uclc(e):
    # assumes "Dolly" exists.
    r = e.resolve( "Dog ${DOG_NAME_uc}")
    assert(r.ok)
    assert(r.result == "Dog DOLLY")
    r = e.resolve( "Dog ${DOG_NAME_lc}")
    assert(r.ok)
    assert(r.result == "Dog dolly")

def test_syntax_errors(e):
    r = e.resolve("Dog ${str.upper(${DOG_NAME},)}")
    assert(not r.ok)
    # invalid comma
    r = e.resolve("Dog ${str.upper(,${DOG_NAME})}")
    assert(not r.ok)
    # Missing quote
    r = e.resolve("Dog ${str.upper(\"Dolly)}")
    assert(not r.ok)

def test_func(e):
    r = e.resolve( "Dog ${str.upper(${DOG_NAME})}")
    assert(r.ok)
    assert(r.result == 'Dog DOLLY')
    r = e.resolve( "Dog ${str.lower(${DOG_NAME})}")
    assert(r.ok)
    assert(r.result == 'Dog dolly')
    r = e.resolve("${str.upper(dolly the dog)}")
    assert(r.ok)
    assert(r.result == 'DOLLY THE DOG')
    r = e.resolve( "i like ${math.cos(-${math.pi})} pie")
    assert(r.ok)
    assert(r.result == 'i like -1.0 pie')
    apath = os.path.abspath(__file__)
    parts = list(os.path.split(apath))
    if parts[0].endswith('\\'):
        parts[0] += '\\'
    for x in range(0,len(parts)):
        parts[x] = '"' + parts[x] + '"'
    s = ' , '.join( parts )
    r = e.resolve("${os.path.join(%s)}" % s )
    assert(r.ok)
    assert(r.result == apath )
    dname = 'tmp-macro-test'
    try:
        os.rmdir(dname)
    except:
        pass
    try:
        os.unlink(dname)
    except:
        pass

    # Try functions that return None.
    r = e.resolve("${os.mkdir(%s)}" % dname)
    assert(r.ok)
    assert(r.result == '' )
    r = e.resolve("${os.rmdir(%s)}" % dname)
    assert(r.ok)
    assert(r.result == '' )


def seftest_macroengine():
    print("test: %s" % __file__ )
    e = MacroEngine()
    test_nochange(e)
    test_syntax(e)
    test_endless(e)
    test_simple_replacement(e)
    test_undef(e)
    test_deepnesting(e)
    test_uclc(e)
    test_func(e)
    test_syntax_errors(e)
    print("Success: %s" % __file__ )
