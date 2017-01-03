# AeroQual + raspberry

Loggin the data from AeroQual to raspbarry.

## Compatibilities
.+ S500.

## Compile and clean

```bash
$make
$make clean
```
## Run

```bash
$sudo ./bin/AeroQual [/dev/tty] [type]
```

## Order Kill and close the connection with Aeroqual
```bash
$sudo ps aux | grep Aero
#         PID
# root     18925  0.0  0.0 102196  4796 pts/11   S+   00:42   0:00 sudo ./bin/AeroQual /dev/ttyUSB0
$sudo kill -SIGTERM PID
```
## Author
