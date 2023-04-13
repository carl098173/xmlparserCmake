import requests
import os
# importing the zipfile module
from zipfile import ZipFile

url = 'https://dl.dod.cyber.mil/wp-content/uploads/stigs/zip/U_CCI_List.zip'
r = requests.get(url, allow_redirects=True)
print(os.getcwd())
open('U_CCI_List.zip', 'wb').write(r.content)

# loading the temp.zip and creating a zip object
with ZipFile("U_CCI_List.zip", 'r') as zObject:
    zObject.extract("U_CCI_List.xml", path=os.getcwd())
zObject.close()

os.remove("U_CCI_List.zip")
