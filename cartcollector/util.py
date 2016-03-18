BBS_BASE_URL = 'http://lexaloffle.com/bbs/'


def parameter_string(cat, sub, page):
    ''' Construct the HTML string for the given parameters. page optional '''

    return_value = '?cat=' + str(cat) + '&sub=' + str(sub)

    if page:
        return_value += '&page=' + str(page)

    return return_value
