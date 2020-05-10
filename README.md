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
3. Create `api_key.key` file in de `config` dir.
4. Login to your SolarEgde monitoring system (https://monitoring.solaredge.com/).
5. Go to `admin` and select `Site entry`.
6. Accept SolarEdge API T&C terms.
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
