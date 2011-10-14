def cache(origfunc):
    def wrapper(x):
        if x in cache_dict:
            return cache_dict[x]
        result = origfunc(x)
        cache_dict[x] = result
        return result
    return wrapper

cache_dict = {}

@cache
def g(x):
    return 2 * x

g(10)
g(20)
print cache_dict
print g(10)