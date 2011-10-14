dispatcher = {}

def register(pattern):
    def inner(f):
        dispatcher[pattern] = f
        return f
    return inner

@register('/env')
def f(req):
    return 'Hello!'

@register('/bye')
def g(req):
    return 'Goodbye!'

@register('/wtf')
def wtf(req):
    return dispatcher

def run():
    while True:
        req = raw_input("State your business: ")
        if req in dispatcher:
            result = dispatcher[req](req)
            print '    This returned', result
        else:
            print '404 -- Resource not found'
        
if __name__ == '__main__':
    run()