
class DictionaryMixIn(object):
    '''Mixin class giving all dict methodds
    as long as you provide get/set/del-item
    len, iter, and init
    
    '''
    # --- code below does not access dictionary internals --- 
    
    def __contains__(self, key):
        try:
            value = self[key]
        except KeyError:
            return False
        return True
    
    def get(self, key, default):
        'looks up key, returns value or default if not found'
        try:
            return self[key]
        except KeyError:
            return default
    
    def pop(self, key):
        'looks up value, deletes value return value raise key error if not found'
        v = self[key]
        del self[key]
        return v
    
    def keys(self):
        'return list of keys'
        return [key for key in self]
    
    def values(self):
        'return list of values'
        return [self[key] for key in self]
    
    def items(self):
        'return list of (key, value) pairs'
        return [(key, self[key]) for key in self]
    
    def update(self, items):
        'loop over a list of key/value pairs, adding them to the dict'
        for key,value in items:
            self[key] = value
            
    def __repr__(self):
        result = ['%r : %r' % item for item in self.items()]
        return "{" + ", ".join(result) + "}"

    def _resize(self, n):
        save = self.items()
        self.n = n
        self.desks = [[] for i in xrange(self.n)]
        self.folders = [[] for i in xrange(self.n)]
        self.update(save)
    
    def clear(self):
        'Remove all entries from the dictionary (make empty)'
        for k in self.keys():
            del self[k]

class Dictionary(DictionaryMixIn):
    'Dictionary Look-a-like built from scratch'
    
    def __init__(self, **kwds):
        self.n = 8
        self.desks = [[] for i in xrange(self.n)]
        self.folders = [[] for i in xrange(self.n)]
        self.update(kwds.items())
    
    def __setitem__(self, key, value):
        if key in self:
            del self[key]
        else:
            if len(self) > self.n:
                self._resize(self.n * 4)
        i = abs(hash(key)) % self.n
        desk = self.desks[i]
        desk.append(key)
        folder = self.folders[i]
        folder.append(value)

    def __getitem__(self, key):
        i = abs(hash(key)) % self.n
        desk = self.desks[i]
        try:
            j = desk.index(key)
        except ValueError:
            raise KeyError(key)
        return self.folders[i][j]
    
    def __delitem__(self, key):
        i = abs(hash(key)) % self.n
        desk = self.desks[i]
        try:
            j = desk.index(key)
        except ValueError:
            raise KeyError(key)
        
        del self.folders[i][j]    
        del self.desks[i][j]   
    
    def __len__(self):
        return sum([len(desk) for desk in self.desks])
    
    def __iter__(self):
        for desk in self.desks:
            for key in desk:
                yield key
      
import os, pickle
          
class DirDictionary(DictionaryMixIn):
    'Persistent Dictionary Stored in a given directory'
    def __init__(self, dirname):
        self.dirname = dirname
        try:
            os.mkdir(dirname)
        except OSError:
            pass
    def __setitem__(self, key, value):
        fullname = os.path.join(self.dirname, key)
        with open(fullname, 'w') as f:
            f.write(pickle.dumps(value))
    
    def __getitem__(self, key):
        fullname = os.path.join(self.dirname, key)
        try:
            with open(fullname, 'r') as f:
                return pickle.loads(f.read())
        except IOError:
            raise KeyError(key)

    def __delitem__(self, key):
        fullname = os.path.join(self.dirname, key)
        try:
            os.unlink(fullname)
        except OSError:
            raise KeyError(key)
    
    def __iter__(self):
        files = os.listdir(self.dirname)
        return iter(files)

if __name__ == '__main__':
    d = DirDictionary('ciscodemo')
    d['raymond'] = [1, 2, 3]
    print d['raymond']
#    del d['raymond']
    print list(d)
    print d.update([('critter', 'yellow')])
    

