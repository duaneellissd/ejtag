import array
import sys
from ..utils import FrozenClass

DOLLAR = ord('$')
LBRACE = ord('{')
RBRACE = ord('}')
BACKSLASH = ord('\\')

class MacroParser(FrozenClass):
    '''
    This class parses a string looking for macro invocations.
    For example: ${foo} or, ${foo_${bar}}

    A macro can also be a function call, for example: ${os.getcwd()}
    
    When resolving text we can come across three types of macros.
    Type (A) normal, things that should and can be resolved.
    Type (B) the type we might not always want to resolve
    Type (C) externally defined macros that we cannot resolve.
    
    Thus when we come across types (B) and (C) we need to some how
    keep and/or ignore the unresolvable macro.

    Recall that our macros are simple ASCII text, nothing fancy.
    thus the text is always byte values from 0x00 to 0x7f

    To mark/ignore the text we convert the text into an array of uint16
    then to mark, we add 0x100 to each value, later we mask the values
    '''
    def __init__(self,engine,input=None,location=None):
        # The this seems faster
        FrozenClass.__init__(self)
        self.engine = engine
        self.data = array.array('h')
        # initialize variables
        self._fail()
        self.nested = False
        if location is not None:
            location = location.clone()
        self.location = location
        self.error_msg = None
        self.text_before = ''
        self.text_macro = ''
        self.text_after = ''
        self.mark_before = []
        self.mark_macro = []
        self.mark_after = []
        # Lock the class.
        self._freeze()
        if input is not None:
            self.set_string(input)

    def debug_print(self, writable = None):
        if writable is None:
            writable = sys.stdout
        
        parts = self._parts(0,len(self.data))
        s = parts[0] + parts[2] + parts[4]
        writable.write("value: %s\n" % s )
        tmp = self.rhs - self.lhs
        if tmp > 0:
            s = ' ' * self.lhs + '^' + ' ' * (tmp-1) + '^'
            writable.write(" locs: " + s + '\n' )
        else:
            writable.write("Not found\n")

    def debug_markout(self,writable=None):
        if writable is None:
            writable = sys.stdout
        parts = self._parts()
        s = parts[0] + parts[2] + parts[4]
        m = parts[1] + parts[3] + parts[5]
        m = ''.join( map( lambda x: '*' if x else ' ',m))
        writable.write("value: %s\n" % s )
        writable.write("marks: %s\n" % m )

    def set_string( self, s ):
        '''Initialize the string data'''
        self._fail()
        # convert the string into integers
        self.data = array.array('h', list(map( lambda x : ord(x), s )))
        self.restart()

    def restart( self ):
        '''Restart scanning at the start of the string'''
        self.nested = False
        self.set_range(0,len(self.data))
        self.error_msg = None

    def as_str2( self, lhs, rhs ):
        '''
        Set the macro as within [lhs:rhs]
        and the text of the macro invocation including ${}
        '''
        self.open_loc = lhs
        self.close_loc = rhs
        return self.as_str()

    def as_str( self, lhs = None, rhs = None ):
        '''
        Return the text as a string, all text between LHS and RHS.
        '''
        parts = self._parts(lhs,rhs)
        return parts[2]

    def replace( self, result, newtext ):
        s = self.text_before + self.text_macro + self.text_after
        result.history.append(" txt: %s" % s )
        s = ''
        s += ' ' * len(self.text_before)
        s += '*' * len(self.text_macro)
        s += ' ' * len(self.text_after)
        result.history.append("mark: %s" % s )
        result.history.append("from: %s" % self.text_macro)
        result.history.append("  to: %s" % newtext )
        s = self.text_before + newtext + self.text_after 
        result.history.append(" new: %s" % s )
        self.set_string(s)
        m = self.mark_before[:]
        m += [False] * len(newtext)
        m += self.mark_after
        for loc,value in enumerate(m):
            if value:
                self.data[loc] += 0x100
    def update_parts( self, lhs = None, rhs = None ):
        parts = self._parts(lhs,rhs)
        # save this
        self.text_before = parts[0]
        self.text_macro = parts[2]
        self.text_after = parts[4]
        self.mark_before = parts[1]
        self.mark_macro = parts[3]
        self.mark_after = parts[5]

    def _parts( self, lhs = None, rhs = None ):
        '''
        This returns a tuple of 6 elements (3 pairs of 2)

        Think of these as the results macro parsing.
        The macro is delimited by the slice: [lhs:rhs]
        By default, this is where the macro was found.
        
        The 3 pairs are the text before, text of, and text after the macro
        
        The even element is the text
        the odd element is an array of marking
        where the mark is t/f if a mark is present or not.
  
        Element[0] is the text before the LHS
        Element[1] is the mark before the LHS

        Element[2] is the text of the macro
        Element[3] is the mark of the macro
        This includes the ${ and } around the macro.

        Element[2] is the text after the macro
        Element[3] is the mark after the macro
        '''

        if lhs is None:
            lhs = self.open_loc
        if rhs is None:
            rhs = self.close_loc
        s = ''.join(map(lambda x: chr(x&0x0ff), self.data ))
        m = list(map(lambda x: (x >= 0x100), self.data ))

        return (s[0:lhs], m[0:lhs], 
                s[lhs:rhs], m[lhs:rhs], 
                s[rhs:], m[rhs:])

    def set_range( self, lhs, rhs ):
        '''Set/restrict the search range'''
        self._fail()
        # then override
        self.lhs = lhs
        self.rhs = rhs

    def mark_range( self, lhs, rhs ):
        '''
        This marks out a range so that is not recognizable as macro.
        '''
        for loc in range( lhs, rhs ):
            self.data[loc] += 0x100

    def find_macro( self ):
        '''This find the next macro in the data'''
        self.find_open()
        if self.open_loc < 0:
            return
        self.find_close()
        if self.close_loc < 0:
            return
        self.update_parts()

    def find_close( self ):
        '''
        We have already found the Opening, our goal is to find the
        closing brace for the macro, we might how ever find a nested
        macro, for example: ${foo_${bar}}
        
        At the start, lhs the opening dollar sign.
        '''
        # if we did not find an open, we should stop now
        self.close_loc = -1
        if self.open_loc < 0:
            return

        #We advance 2, to skip over DOLLAR LBRACE
        self.lhs += 2
        try:
            self._find_close()
        except:
            # Something went really wrong
            self.error_msg = "Syntax, col: %d" % self.open_loc
            self._fail()

    def _find_close(self):
        # wrapped function
        while self.lhs < self.rhs:
            c = self.data[self.lhs+0]
            if c == BACKSLASH:
                # need to skip the escaped char.
                self.lhs += 2
                continue
            if c == RBRACE:
                # great success!
                self.close_loc = self.lhs+1
                # go past the macro
                self.lhs += 2
                # so we can start at the next place
                return
            if c != DOLLAR:
                self.lhs += 1
                continue
            # We found a dollar, is this a nested macro?
            # for example ${foo_${bar}}, need to look ahead 1
            c = self.data[self.lhs+1]
            if c == LBRACE:
                # found, adjust the opening location
                self.nested = True
                self.open_loc = self.lhs
                self.lhs = self.lhs + 2
                continue
            else:
                # This is a syntax error, stray dollar sign
                self.error_msg = "Syntax col: %d, stray $" % self.lhs
                return
        # Fall off the end, we did not find it
        self.error_msg = "Syntax col: %d, no close" % self.open_loc
        self._fail()

    def find_open( self ):
        '''
        Search for an open macro, ie: "${" in the range lhs to rhs
        '''
        self.open_loc = -1
        self.close_loc = -1
        if DOLLAR not in self.data:
            self._fail()
            return
        try:
            self._find_open()
        except:
            self._fail()

    def _find_open(self):
        # wrapped find open
        while self.lhs < self.rhs:
            c = self.data[self.lhs+0]
            if c == BACKSLASH:
                # skip next item
                self.lhs += 2
                continue
            if c != DOLLAR:
                self.lhs += 1
                continue
            # get byte after the dollar
            c = self.data[self.lhs+1]
            if c != LBRACE:
                # keep looking.
                self.lhs += 1
                continue
            # found, remember the opening spot
            self.open_loc = self.lhs
            return 
        # not found or error
        self._fail()

    def _fail(self):
        self.lhs = len(self.data)
        self.rhs = len(self.data)
        self.open_loc = -1
        self.close_loc = -1

