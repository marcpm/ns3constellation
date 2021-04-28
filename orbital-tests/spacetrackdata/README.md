# Spacetrack tests:
## Requirements

- Crear cuenta en [spacetrack.org](https://www.space-track.org/).
- Editar archivo spacetrackdata/stconfig_example.py
	- Poner email, y pass recien creados.
	- Cambiar nombre archivo a stconfig.py

## Instalacion (requiere Python3.7+) (probado en 3.9)

- crear virtual env (opcional)

'''pip install spacetrack sgp4 '''

( o pip3 install spacetrack sgp4) 
dependiendo si el link a python3 está en python o python3. 
(comprobable con """which python""" ) o '''which python3'''



#### Ejemplos
ejemplo en "spacetrack_test.py". editar allí eligiendo constelaciones predefinidas (starlink, oneweb, iridium). O añadiendo manualmente NORAD_IDs de cualquier satélite.
Establecer t_inicial,  t_final,  y timestep.

El Ejemplo  escribe por defecto archivos en formato json, para cada satélite. 
Escribe TLEs en un archivo y  archivos de Posición [m] Velocidad [m/s] en un frame inercial (TEME).'

Ver Ejemplo json para Iridium: "iridium_TLES_PV_ejemplo.json".

Archivo "getPV_data_from_json.m" contiene una rutina rápida para parsear el json de posiciones / velocidades. en Pxyz, Vxyz ambos con dimensiones (num_satellites x num_timesteps x 3).







 