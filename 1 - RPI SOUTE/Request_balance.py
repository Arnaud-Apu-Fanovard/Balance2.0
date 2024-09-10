import requests
import json

# req = requests.get("https://balance.e-kot.be/balance/balance")
# data = req.text
# print(data)

req1 = requests.get("https://balance.e-kot.be/balance/beers_on_balance")
# print(req1.status_code)
data1 = req1.text
print(data1)

# test = json.loads(data1)
# print(test['balance_1']['name'])


