## Load required module
from jira.client import JIRA

## Step 1
def jira():
    JIRA_SERVER="https://your.jira.server.com"
    
    key_cert='/path/to/private/key.pem'
    with open(key_cert, 'r') as key_cert_file:
        key_cert_data = key_cert_file.read()
        
    oauth_dict={
        'access_token': '<ACCESS_TOKEN_HERE>',
        'access_token_secret': '<ACCESS_TOKEN_SECRET_HERE>',
        'consumer_key': 'OAuth key',
        'key_cert': key_cert_data
    }

    jira = JIRA(options={'server':JIRA_SERVER},oauth=oauth_dict)
    return jira

## Step 2
jira = jira()

## Step 3: Use the jira object
## Example: Search for issues by summary
fcid= "HHM3YBGX2"
issue = jira.search_issues('summary~' + fcid)
if len(issue) == 0:
    print("ERROR: no issue returned from JIRA with summary~" + fcid)
    return
        
issue = issue[0]

## Once you have an issue, you can add a comment like this  
comment = jira.add_comment(issue.key, "This is a comment")
