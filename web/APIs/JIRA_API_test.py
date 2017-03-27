#!/usr/bin/env python
import requests
import json
import base64

JIRA_USER = "<USERNAME>"
JIRA_PASS = "<PASSWORD>"
JIRA_URL = "<JIRA URL>"

auth = str(base64.b64encode("%s:%s" % (JIRA_USER, JIRA_PASS)))

#create issue

req_json = {
    "fields": {
       "project":
       { 
          "key": "<PROJECT KEY>"
       },
       "summary": "This is a test only!!!",
       "description": "Creating an issue using REST API!!!",
       "issuetype": {
          "name": "Bug"
       },
       "components": [{"name": "<COMPONENT NAME>"}]
   }
}

r = requests.post("%s%s" % (JIRA_URL, "rest/api/2/issue/"), json.dumps(req_json), headers={'Authorization': 'Basic %s' % auth, 'content-type': 'application/json'})
print r.text