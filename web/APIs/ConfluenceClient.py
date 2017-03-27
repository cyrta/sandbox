# -*- coding: utf-8 -*-
#!/usr/bin/env python
# Title              : ConfluenceClient.py
# Description        : Confluence xml-rpc client
# Author             : Jason Orona
# Date               : 07/08/14
# Version            : 1.0
# Usage              : Usage: ConfluenceClient.py -f file -n namespace -p pagename
# python_version     : 2.6.6
#================================================================================

import xmlrpclib
from optparse import OptionParser
import logging

logger = logging.getLogger(__name__)
FORMAT = '%(asctime)s %(name)s %(levelname)s %(message)s'
logging.basicConfig(format=FORMAT)

class Confluence(object):
    """
    A very basic Confluence API wrapper used for storing and retrieving
    wiki pages.
    """
    def __init__(self, username, password, rpc_url):
        self.username = username
        self.password = password
        self.rpc_url = rpc_url
        self.server = xmlrpclib.ServerProxy(self.rpc_url)
        self.token = self.server.confluence2.login(self.username, self.password)

    def getPage(self, namespace, title):
        return self.server.confluence2.getPage(self.token, namespace, title)

    def storePage(self, page):
        return self.server.confluence2.storePage(self.token, page)  # Do I need the namespace


def execute_main():
    DEFAULT_OPTIONS = {
        'server': "https://wiki.mynmg.com/rpc/xmlrpc",
        'username': "",
        'password': ''
        }

    server = DEFAULT_OPTIONS['server']
    username = DEFAULT_OPTIONS['username']
    password = DEFAULT_OPTIONS['password']

    parser = OptionParser("Usage: ConfluenceClient -f file -n namespace -p pagename")
    parser.add_option('-f', '--file', dest='file', help='Filename')
    parser.add_option('-n', '--namespace', dest='namespace', help='Confluence namespace')
    parser.add_option('-p', '--pagename', dest='pagename', help="Confluence page name or id")
    parser.add_option('-s', '--search', dest='search', action='store_true', help="Enables search capability")
    #parser.add_option('-r', '--rollback', dest='rollback', help='Rollback to previous changes')
    parser.add_option('-v', '--verbose', dest='verbose', action='store_true', help='Verbose mode')
    (options, args) = parser.parse_args()

    if options.verbose:
        logger.setLevel(level=logging.DEBUG)
    else:
        logger.setLevel(level=logging.INFO)

    if options.namespace is None:
        parser.error('Namespace must be provided. See help.')
        exit()
    elif options.namespace:
        namespace = options.namespace

    #assert isinstance(options.pagename, object)
    if options.pagename is None:
        parser.error('Pagename must be provided. See help')
        exit()
    elif options.pagename:
        pagename = options.pagename

    if options.search and options.file:
        parser.error('Cannot use --search and --file parameters simultaneously. See help')
        exit()

    if options.search :
        try:
            logger.debug("Connecting to %s, username=%s" % (server, username))
            c = Confluence(username, password, server)
            page = c.getPage(namespace, pagename)
            logger.debug("Now retrieving page data, namespace=%s, pagename=%s" % (namespace, pagename))
            print page
            return
        except:
            logger.error("Page not found!")
            return

    if options.file is None:
        parser.error('Filename must be provided. See help.')
        exit()
    elif options.file:
        file = options.file
        try:
            logger.debug("Filename=%s" % (file))
            logger.debug("Opening file %s" % (file))
            content = open(file, "r").read()
        except IOError as e:
            logger.error(e)
            exit()

        try:
            logger.debug("Connecting to %s, username=%s" % (server, username))
            c = Confluence(username, password, server)
            logger.debug(c)
            logger.debug("Now retrieving page data, namespace=%s, pagename=%s" % (namespace, pagename))
            data = c.getPage(namespace, pagename)
            logger.debug("Page Contents... %s" % (data))
            logger.debug("Swapping %s page content with %s content" % (pagename, file))
            data['content'] = content
        except:
            logger.error("Page not found!")
            exit()

        try:
            logger.info("Attempting to store content from %s into space '\%s\' page \'%s\'"  % (file, namespace, pagename))
            c.storePage(data)
        except:
            logger.error("An error occured while updating wiki! Wiki did not update.")
            return
        logger.info("Success!")
        return

if __name__ == '__main__':
    execute_main()