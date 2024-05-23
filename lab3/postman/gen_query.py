import datetime
import random


def genItem(id, time, state, i, n):
    host = "api"
    page = "send"
    itemStr = f"""
		{{
			"name": "1",
			"request": {{
				"method": "POST",
				"header": [],
				"body": {{
					"mode": "raw",
					"raw": "{{\\n    \\"userId\\": \\"{id}\\",\\n    \\"state\\": \\"{state}\\",\\n    \\"currentTime\\": \\"{time}\\"\\n}}",
					"options": {{
						"raw": {{
							"language": "json"
						}}
					}}
				}},
				"url": {{
					"raw": "{host}:3000/{page}",
					"host": [
						"{host}"
					],
					"port": "3000",
					"path": [
						"{page}"
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
    query = genQuery(7500)
    f.write(query)