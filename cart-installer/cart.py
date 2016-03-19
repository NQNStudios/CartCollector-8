import re
import requests
import os.path
from bs4 import BeautifulSoup
import util


class Cart(object):
    ''' Define all the attributes of a cart '''

    def __init__(self, tid):
        print('Parsing cart with tid: ' + str(tid))
        self._tid = tid

        # Open the cart page for parsing
        soup = self._open_cart_page()

        # Parse the title of the cart
        self._title = soup.title.contents[0]

        self._valid = True

        # Parse the number of the cart
        number_label_match = soup.find(string=re.compile('#[0-9]+#'))
        number_label = str(number_label_match)
        if number_label_match:
            self._number = int(number_label[2:-2])
        else:
            self._valid = False

    @property
    def tid(self):
        return self._tid

    @property
    def title(self):
        return self._title

    @property
    def number(self):
        return self._number

    @property
    def valid(self):
        return self._valid

    def _open_cart_page(self):
        ''' Return a BeautifulSoup object for the desired cart's page '''
        html = requests.get(util.BBS_BASE_URL + '?tid=' + str(self._tid)).text
        return BeautifulSoup(html, 'html.parser')

    def _cart_data(self):
        ''' Retrieve the byte data of this cartridge '''
        data = requests.get(util.BBS_BASE_URL + 'cposts/1/' +
                            str(self._number) + '.p8.png').content
        return data

    def _filename(self):
        ''' Convert the title of this cartridge into a cart filename '''
        name = self._title + '.p8.png'
        return util.cart_folder() + '/' + name

    def download(self):
        ''' Download this cartridge as a named .p8.png file '''
        file = open(self._filename(), 'wb')
        data = self._cart_data()
        file.write(data)
        file.close()

    def is_downloaded(self):
        return os.path.isfile(self._filename())
