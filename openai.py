from openai import OpenAI
from dotenv import load_dotenv, find_dotenv
load_dotenv(find_dotenv())

API_KEY = "sk-proj-31BzJSJnMlyqyMfdDltyT3BlbkFJP5mx3w3w8emurareIekK"

client = OpenAI(api_key=API_KEY)

test_msg = ("The idea succulent must have the following Sensors thresholds: "
            "Humidity - Rh value 40-50 % Internal Temperature - 65-75 F External Temperature - 72.5-82.5 F "
            "Light - More than 15k lux Soil moisture Needs water if <400 Too much water if >700. "
            "Provide me with health tips for the succulent and how to take care of it.")

completion = client.chat.completions.create(model = "gpt-3.5-turbo",
messages = [{"role": "user", "content": test_msg}])

print(completion.choices[0].message.content)