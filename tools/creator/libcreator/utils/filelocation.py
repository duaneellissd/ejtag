
__all__ = ('FileLocation')

if __name__ == '__main__':
    from frozenclass import FrozenClass
else:
    from .frozenclass import FrozenClass

class FileLocation(FrozenClass):
    '''
    This represents a location that something was found
    
    An example use case is a macro which could be defined
    in some file, or perhaps on the command line

    This would result in a location string like:

    Example 1: foo.txt:123
    Example 2: command-line
    '''
    def __init__(self, filename, linenumber = None):
        FrozenClass.__init__(self)
        self.filename = filename
        if linenumber is None:
            linenumber = 0
        self.linenumber = linenumber
        self._str = "%s:%s" % (self.filename,self.linenumber)
        self._freeze()
    def __str__(self):
        return self._str
    def clone( self ):
        return FileLocation( self.filename, self.linenumber )

class FileLocation_cmdline( FileLocation ):
    '''
    Provide a consistently named location for things
    that came from the application command line.
    '''
    def __init__(self):
        FileLocation.__init__(self,'cmd-line', 0 )

class FileLocation_builtin( FileLocation ):
    '''
    Provide a consistently named location for things
    that came from the application internals.
    '''
    def __init__(self):
        FileLocation.__init__(self,'built-in', 0 )

def selftest_filelocation():
    print("Test: %s" % __file__ )
    dut = FileLocation( "dog", 1234 )
    assert( str(dut) == "dog:1234" )
    print("Success: %s" % __file__ )

if __name__ == '__main__':
    selftest_location()


