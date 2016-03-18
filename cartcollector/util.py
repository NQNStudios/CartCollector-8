import os
from os.path import expanduser

BBS_BASE_URL = 'http://lexaloffle.com/bbs/'
INDEX_FILE = expanduser('~') + '/.collectorindex.json'


def parameter_string(cat, sub, page):
    ''' Construct the HTML string for the given parameters. page optional '''

    return_value = '?cat=' + str(cat) + '&sub=' + str(sub)

    if page:
        return_value += '&page=' + str(page)

    return return_value

def cart_folder():
    ''' Return the folder where carts are stored '''
    return os.environ['CART_COLLECTOR_DIR']
