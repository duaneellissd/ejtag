
# this exists as a single file to that anything 
# can directly import, and we do platform determination
# in a single common file across the entire package.
# thus if we do something wrong, we fix it here.

__all__ = ( '_platname', 
            '_cygwin', 
            '_msys', 
            '_mswindows', 
            '_linux', 
            '_mac', 
            '_posix',
            '_python2',
            '_python3')

# From the table:
#   https://docs.python.org/2/library/sys.html#sys.platform
import sys
_platname = sys.platform
_cygwin = (_platname=='cygwin')
_msys   = (_platname=='msys')
_mswindows = ((_platname=='win32') or _cygwin or _msys)
_linux = _platname.startswith('linux')
_mac   = (_platname == 'darwin')
_posix = (_mac | _linux)
_python2 = (sys.version_info[0] == 2)
_python3 = (sys.version_info[0] == 3)
