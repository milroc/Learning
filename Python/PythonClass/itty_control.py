from itty import get, run_itty

@get('/')
def index(request):
    return 'Hello World'

run_itty()