def subtest_slice_and_dice():
    #            0123456789012
    p = MacroParser(None)
    p.set_string("the room of wonder")
    # test 1, from left side
    s = p.as_str2(0,3)
    p.debug_print()
    assert( s == 'the' )
    # test 2 middle 
    s = p.as_str2(4,8)
    p.debug_print()
    assert( s == 'room')
    # test 3 end
    s = p.as_str(12,18)
    assert( s == 'wonder')
    # test 3 the entire string )
    s = p.as_str2(0,len(p.data))
    assert( s == 'the room of wonder')
    p.debug_print()
    p = None

def test_find( s, expect_o, expect_c ):
    p = MacroParser(None)
    print("search: %s, expect: %d,%d" % (s,expect_o,expect_c))
    p.set_string( s )
    p.find_macro()
    if (expect_o != p.open_loc) or (expect_c != p.close_loc):
        p.debug_print()
    assert( expect_o == p.open_loc )
    assert( expect_c == p.close_loc )
    # if we did not expect to find the macro
    if( expect_c < 0 ):
        # we are done
        return p
    b4 = s[0:expect_o]
    m  = s[expect_o:expect_c]
    af = s[expect_c:]
    assert( b4 == p.text_before)
    assert( m  == p.text_macro)
    assert( af == p.text_after)
        
    return p


