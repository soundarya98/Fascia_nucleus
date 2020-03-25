# ADS1299 Dataplotter 

The files in this repo should allow you install a working demo for projects that need to visualize data from the ADS1299 chip
## Getting Started

It is highly advised to create a conda environment to run this project successfully 
### Prerequisites

Run the following commands 
```
conda create --name <nameYourEnv> --file requirements.txt
```

### Installing
After installing all the requierements go the file MainGUI.py and change the IP address to your own addess.
Go to the arduino file and in the WIFI_Settings.h file and add your router info and IP address
Run Arduino File first and wait until the Serial console has shown you that a connection has been established.
Now run the MainGUI.py file. 



## Authors

* **Guillermo Bernal** - *Initial work* -
* **Walaa Alkahanaizi** - 
* **Junqing Qiao** - 

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Project wouldn't be possible with out the help of friends and collaborators
Walaa Alkahanaizi, Aby Jain, Junqing Qiao.


