import requests

req = requests.get("https://balance.e-kot.be/balance/balance")
data = req.text
print(data)

req1 = requests.get("https://balance.e-kot.be/balance/beers_on_balance")
data1 = req1.text
print(data1)