def subtest_find_errors():
    # negative cases
    for s in ('', '$', '$nope', 'nope$', 'no$pe', r'not \${found} here' ):
        test_find( s, -1, -1 )
    # syntax errors
    for s in ('${', '${nope${pe))' ):
        print("search: %s, expect: 0, then -1" % s)
        p = MacroParser(None)
        p.set_string(s)
        # here we are testing that we FIND the open
        p.find_open()
        assert( p.open_loc == 0 )
        # and test we FAIL to find the close
        p.find_close()
        assert( p.open_loc == -1 )
        p = None
        
    p = MacroParser(None)
    p.set_string("this ${here")
    print("Search: %s, expect open at 5, then fail" % p.as_str())
    # here we are testing the open works then (below...)
    p.find_open()
    #p.debug_print()
    assert(p.open_loc == 5 )
    p.find_close()
    # and the close errors out
    assert(p.open_loc == -1)
    assert(p.close_loc == -1)
    p = None

def subtest_find_ok():
    # positive cases
    test_find( "${this}", 0, 7 )
    test_find( "${this} text after", 0, 7  )
    test_find( "foo ${this} bar", 4, 4+7)
    test_find( "text before ${this}", 12, 12+7)
    p = test_find( "${this} ${that}", 0, 7)
    
    # search from that point for the second macro
    print("Find second macro")
    p.find_macro()
    assert( p.open_loc == 8 )
    assert( p.close_loc == 15 )
    assert( p.text_before == '${this} ')
    assert( p.text_macro == '${that}')
    assert( p.text_after == '')

    # test nested macro
    # we already did nested error
    p = test_find( "${this ${that}}", 7, 14 )
    # mark the range
    p.set_string('${this ${that}}')
    p.mark_range( 7, 14 )
    p.debug_markout()
    p.find_macro()
    assert( p.open_loc == 0 )
    assert( p.close_loc == 15 )
    assert( p.text_before == '' )
    assert( p.text_after == '' )
    s = p.text_macro
    assert( s == '${this ${that}}')
    # test a nested escaped macro
    # This is NOT wrong, because the closing brace
    # after the last t in that is NOT escaped it does
    # serve as the closing brace for the opening brace
    # that is what we are testing here.
    p = MacroParser( None, "${this \\${that}}" )
    p.find_macro()
    assert( '' == p.text_before )
    s = p.text_macro
    assert( '${this \\${that}' == s)
    assert( '}' == p.text_after )
    # now test with the internal macro properly escaped
    p = MacroParser( None, "${this \\${that\\}}")
    p.find_macro()
    assert('' == p.text_before )
    assert('${this \\${that\\}}' == p.text_macro )
    assert('' == p.text_after )

def selftest_parser():
    # first test is that we can construct the class...
    print("test: %s" % __file__)
    p = MacroParser(None)
    p = None
    # now do other tests
    subtest_slice_and_dice()
    subtest_find_errors()
    subtest_find_ok()
    print("Success: %s" % __file__ )


