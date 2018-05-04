
__all__ = ('FileLocation')

if __name__ == '__main__':
    from frozenclass import FrozenClass
else:
    from .frozenclass import FrozenClass

class FileLocation(FrozenClass):
    """This represents a location in a file"""
    def __init__(self, filename, linenumber):
        FrozenClass.__init__(self)
        self.filename = filename
        self.linennumber = linenumber
    def __str__(self):
        s = "%s:%s" % (self.filename, self.linennumber)
        return s
    def clone( self ):
        n = FileLocation( self.filename, linenumber )
        return n


def selftest_location():
    dut = FileLocation( "dog", 1234 )
    assert( str(dut) == "dog:1234" )
    print("Success")

if __name__ == '__main__':
    selftest_location()


