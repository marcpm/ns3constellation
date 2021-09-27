# ns3constellation



#### Setup 


```
./waf clean  

./waf configure --build-profile=debug --enable-examples --enable-tests 
```
.
#### DEBUGGG
```./waf --command-template="gdb %s" --run src/satellite-constellation/examples/basic-constellation-example
```

### Pasos
```
cd /home/tieg/plathon/ns3-constellations/ns3constellation
```

Entrar en virtualenv dedicado:
```
workon orbital 
```
Una vez dentro, podemos descargar TLE data a partir de:

```
python ns3constellation_main.py --predef-constellation iridium 

```




ns3 Example Scripts:


```(time  ./waf --run "basic-airborne-satellite-constellation-example --airPath=//home/tieg/plathon/ns3-constellations/ns3constellation/case-studies/testcase8airborne/air-EZE_FRA-tracklog.AIR --tlePath=/home/tieg/plathon/ns3-constellations/ns3constellation/case-studies/testcase9/STARLINK-21160_89993.TLE  --gsPath=/home/tieg/plathon/ns3-constellations/ns3constellation/case-studies/testcase8airborne/gs-cities-hypatia.GS --timespan=10000") &> airborne-constellation-starlink-hypatiaGS-EZEFRA-10000-log.txt```



Finalmente podemos hacer un analisis rápido del archivo FlowMonitor output de la simulacion de ns3.
```
python ns3postprocess.py  --flowmon-path "path-al-archivo.flowmon"  --results-file  "path-a-output-example-results.txt"
```