
from ..utils import FrozenClass

class MacroValue( FrozenClass ):
    """
    This represents a single macro variable and its associated value

    Variables come in 5 types: 

    1) Normal macros, this is the default
       If the macro name is not know it is an error (ie: Undefined)
    
    2) Externally defined - meaning this macro engine does not
       know the value of the macro, and it should effectively
       be ignored unless a fully resolved text is requested.

    3) Do not expand - BUT this code knows the value of the
       macro, and for other purposes (when we generate output)
       the macro needs to remain as a macro.

       There are two types of macro resolution, normally expand
       and full expand. In the "normal" case this macro is not
       expanded, it remains as a macro. In the full case this
       macro is fully expanded.

        An example might be a macro used in a makefile.
        For example: "${${PROJECT_NAME}_C_SOURCES}" which might
        first resolve as ${foo_C_SOURCES}

        There are times it might be helpful to fully resolve this macro
        producing the list of source files, but at other times when
        generating a Makefile it might be helpful to NOT expand
        the ${foo_C_SOURCES}

    4) Function like macros - these are NOT handled here.
       These are discovered and handled by the macro engine.
       For example you can invoke Python functions within a macro
       by doing things like this:

        UPPER_foo = ${str.upper(${foo})}

    5) Macros with the suffix _lc, or _uc example: ${foo_lc}
       Again, these are handled by the macro engine.

       Basically ${foo} and ${foo_uc} or ${foo_lc} are the same.
       The _uc variant is the upper case version of the value.
       The _lc variant is the lower case version of the value
       
    """
    def __init__(self,name,value, location = None):
        FrozenClass.__init__(self)
        self.name = name
        self.value = value
        self.used = False
        # is this item externally defined?
        self.external_def = False
        self.donot_expand = False
        if location is not None:
            location = location.clone()

        self.location = location
        self._freeze()

# SELFTEST:
#   There's not much to test here...
#   It is effectively tested by the MacroValues package.

