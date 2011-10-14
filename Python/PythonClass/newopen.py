# newopen.py                                                                    
#                                                                               
# Replacement for the built-in open()                                           

import gzip
import urllib

def newopen(filename,mode='r'):
    # Check for .gz file names                                                  
    if filename.endswith(".gz"):
        return gzip.open(filename,mode)

    # See if the first part of the filename looks like a URL.
    # If so, use urlopen to open it.  Note: the any() function
    # is used to test whether any member of a sequence is true.
    if any([filename.startswith(scheme) 
            for scheme in ['http:','https:','ftp:']]):
        return urllib.urlopen(filename)
    # Use the normal open
    else:
        return open(filename,mode)