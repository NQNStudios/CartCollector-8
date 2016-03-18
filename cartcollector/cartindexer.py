import re
import requests
from bs4 import BeautifulSoup
import util


class CartIndexer(object):
    ''' Creates an index of carts from the Lexaloffle website '''

    def __init__(self, cat=7, sub=2):
        ''' Create a CartIndexer for submissions matching the given parameters.
        Defaults:
            cat = 7 (for PICO-8 submissions)
            sub = 2 (for finished carts)
        '''

        self._cat = cat
        self._sub = sub

    def _open_page(self, page):
        ''' Return a BeautifulSoup object for the desired page of submissions '''
        html = requests.get(
            util.BBS_BASE_URL + util.parameter_string(
                self._cat, self._sub, page)).text
        soup = BeautifulSoup(html, 'html.parser')
        return soup

    def _num_pages(self):
        ''' Find how many pages there are of matching submissions '''
        # Do this by parsing the second page of submissions on which the
        # total number of pages is displayed in format 'Page 2 of [max]'
        soup = self._open_page(2)
        page_string = str(soup.find(string=re.compile('Page 2 of [0-9]+')))
        num_pages = int(page_string.split(' ')[3])
        return num_pages

    def _extract_tids(self, page):
        ''' Return a list of tid values for carts listed on the given page '''
        soup = self._open_page(page)
        links = []
        for link in soup.find_all('a'):
            links.append(link.get('href'))

        tids = [
            re.search(re.compile('[0-9]+'), link).group(0)
            for link in links
            if re.search(re.compile('^\?tid=[0-9]+$'), link)
        ]

        return tids

    def all_cart_tids(self):
        ''' Return a list of every tid for a matching cartridge '''
        tids = []
        for page in range(1, self._num_pages()+1):
            tids.extend(self._extract_tids(page))
        return tids
