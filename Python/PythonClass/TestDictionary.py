import unittest
from Dictionary import Dictionary

class TestSequentialFunctions(unittest.TestCase):
    def test_basic(self):
        d = Dictionary()
        d['raymond'] = 'red'
        self.assertEqual(d['raymond'], 'red')
        d['rachel'] = 'blue'
        self.assertEqual(d['rachel'], 'blue')
        d['critter'] = 'yellow'
        self.assertEqual(d['critter'], 'yellow')
        with self.assertRaises(KeyError):
            d['daughter']
        
        d['rachel'] = 'purple'
        self.assertEqual(d['rachel'], 'purple')
        self.assertEqual(len(d), 3)
        self.assertEqual(set(d), {'raymond', 'rachel', 'critter'})
    
    def test_contains(self):
        d = Dictionary()
        d['raymond'] = 'red'
        d['rachel'] = 'blue'
        d['critter'] = 'yellow'
        self.assertTrue('rachel' in d)
        self.assertFalse('john' in d)
    
    def test_get(self):
        d = Dictionary()
        d['raymond'] = 'red'
        self.assertEqual(d['raymond'], 'red')
        d['rachel'] = 'blue'
        self.assertEqual(d['rachel'], 'blue')
        d['critter'] = 'yellow'
        self.assertEqual(d.get('raymond', 'not found'), 'red')
        self.assertEqual(d.get('john', 'not found'), 'not found')
    
    def test_pop(self):
        d = Dictionary()
        d['raymond'] = 'red'
        d['rachel'] = 'blue'
        self.assertEqual(d.pop('rachel'), 'blue')
        self.assertEqual(d['raymond'], 'red')
        self.assertEqual(len(d), 1)
        with self.assertRaises(KeyError):
            d.pop('john')
    
    def test_keys_values_items(self):
        d = Dictionary()
        d['raymond'] = 'red'
        d['rachel'] = 'blue'
        keys = d.keys()
        self.assertTrue(isinstance(keys,list))
        self.assertEqual(set(keys), {'raymond', 'rachel'})
        values = d.values()
        self.assertTrue(isinstance(values,list))
        self.assertEqual(set(values), {'red', 'blue'})
        
        items = d.items()
        self.assertTrue(isinstance(values,list))
        self.assertEqual(set(items), {('raymond','red'), 
                                     ('rachel','blue')})
        
    def test_update(self):
        d = Dictionary()
        d.update([('raymond', 'red'), ('rachel', 'blue')])
        self.assertEqual(len(d), 2)
        self.assertEqual(d['raymond'], 'red')
        self.assertEqual(d['rachel'], 'blue')
        
    def test_constructor(self):
        d = Dictionary(raymond='red', rachel='blue')
        self.assertEqual(len(d), 2)
        self.assertEqual(d['raymond'], 'red')
        self.assertEqual(d['rachel'], 'blue')
    
#    def test_repr(self):
#        d = Dictionary(raymond='red', rachel='blue')
#        print d
#        self.assertEqual(d.__repr__(), '{\'raymond\': \'red\', \'rachel\', \'blue\'}') 
#        # THIS TEST WILL FAIL BECAUSE SAVING THIS IS ODD/HARD TO KNOW ORDER

    

if __name__ == '__main__':
    try:
        unittest.main()
    except SystemExit:
        pass