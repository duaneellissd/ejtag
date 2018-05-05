# Based upon:
# https://stackoverflow.com/questions/3603502/prevent-creating-new-attributes-outside-init

__all__= ('FrozenError', 'FrozenClass')

class FrozenException(Exception):
    pass

class FrozenClass(object):
    """
    This class exists to prevent random mis-spelled attributes
    Some of us are marvelous spellers and always got perfect marks in school.
    
    I am the opposite person, I consistently mis-spell things
    trivial example:  foo.bar verses foo.barr  (an extra R)

    OR I commonly forget how I spelled something, resulting in strangeness
    Such as foo.data_package verses the plural: foo.data_packages

    Another language example is British verses USA spelling.
    names like:  foo.colour, when I wanted foo.color

    This class prevents those types of simple nonsense mistakes.
    """

    def __init__(self):
        object.__init__(self)
        object.__setattr__( self, '__isfrozen', False )

    def __setattr__( self, key, value ):
        # we override __setattr__ and test...
        if hasattr( self, key ):
            # success
            object.__setattr__( self, key, value )
            return
        # attribute does not exist fix it
        v = getattr( self, '__isfrozen', None )
        if v is None:
            # Developer must fix this!
            # the class should have called FrozenClass.__init__()
            raise Exception("%r not initialized correctly" % self )
        if v:
            raise FrozenException("%r is frozen cannot add: %s" % (self,key))
        # use the base class to set
        object.__setattr__(self,key,value)

    def _freeze( self ):
        object.__setattr__(self,'__isfrozen', True );

    def _thaw( self ):
        object.__setattr__(self,'__isfrozen', False );

def selftest_frozenclass():
    print("Test: %s" % __file__)
    class Foo(FrozenClass):
        def __init__(self):
            FrozenClass.__init__(self)
            self.xyzzy = 42
            self._freeze()

    tst = Foo()
    # this should not fail
    tst.xyzzy = 123
    fail = False
    try:
        # use an invalid name
        tst.xYzzy = "this should fail"
        fail = True
    except Exception as x:
        pass
    assert( not fail )
    tst._thaw()
    # Allow an addition
    tst.dolly = "Dog"
    tst._freeze()
    # and modification
    tst.xyzzy *= 42
    try:
        # but not addition
        tst.arthur = "Dent"
        fail = True
    except Exception as x:        
        pass
    assert( not fail )
    print("Success: %s" % __file__ )

if __name__ == '__main__':
    selftest_frozenclass()

        

