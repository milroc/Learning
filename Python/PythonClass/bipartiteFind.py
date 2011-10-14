a = [(1, 'a'), (5, 'b'), (3, 'c'), (6, 'd')]
b = [('a', 1), ('b', 5), ('c', 1), ('d', 5)]

da = dict(a)
db = [(y,x) for x,y in b]

dc = set(a) & set(db)

print dc