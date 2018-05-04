# Based upon:
# https://stackoverflow.com/questions/3603502/prevent-creating-new-attributes-outside-init

__all__= ('FrozenError', 'FrozenClass')

class FrozenError(Exception):
    pass

class FrozenClass(object):
    """A frozen class prevents attribute mis-spelling"""
    __isfrozen = False
    def __init__(self):
        object.__init__(self)

    def __setattr__( self, key, value ):
        if self.__isfrozen and not hasattr( self, key ):
            raise FrozenError("%r is frozen cannot add: %s" % (self,key))
        object.__setattr__(self,key,value)

    def _freeze( self ):
        object.__setattr__(self,'__isfrozen', True );

    def _thaw( self ):
        object.__setattr__(self,'__isfrozen', False );

def frozen_selftest():
    tst = FrozenClass()
    tst.valid_spelling = 123
    tst._freeze()
    try:
        tst.invalid_spelling = "this should fail"
        raise Exception("This should not have worked")
    except Exception as x:
        pass
    tst._thaw()
    tst.other_valid = "Dog"
    print("Success")

if __name__ == '__main__':
    frozen_selftest()

        

