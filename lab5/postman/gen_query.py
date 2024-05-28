import datetime
import random


def genItem(id, time, state, i, n):
    itemStr = f"""
		{{
			"name": "New Request",
			"request": {{
				"method": "GET",
				"header": [],
				"url": {{
					"raw": "api:3000/",
					"host": [
						"api"
					],
					"port": "3000",
					"path": [
						""
					]
				}}
			}},
			"response": []
		}}
    """
    if i < n - 1:
        itemStr += ','
    return itemStr


def genQuery(n):
    items = ""
    for i in range(n):
        id = random.randint(1, n//2)
        time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        state = random.choice(['start','stop'])
        items += genItem(id, time, state, i, n)
    query = f"""
{{
	"info": {{
		"_postman_id": "a31330b9-40dd-4c3f-a185-509eece25221",
		"name": "Сережа",
		"schema": "https://schema.getpostman.com/json/collection/v2.1.0/collection.json",
		"_exporter_id": "34569649"
	}},
	"item": [
    {items}
	]
}}
"""
    return query


with open('queries.json', 'w') as f:
    query = genQuery(2000)
    f.write(query)