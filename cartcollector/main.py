import json
import os.path
import util
from cartindexer import CartIndexer
from cart import Cart


if __name__ == "__main__":
    # Create an index if one doesn't exist yet
    if not os.path.isfile(util.INDEX_FILE):
        index = {
            'installed-carts': []
        }
        with open(util.INDEX_FILE, 'w') as index_file:
            json.dump(index, index_file)


    # Parse the tid of every cart that is already installed
    installed_tids = []
    index = {}

    with open(util.INDEX_FILE, 'r') as index_file:
        index = json.load(index_file)
        installed_tids = index['installed-carts']

    # Index all PICO-8 carts
    indexer = CartIndexer()
    tids = indexer.all_cart_tids()

    # Process each cart that is not yet installed
    for tid in tids:
        if tid in installed_tids:
            continue

        # Parse the cart
        cart = Cart(tid)

        print('Processing cart: ' + cart.title)

        # If the cart is not yet downloaded
        if not cart.is_downloaded() and cart.valid:
            print('Downloading cart: ' + cart.title)
            # Download the cart
            cart.download()

        # Remember the cart in the index
        installed_tids.append(tid)

        index['installed-carts'] = installed_tids
        with open(util.INDEX_FILE, 'w') as index_file:
            json.dump(index, index_file)
