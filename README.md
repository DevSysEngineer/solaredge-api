# SolarEdge API
SolarEdge API is a framework written in PHP that allow you to connect with the SolarEdge backend/API.

## Demo
### Details
1. Clone this project to your environment
```
git clone https://github.com/KvanSteijn/solaredge-api.git
```
2. Run composer
```
composer install
```
3. Create `api_key.key` file in the `config` dir.
4. Login to your SolarEgde monitoring system (https://monitoring.solaredge.com/).
5. Go to `admin` and select `Site entry`.
6. Accept `SolarEdge API T&C terms`.
7. Create API key and paste this API key in the `api_key.key` file.
8. Navigate to example dir in you shell: `cd example`.
9. Run details.php: `php details.php`.

```
Site: xxxxxxxxxx
Data period from: 2019-08-25 until 2020-05-10
----------------------------------------------
Energy year:
2019: 0.36MWh
2020: 1.04MWh
----------------------------------------------
Energy this month (179.73KWh):
Week 18: 65.07KWh
Week 19: 114.66KWh
Week 20: 0.00KWh
Week 21: 0.00KWh
Week 22: 0.00KWh
----------------------------------------------
Yesterday energy (2020-05-09): 19.01KWh
```

### Daily report with Telegram
Send automatic a message to your telegram account.

1. Compleet first the `Details` steps.
2. Create `telegram.json` file in the `config` dir.
3. Create a bot with the `telegram bot father`.
3. Paste follow code below with you `telegram` bot information.
```
{
    "apiKey": "YOUR_API_KEY",
    "botName": "YOUR_BOT_NAME",
    "chatIds": ["YOUR_CHAT_ID"]
}
```
8. Navigate to example dir in you shell: `cd example`.
9. Run telegram_send_message.php.php: `php telegram_send_message.php.php`.
10. Check if you received any messages.
11. Create a cronjob: `0 8 * * * /usr/bin/php {LOCATION}/example/telegram_send_message.php >/dev/null 2>&1`
13. You will receive everyday at 8:00AM a message with daily report.
